
#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <memory.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>

#include "service/raid_service.h"

using namespace std;

extern "C" {
	int bc_retreive_device_list ( list< bc_dev_info >& device_list );
	int bc_remove_md( char* dev_name );
	int bc_make_new_md( int dev_count, char* dev_list, int level, char* mount_point );
}
