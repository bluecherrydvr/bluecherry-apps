/*
 * Copyright (C) 2013 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include "RaidMgm.h"


using namespace std;

/* Global Definitions*/

#define SEPERATOR " "
#define TRUE  1
#define FALSE 0

#define E_CANNOT_DUPLICATE -100
#define E_CANNOT_GET_NEW_NAME -101
/* global variables*/

static list< bc_dev_info > hdd_list;
static map< int, string > filesystem_table;
static int b_init = 0;
/************************************************************************************/
/*  Function Name    :    bc_get_major_num                                          */
/*  Function Desc    :             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
int bc_get_major_num( char *dev_name )
{
	char strcmd[100];
	sprintf( strcmd, "ls -l %s", dev_name );
	FILE* stream = popen( strcmd, "r" );
	
	ostringstream output;

	while( !feof( stream ) && !ferror( stream ) )
	{
		char buf[128];
		int bytesRead = fread( buf, 1, 128, stream );
		output.write( buf, bytesRead );
	}
	
	string result = output.str();
	int i_size = result.length() + 1;
	char *output_result = (char*)malloc( i_size );
	memset( output_result, 0, i_size );
	strcpy( output_result, result.c_str() );

	char* token = NULL;
	token = strtok( output_result, SEPERATOR );
	int i_column = 0;
	int major = 0;
	while( token != NULL )
	{
		if ( i_column == 4 )
		{
			major = strtol( token, NULL, 10 );
			break;
		}
		i_column++;
		token = strtok( NULL, SEPERATOR );
	}
	free( output_result );
	return major;
}

/************************************************************************************/
/*  Function Name    :    bc_md_info                                                */
/*  Function Desc    :             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
void bc_md_info( char *dev_name, bc_dev_info* info )
{

	char short_dev_name[50];
	char* pos = strstr( dev_name, "/md" );
	strcpy( short_dev_name, pos + 1 );

	system( "cat /proc/mdstat > tmp_mdstat" );
	FILE *fp = fopen( "tmp_mdstat", "r" );

	//waiting until tempfile was created
	while( fp == NULL )
		sleep( 1 );
	fclose( fp );

	ifstream md_info;
	md_info.open( "tmp_mdstat" );
	string read_line;

	while ( !md_info.eof() )
	{
		getline( md_info, read_line );
		if ( read_line.find( short_dev_name ) != string::npos ) //found entry
		{
			int string_size = read_line.length() + 1;
			char *line_string = (char*)malloc( string_size );
			memset( line_string, 0, string_size );
			strcpy( line_string, read_line.c_str() );

			char* token = NULL;
			int column = 0;
			int i_pos = 0;
			token = strtok( line_string, SEPERATOR );
			while( token != NULL )
			{
				if ( strstr(token, "raid") != NULL )
				{

					strcpy( info->raid_level, token );

					for( int x = 0; info->raid_level[x] != '\0'; x++ ) 
						info->raid_level[x] = toupper( info->raid_level[x] );

					i_pos = column;
				}
				else if ( i_pos != 0 && column > i_pos )
				{
					char* pos = strstr( token, "[");

					/*char* devices = info->component_devices;
					int size = strlen( devices );
					strcat( devices + size, " /dev/" );
					size = strlen( devices );
					strncat( devices + size, token, pos - token);*/
					char dev[50];
					char tmp_dev[50];
					memset( dev, 0, 50 );
					memset( tmp_dev, 0, 50 );

					strncpy( tmp_dev, token, pos - token );
					sprintf( dev, "/dev/%s", tmp_dev );
					string device;
					device = dev;
					info->component_devices.push_back( device );
				}
				column++;
				token = strtok( NULL, SEPERATOR );
			}
		}
	}
	remove( "tmp_mdstat" );
}

