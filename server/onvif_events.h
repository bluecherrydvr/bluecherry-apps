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

#include <string>
#include <atomic>
#include <thread>

struct bc_record;

class onvif_events
{
public:
    explicit onvif_events();
    ~onvif_events();

    void run(bc_record *rec);
    void stop();

private:
    std::atomic<bool> exit_flag;
    bc_record *record;
};

#endif // ONVIF_EVENTS_H

