#include "onvif_events.h"
#include "bc-server.h"
#include "trigger_server.h"
#include "trigger_processor.h"

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

#include <thread>

onvif_events::onvif_events()
	: rec(0), exit_flag(false), onvif_tool_pid(-1)
{
}

void onvif_events::run(struct bc_record *r)
{
	while(!exit_flag)
		run_onvif_tool(r);
}

void onvif_events::run_onvif_tool(struct bc_record *r)
{
	int stdout_fds[2] = {-1, -1};
	//int stderr_fds[2] = {-1, -1};
	FILE *onvif_tool_stdout;
	char read_buf[2048];

	this->rec = r;

	r->log.log(Info, "Starting onvif events thread %zu for device %u ...", 
			   std::hash<std::thread::id>{}(std::this_thread::get_id()), r->id);

	char *devaddr = strdupa(r->cfg.dev);
	char *split = strchr(devaddr, '|');
	*split = '\0';

	sprintf(read_buf, "%s:%u", devaddr, r->cfg.onvif_port);
	this->addr = read_buf;
	this->username = r->cfg.rtsp_username;
	this->password = r->cfg.rtsp_password;

	if (pipe(stdout_fds) == -1) {
		r->log.log(Error, "Failed to create stdout pipe for onvif_tool");
		return;
	}

	pid_t pid = fork();

	if (pid == -1) {
		r->log.log(Error, "Failed to fork...");
		return;
	}
	else if (pid == 0) {
		int ret;

		while ((ret = dup2(stdout_fds[1], STDOUT_FILENO)) == -1 && (errno == EINTR));

		if (ret == -1)
			exit(-1);

		close(stdout_fds[1]);
		close(stdout_fds[0]);

		/* Prevent descriptor leakage from parent process */
		for (int i = 3; i < 1024; i++)
			close(i);

		execl("/usr/lib/bluecherry/onvif_tool",
			"/usr/lib/bluecherry/onvif_tool",
			addr.c_str(),			
			username.c_str(),
			password.c_str(),
			"events_subscribe",
			(char*)0);
		exit(-1);
	}

	onvif_tool_pid = pid;
	close(stdout_fds[1]);

	//catch the first string, save subscription reference
	//on each new line call the trigger, pass the topic
	//check stderr & if child proc is alive
	onvif_tool_stdout = fdopen(stdout_fds[0], "r");

	if (!onvif_tool_stdout)
		goto thread_exit;
	
	if (!fgets(read_buf, sizeof read_buf, onvif_tool_stdout))
		goto thread_exit;

	subscription_ref_addr = read_buf;

	r->log.log(Info, "Subscribed to ONVIF events at %s", subscription_ref_addr.c_str());

	while(fgets(read_buf, sizeof read_buf, onvif_tool_stdout))
	{
		trigger_processor *proc = trigger_server::Instance().find_processor(r->id);

		if (proc) {
			proc->trigger(read_buf);
			bc_db_query("INSERT INTO OnvifEvents (device_id,event_time,onvif_topic)"
					" VALUES(%i, NOW(), '%.255s')",
					r->id, read_buf);
		} else {
			r->log.log(Error, "Unable to find trigger processor for ONVIF event");
			break;
		}
	}

thread_exit:
	r->log.log(Info, "ONVIF events thread exiting... ");
	unsubscribe();
	fclose(onvif_tool_stdout);
}

void onvif_events::unsubscribe()
{
	if (subscription_ref_addr.size() > 0) {
		char buf[4096];
		FILE *pf;

		sprintf(buf, "/usr/lib/bluecherry/onvif_tool %s %s %s events_unsubscribe %s",
			addr.c_str(),
			username.c_str(),
			password.c_str(),
			subscription_ref_addr.c_str());

		pf = popen(buf, "r");

		if (pf && fgets(buf, 4, pf) && strncmp("OK", buf, 2) == 0)
			rec->log.log(Info, "Successfully unsubscribed from ONVIF events");
		else
			rec->log.log(Error, "Failed to unsubscribe from ONVIF events");
		pclose(pf);
		subscription_ref_addr.clear();
	}
}

void onvif_events::stop()
{
	if (onvif_tool_pid == -1)
		return;
	exit_flag = true;
	kill(onvif_tool_pid, SIGKILL);
	waitpid(onvif_tool_pid, NULL, 0);
	onvif_tool_pid = -1;
}