/************************************************************************************/
/*  Function Name    :    bc_get_device_info                                        */
/*  Function Desc    :             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
void bc_get_device_info()
{
	hdd_list.clear();
	
	system( "/sbin/sfdisk -s > tmp_disk_size" );

	FILE *fp = fopen( "tmp_disk_size", "r" );
	
	//waiting until temp file was created
	while ( fp == NULL )
		sleep( 1 );
	fclose( fp );
	
	ifstream disk_info;
	disk_info.open( "tmp_disk_size" );
	string read_line;

	// parsing the result
	while ( !disk_info.eof() )
	{
		getline( disk_info, read_line );
		
		int i_chars = read_line.length() + 1;

		char *strLine = (char*)malloc( i_chars );
		memset( strLine, 0, i_chars );
		strcpy( strLine, read_line.c_str() );

		if ( strLine[0] == '/' )
		{
			char* pos = strstr( strLine, ":" );
			char dev_name[100];
			char dev_size[100];

			memset( dev_name, 0, 100 );
			memset( dev_size, 0, 100 );

			// device name
			strncpy( dev_name, strLine, ( pos - strLine ) );

			// storage size
			strcpy( dev_size, pos + 1 );

			// add new device information entry
			bc_dev_info hdd;

			/* init all device information*/
			hdd.component_devices.clear();
			
			strcpy( hdd.dev_name, dev_name );
			hdd.major_number = bc_get_major_num( dev_name );
			hdd.dev_size = strtol( dev_size, NULL, 10 );

			strcpy( hdd.filesystem,  filesystem_table[hdd.major_number].c_str() );
			strcpy( hdd.raid_level, "");
			if ( strcmp( hdd.filesystem, "md" ) == 0 )
			{
				bc_md_info( dev_name, &hdd );
			}
			hdd_list.push_back( hdd );
		}
		free( strLine );
	}

	// delete temp file
	remove( "tmp_disk_size" );

}

