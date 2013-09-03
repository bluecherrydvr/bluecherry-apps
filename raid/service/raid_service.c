/* A simple standalone XML-RPC server written in C. */

/* This server knows one RPC class (besides the system classes):
   "sample.add".

   The program takes one argument: the HTTP port number on which the server
   is to accept connections, in decimal.

   You can use the example program 'xmlrpc_sample_add_client' to send an RPC
   to this server.

   Example:

   $ ./xmlrpc_sample_add_server 8080&
   $ ./xmlrpc_sample_add_client

   For more fun, run client and server in separate terminals and turn on
   tracing for each:

   $ export XMLRPC_TRACE_XML=1
*/

#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include "raid_service.h"

#ifdef WIN32
  #define SLEEP(seconds) SleepEx(seconds * 1000, 1);
#else
  #define SLEEP(seconds) sleep(seconds);
#endif

#define PARAMS(count) sleep(seconds);

using namespace std;

retreive_device_list_callback g_retreiveDeviceListCallback;
remove_md_callback g_removeMdCallback;
make_new_md_callback g_makeNewMdCallback;

static xmlrpc_value *
retreive_device_list(xmlrpc_env *   const envP,
           xmlrpc_value * const paramArrayP,
           void *         const serverInfo,
           void *         const channelInfo) {

	list< bc_dev_info > device_list;

	int result = g_retreiveDeviceListCallback(device_list);
	
	xmlrpc_value* myArrayP;
	xmlrpc_value* myStructP;
	xmlrpc_value* memberValueP;

	myArrayP = xmlrpc_array_new(envP);

	list< bc_dev_info >::iterator it = device_list.begin();
	for ( it = device_list.begin(); it != device_list.end(); it++ )
	{
		myStructP = xmlrpc_struct_new(envP);

		memberValueP = xmlrpc_string_new(envP, (*it).dev_name);
		xmlrpc_struct_set_value(envP, myStructP, "dev_name", memberValueP);
		xmlrpc_DECREF(memberValueP);

		memberValueP = xmlrpc_int_new(envP, (*it).major_number);
		xmlrpc_struct_set_value(envP, myStructP, "major_number", memberValueP);
		xmlrpc_DECREF(memberValueP);

		memberValueP = xmlrpc_string_new(envP, (*it).filesystem);
		xmlrpc_struct_set_value(envP, myStructP, "device_type", memberValueP);
		xmlrpc_DECREF(memberValueP);

		memberValueP = xmlrpc_int_new(envP, (*it).dev_size);
		xmlrpc_struct_set_value(envP, myStructP, "dev_size", memberValueP);
		xmlrpc_DECREF(memberValueP);

		memberValueP = xmlrpc_string_new(envP, (*it).raid_level);
		//memberValueP = xmlrpc_string_new(envP, "test");
		xmlrpc_struct_set_value(envP, myStructP, "raid_level", memberValueP);
		xmlrpc_DECREF(memberValueP);

		memberValueP = xmlrpc_array_new(envP);
		if ( (*it).component_devices.size() != 0 )
		{
			list<string>::iterator iter = (*it).component_devices.begin();
			for (; iter != (*it).component_devices.end(); iter ++)
			{
				xmlrpc_value* subValueP = xmlrpc_string_new(envP, (*iter).c_str());
				xmlrpc_array_append_item(envP, memberValueP, subValueP);
				xmlrpc_DECREF(subValueP);
			}
		}
		xmlrpc_struct_set_value(envP, myStructP, "component_devices", memberValueP);
		xmlrpc_DECREF(memberValueP);

		xmlrpc_array_append_item(envP, myArrayP, myStructP);
		xmlrpc_DECREF(myStructP);

	}

	xmlrpc_value* rpcValue = xmlrpc_build_value(envP, "A", myArrayP);

	//xmlrpc_DECREF(myArrayP);

	device_list.clear();

	return rpcValue;
}

