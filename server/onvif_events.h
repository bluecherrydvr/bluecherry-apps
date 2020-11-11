/*
 * Copyright 2010-2020 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ONVIF_EVENTS_H
#define ONVIF_EVENTS_H

#include <sys/types.h>

#include <string>

class onvif_events
{
public:
	explicit onvif_events();
	void run(struct bc_record *r);
	void stop();
private:
	void run_onvif_tool(struct bc_record *r);
	void unsubscribe();
	struct bc_record *rec;
	bool exit_flag;
	pid_t onvif_tool_pid;
	std::string subscription_ref_addr;
	std::string addr;
	std::string username;
	std::string password;
};

#endif
