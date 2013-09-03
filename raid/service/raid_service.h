
#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;

struct bc_dev_info
{
	char dev_name[100];
	int  major_number;
	char filesystem[100];
	int  dev_size;
	char raid_level[100];
	list<string> component_devices;
};

struct bc_dev_info;

#define E_CANNOT_DUPLICATE -100
#define E_CANNOT_GET_NEW_NAME -101

typedef int (*retreive_device_list_callback)(list< bc_dev_info >& device_list);
typedef int (*remove_md_callback)(char* dev_name);
typedef int (*make_new_md_callback)(int dev_count, char* dev_list, int level, char* mount_point);

extern "C" {
	void start_rpcserver(int port);
	void set_RetreiveDeviceList_callback(retreive_device_list_callback retreiveDeviceListCallback);
	void set_RemoveMd_callback(remove_md_callback removeMdCallback);
	void set_MakeNewMd_callback(make_new_md_callback makeNewMdCallback);
}
