/*
 * Copyright 2010-2019 Bluecherry, LLC
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

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>

#include "libbluecherry.h"
#include "utils.h"

#define V3_LICENSING 1

#ifdef V3_LICENSING
#include "licensecmd.h"

int bc_license_v3_init()
{
	int status;

	status = SetProductData(PRODUCT_DATA);
	if (LA_OK != status)
	{
		printf("%s\n", "SetProductData");
		return V3_LICENSE_FAIL;
	}
	status = SetProductId(PRODUCT_ID, LA_SYSTEM);
	if (LA_OK != status)
	{
		printf("%s error code = %d\n", "SetProductId", status);
		return V3_LICENSE_FAIL;
	}

	status = SetAppVersion(PRODUCT_VERSION);
	if (LA_OK != status)
	{
		printf("%s\n", "PRODUCT_VERSION");
		return V3_LICENSE_FAIL;
	}

	return V3_LICENSE_OK;
}

// Ideally on a button click inside a dialog
int activate()
{
	int status;

	status = SetLicenseKey(PRODCUT_KEY);
	if (LA_OK != status)
	{
		return V3_LICENSE_FAIL;
	}

	status = ActivateLicense();
	if (LA_OK == status 
			|| LA_EXPIRED == status 
			|| LA_SUSPENDED == status)
	{
		printf("%s\n", "V3_LICENSE_OK");
		return V3_LICENSE_OK;
	}
	else
	{
		printf("%s\n", "V3_LICENSE_FAIL");
		return V3_LICENSE_FAIL;
	}
}

// Ideally on a button click inside a dialog
int activateTrial()
{
	int status;

	status = ActivateTrial();
	if (LA_OK == status)
	{
		return V3_LICENSE_OK;
	}
	else
	{
		return V3_LICENSE_FAIL;
	}
}

int bc_license_v3_check()
{
	if (bc_license_v3_init() != LA_OK)
	{
		printf("%s\n", "bc_license_v3_init");
		return V3_LICENSE_FAIL;
	}

	int status = IsLicenseGenuine();
	if (LA_OK == status)
	{
		printf("%s\n", "IsLicenseGenuine");
		return V3_LICENSE_OK;
	}
	else if (LA_EXPIRED == status 
			|| LA_SUSPENDED == status 
			|| LA_GRACE_PERIOD_OVER == status)
	{
		printf("%s\n", "LA_EXPIRED or LA_SUSPENDED or LA_GRACE_PERIOD_OVER");
		return V3_LICENSE_OK;
	}
	else
	{
		int trialStatus;
		trialStatus = IsTrialGenuine();
		if (LA_OK == trialStatus)
		{
			printf("Trial days left: %s", "IsTrialGenuine");
			return V3_LICENSE_OK;
		}
		else if (LA_TRIAL_EXPIRED == trialStatus)
		{
			// Time to buy the license and activate the app
			return activate();
		}
		else
		{
			// Activating the trial
			return activateTrial();
		}
	}
	return V3_LICENSE_OK;
}

int main(int argc, char **argv)
{
	const char *name;

	if (argc < 2)
	{
		return V3_LICENSE_FAIL;
	}
	name = argv[1];

	do
	{
		if (strcmp(name, "bc_v3license_check") == 0)
		{
			if (bc_license_v3_check() == V3_LICENSE_OK)
			{
				puts("true");
			}
			else
			{
				puts("0");
			}
			break;
		}

		fprintf(stderr, "unknown function name \"%s\"\n", name);
		return V3_LICENSE_FAIL;
	}
	while(0);

	return V3_LICENSE_OK;
}
#else

void usage(const char *argv0)
{
	printf("%s function_name [ARG1 [ARG2]]\n", argv0);
}

int main(int argc, char **argv)
{
	const char *name, *arg1 = NULL, *arg2 = NULL;

	if (argc < 2)
	{
		usage(argv[0]);
		return -1;
	}

	name = argv[1];

	if (argc >= 3)
		arg1 = argv[2];
	if (argc >= 4)
		arg2 = argv[3];

	if (bc_db_open(BC_CONFIG_DEFAULT))
	{
		fputs("Failed to open bluecherry database\n", stderr);
		return -1;
	}

	do
	{
		if (strcmp(name, "bc_license_machine_id") == 0)
		{
			char buf[64];
			int re = bc_license_machine_id(buf, sizeof(buf));

			if (re < 1)
				return -1;

			puts(buf);
			break;
		}

		if (strcmp(name, "bc_license_check") == 0)
		{
			if (!arg1)
			{
				fputs("missing \"key\" argument for function bc_license_check()\n", stderr);
				return -1;
			}

			int re = bc_license_check(arg1);
			
			printf("%i\n", re); //0 casts to boolean FALSE in php
			break;
		}

		if (strcmp(name, "bc_license_check_auth") == 0)
		{
			if (!arg1 || !arg2)
			{
				fputs("missing \"key\" and \"auth\" arguments for function bc_license_check_auth()\n", stderr);
				return -1;
			}

			if (bc_license_check_auth(arg1, arg2) == 1)
				puts("true");
			else
				puts("0");
			break;
		}

		if (strcmp(name, "bc_license_devices_allowed") == 0)
		{
			std::vector<bc_license> licenses;
			if (bc_read_licenses(licenses) < 0)
			{
				puts("0");
				break;
			}

			int num = 0;

			for (std::vector<bc_license>::iterator it = licenses.begin(); it != licenses.end(); ++it)
				num += it->n_devices;

			printf("%i\n", num);
			break;
		}

		fprintf(stderr, "unknown function name \"%s\"\n", name);
		return -1;

	}
	while(0);

	return 0;
}

#endif /* V3_LICENSING */
