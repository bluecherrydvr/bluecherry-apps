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

#include "config.h"  /* information about this build environment */

#ifdef WIN32
  #define SLEEP(seconds) SleepEx(seconds * 1000, 1);
#else
  #define SLEEP(seconds) sleep(seconds);
#endif

typedef int (*get_cameras_callback)(int* count, char** cameras);
typedef int (*get_camerainfo_callback)(char* xAddress, char* username, char* password, char* make, char* model, char* firmware, char* rtspUri, int* rtspPort, char* snapshotUri, int* snapshotPort);

get_cameras_callback g_getCamerasCallback;
get_camerainfo_callback g_getCamerainfoCallback;

static xmlrpc_value *
camera_discovery(xmlrpc_env *   const envP,
           xmlrpc_value * const paramArrayP,
           void *         const serverInfo,
           void *         const channelInfo) {

	int count = 0;
	char* cameras[32];
	char resultString[2048];
	int idx = 0;
	
	memset(resultString, 0, 2048);
	
	for (idx=0; idx<32; idx++)
	{
		cameras[idx] = (char*)malloc(512);
		memset(cameras[idx], 0, 512);
	}
	
	int result = g_getCamerasCallback(&count, cameras);
	
	for (idx=0; idx<count; idx++)
	{
		strcat(resultString, "{");
		strcat(resultString, cameras[idx]);
		strcat(resultString, "}");
	}
	
    /* Return our result. */
    xmlrpc_value* rpcValue = xmlrpc_build_value(envP, "s", resultString);
	
	for (idx=0; idx<32; idx++)
	{
		free(cameras[idx]);
	}

	return rpcValue;
}

static xmlrpc_value *
camera_info(xmlrpc_env *   const envP,
           xmlrpc_value * const paramArrayP,
           void *         const serverInfo,
           void *         const channelInfo) {

	char* cameraToken;
	char* username;
	char* password;
	char make[256];
 	char model[256];
	char firmware[256];
	char rtspUri[512];
	int rtspPort;
	char snapshotUri[512];
	int snapshotPort;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(sss*)", &cameraToken, &username, &password);
    if (envP->fault_occurred)
	    return NULL;
		
	int result = g_getCamerainfoCallback(cameraToken, username, password, make, model, firmware, rtspUri, &rtspPort, snapshotUri, &snapshotPort);
	
    /* Return our result. */
    xmlrpc_value* rpcValue = xmlrpc_build_value(envP, "s", rtspUri);
	
	return rpcValue;
}

void start_rpcserver(int port) {

    struct xmlrpc_method_info3 const methodInfo1 = {
        /* .methodName     = */ "camera.discovery",
        /* .methodFunction = */ &camera_discovery,
    };
	struct xmlrpc_method_info3 const methodInfo2 = {
        /* .methodName     = */ "camera.info",
        /* .methodFunction = */ &camera_info,
    };
	
    xmlrpc_server_abyss_parms serverparm;
    xmlrpc_registry * registryP;
    xmlrpc_env env;

    xmlrpc_env_init(&env);

    registryP = xmlrpc_registry_new(&env);

    xmlrpc_registry_add_method3(&env, registryP, &methodInfo1);
	xmlrpc_registry_add_method3(&env, registryP, &methodInfo2);

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

void set_GetCameras_callback(get_cameras_callback getCamerasFunc)
{
	g_getCamerasCallback = getCamerasFunc;
}

void set_GetCamerainfo_callback(get_camerainfo_callback getCamerainfoFunc)
{
	g_getCamerainfoCallback = getCamerainfoFunc;
}