static xmlrpc_value *
remove_md(xmlrpc_env *   const envP,
           xmlrpc_value * const paramArrayP,
           void *         const serverInfo,
           void *         const channelInfo) {

	char* dev_name;

	/* Parse our argument array. */
	xmlrpc_decompose_value(envP, paramArrayP, "(s*)", &dev_name);
	if (envP->fault_occurred)
		return NULL;
		
	int result = g_removeMdCallback(dev_name);
	
	/* Return our result. */
	xmlrpc_value* rpcValue = xmlrpc_build_value(envP, "{s:s}", "result", "success");
	
	return rpcValue;
}

static xmlrpc_value *
make_new_md(xmlrpc_env *   const envP,
           xmlrpc_value * const paramArrayP,
           void *         const serverInfo,
           void *         const channelInfo) {

	char* dev_count;
	char* dev_list;
	char* level;
	char* mount_point;

	/* Parse our argument array. */
	xmlrpc_decompose_value(envP, paramArrayP, "(ssss*)", &dev_count, &dev_list, &level, &mount_point);

	if (envP->fault_occurred)
		return NULL;

	int int_dev_count = strtol(dev_count, NULL, 10);
	int int_level = strtol(level, NULL, 10);

	int result = g_makeNewMdCallback(int_dev_count, dev_list, int_level, mount_point);
	
	/* Return our result. */
	xmlrpc_value* rpcValue = NULL;
	if (result == E_CANNOT_DUPLICATE )
	{
		rpcValue = xmlrpc_build_value(envP, "{s:s,s:s}", "result", "fail", 
			"message", "Cannot create new md with duplicated device.");
	}
	else if ( result == E_CANNOT_GET_NEW_NAME )
	{
		rpcValue = xmlrpc_build_value(envP, "{s:s,s:s}", "result", "fail", 
			"message", "Cannot get new md name. You should remove any one md device.");
	}
	else
	{
		rpcValue = xmlrpc_build_value(envP, "{s:s,s:s}", "result", "success", "message", "Adding new md was success.");
	}
	
	return rpcValue;
}

void start_rpcserver(int port) {

    struct xmlrpc_method_info3 const methodInfo1 = {
        /* .methodName     = */ "raid.retreive",
        /* .methodFunction = */ &retreive_device_list,
    };
	struct xmlrpc_method_info3 const methodInfo2 = {
        /* .methodName     = */ "raid.remove",
        /* .methodFunction = */ &remove_md,
    };
    struct xmlrpc_method_info3 const methodInfo3 = {
        /* .methodName     = */ "raid.new",
        /* .methodFunction = */ &make_new_md,
    };
	
    xmlrpc_server_abyss_parms serverparm;
    xmlrpc_registry * registryP;
    xmlrpc_env env;

    xmlrpc_env_init(&env);

    registryP = xmlrpc_registry_new(&env);

    xmlrpc_registry_add_method3(&env, registryP, &methodInfo1);
    xmlrpc_registry_add_method3(&env, registryP, &methodInfo2);
    xmlrpc_registry_add_method3(&env, registryP, &methodInfo3);

    /* In the modern form of the Abyss API, we supply parameters in memory
       like a normal API.  We select the modern form by setting
       config_file_name to NULL: 
    */
    serverparm.config_file_name = NULL;
    serverparm.registryP        = registryP;
    serverparm.port_number      = port;
    serverparm.log_file_name    = "/tmp/xmlrpc_log";

    printf("Running XML-RPC server...\n");

    xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(log_file_name));

}

void set_RetreiveDeviceList_callback(retreive_device_list_callback retreiveDeviceListCallback)
{
	g_retreiveDeviceListCallback = retreiveDeviceListCallback;
}

void set_RemoveMd_callback(remove_md_callback removeMdCallback)
{
	g_removeMdCallback = removeMdCallback;
}

void set_MakeNewMd_callback(make_new_md_callback makeNewMdCallback)
{
	g_makeNewMdCallback = makeNewMdCallback;
}

