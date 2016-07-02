
#include <stdio.h>

#include "RaidMgm.h"

#define RPC_PORT 7000

int main()
{
	set_RetreiveDeviceList_callback(bc_retreive_device_list);
	set_RemoveMd_callback(bc_remove_md);
	set_MakeNewMd_callback(bc_make_new_md);

	start_rpcserver(RPC_PORT);

	return 0;
}