/************************************************************************************/
/*  Function Name    :    bc_show_dev_infos                                         */
/*  Function Desc    :             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
void bc_show_dev_infos()
{
	list< bc_dev_info >::iterator it = hdd_list.begin();
	for ( it = hdd_list.begin(); it != hdd_list.end(); it++ )
	{
		//FIXME : just debugging.
		cout << (*it).dev_name << "(" << (*it).filesystem << ")" << " : ";
		printf( "%.2fGB ",  (float)( (*it).dev_size / 1024) /1024 );
		
		if ( (*it).component_devices.size() != 0 )
		{
			cout << (*it).raid_level << " ";
			list<string>::iterator iter = (*it).component_devices.begin();
			for (; iter != (*it).component_devices.end(); iter ++)
			{
				cout << (*iter).c_str() << ",";
			}
		}
		cout << endl;
	}
}

/************************************************************************************/
/*  Function Name    :    bc_load_filesystem_table                                  */
/*  Function Desc    :             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
void bc_load_filesystem_table()
{
	system( "cat /proc/devices > tmp_file_systems" );

	FILE *fp = fopen( "tmp_file_systems", "r" );
	
	//waiting until temp file was created
	while ( fp == NULL )
		sleep( 1 );
	fclose( fp );

	ifstream filesystem;
	filesystem.open( "tmp_file_systems" );
	string read_line;
	
	while ( !filesystem.eof() )
	{
		getline( filesystem, read_line );

		int i_strings = read_line.length() + 1;
		char *str_line = (char*)malloc( i_strings );
		memset( str_line, 0, i_strings );
		strcpy( str_line, read_line.c_str() );

		// skip category string
		char *pos = strstr( str_line, ":" );
		if (pos != NULL ) 
		{
			free( str_line );
			continue;
		}

		// tokenize the data
		char *token = NULL;
		int i_column = 0;
		int major_num = 0;
		
		token = strtok( str_line, SEPERATOR );
		string filesystem_type;
		while( token != NULL )
		{
			if ( i_column == 0 )
			{
				major_num = strtol( token, NULL, 10 );
				i_column++;
			}
			else
			{
				filesystem_type = token;
				filesystem_table[major_num] = filesystem_type;
			}
			token = strtok( NULL, SEPERATOR );
		}
		free( str_line );
	}
	remove( "tmp_file_systems" );
}

/************************************************************************************/
/*  Function Name    :    bc_get_available_md                                       */
/*  Function Desc    :             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
int bc_get_available_md( char *dev_name )
{
	char tmp_md_name[50];
	int i = 0;

	for ( i = 0; i < 128; i++ )
	{
		sprintf(tmp_md_name, "/dev/md%d", i);
		FILE *fp = fopen(tmp_md_name, "r");
		if (fp == NULL )
		{
			strcpy( dev_name, tmp_md_name );
			return TRUE;
		}
		fclose( fp );
	}
	strcpy( tmp_md_name, "" );
	return FALSE;
}

/************************************************************************************/
/*  Function Name    :    bc_check_duplication_device                               */
/*  Function Desc    :             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
int bc_check_duplication_device( list<string> dev_list )
{
	
	list< string >::iterator iter = dev_list.begin();
	for( ; iter != dev_list.end(); iter ++ )
	{
		string device = (*iter);
		list< bc_dev_info >::iterator it = hdd_list.begin();
		for ( it = hdd_list.begin(); it != hdd_list.end(); it++ )
		{
			if ( (*it).component_devices.size() != 0 )
			{
				list<string>::iterator iter = (*it).component_devices.begin();
				for (; iter != (*it).component_devices.end(); iter ++)
				{
					if( (*iter).compare(device) == 0 )
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}

/************************************************************************************/
/*  Function Name    :    bc_make_new_md                                            */
/*  Function Desc    :    make the md                                               */
/*                        dev_count   : device count for new md                     */
/*                        dev_list    : list of devices for new md                  */
/*                                      (e.g: /dev/sda /dev/sdb /dev/sdc )          */
/*                        level       : RAID level                                  */
/*                        mount_point : mount point for new md                      */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
int bc_make_new_md( int dev_count, char* dev_list, int level, char* mount_point )
{
	char cmd_string[500];
	char dev_name[50];
	char tmp_dev_list[500];
	strcpy( tmp_dev_list, dev_list );

	memset( cmd_string, 0, 500 );
	memset( dev_name, 0, 50 );

	/* get available new md device name */
	int ret = bc_get_available_md( dev_name );
	if ( ret == FALSE ) return E_CANNOT_GET_NEW_NAME;

	char *token = NULL;
	list<string> list_devices;
	token = strtok( tmp_dev_list, SEPERATOR );
	while ( token != NULL )
	{
		string str = token;
		list_devices.push_back( str );
		token = strtok( NULL, SEPERATOR );
	}
	
	int result = bc_check_duplication_device( list_devices );
	if ( result == FALSE )
	{
		printf("Cannot create md with duplicated device.\n"); 
		return E_CANNOT_DUPLICATE;
	}

	sprintf( cmd_string, "echo y | /sbin/mdadm -C %s -f -l %d --raid-devices=%d %s",
	        dev_name, level, dev_count, dev_list );

	system( cmd_string );

	int try_count = 3;
	while ( try_count != 0 )
	{
		FILE *fp = fopen( dev_name, "r" );
		if ( fp == NULL )
		{
			sleep( 1 );
			try_count --;
			continue;
		}
		fclose( fp );
		break;
	}
	if ( try_count == 0 ) return FALSE;

	/* format as ext4 file system */
	sprintf( cmd_string, "/sbin/mkfs.ext4 %s", dev_name );
	system( cmd_string );

	/* mount new md to file system */
	sprintf( cmd_string, "mkdir -p %s", mount_point );
	system( cmd_string );

	sprintf( cmd_string, "mount %s %s", dev_name, mount_point );
	system( cmd_string );

	return 0;
}
	
/************************************************************************************/
/*  Function Name    :    bc_remove_md                                              */
/*  Function Desc    :    remove the md                                             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
int bc_remove_md( char* dev_name )
{
	if( dev_name == NULL || strlen( dev_name ) == 0 )
		return FALSE;

	char cmd_string[100];
	memset( cmd_string, 0, 100 );

	/*unmount the target device */
	sprintf( cmd_string, "umount %s", dev_name );
	system( cmd_string );

	/*stop md device*/
	sprintf( cmd_string, "/sbin/mdadm -f --stop %s", dev_name );
	system( cmd_string );
	
	return 0;
}

/************************************************************************************/
/*  Function Name    :    bc_retreive_device_list                                   */
/*  Function Desc    :    retreive the device list                                  */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
int bc_retreive_device_list ( list< bc_dev_info >& device_list )
{
	if (b_init == 0)
	{
		bc_load_filesystem_table();
		b_init = 1;
	}
	bc_get_device_info();
	
	device_list.clear();

	list< bc_dev_info >::iterator it = hdd_list.begin();
	for ( ; it != hdd_list.end(); it++ )
	{
		device_list.push_back( *it );
	}
	return 0;
}

