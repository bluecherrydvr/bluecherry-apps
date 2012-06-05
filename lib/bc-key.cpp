/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <set>
#include <string>
#include <bsd/string.h>

#ifndef BC_KEY_STANDALONE
#include <libbluecherry.h>
#endif

int bc_license_machine_id(char *out, int out_sz);

static unsigned short const crc16_table[256] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

static unsigned short crc16_byte(unsigned short crc, const unsigned char data)
{
	return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}


static unsigned short crc16(unsigned char const *buf, size_t len)
{
	unsigned short crc = 0;

	while (len--)
		crc = crc16_byte(crc, *buf++);

	return crc;
}

static const char base32_charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
// subtract 50
static const char base32_charset_reverse[] = {
	26, 27, 28, 29, 30, 31, -1, -1, -1, -1,
	-1, -1, -1, -1, -1,  0,  1,  2,  3,  4,
	 5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25
};

/* Implements base32 encoding as in rfc3548. Requires that srclen*8 is a multiple of 5. */
static void base32_encode(char *dest, unsigned destlen, const char *src, unsigned srclen)
{
	unsigned i, bit, v, u;
	unsigned nbits = srclen * 8;

	for (i = 0, bit = 0; bit < nbits; ++i, bit += 5) {
		if (bit && !(bit % 20))
			dest[i++] = '-';

		/* set v to the 16-bit value starting at src[bits/8], 0-padded. */
		v = ((uint8_t) src[bit / 8]) << 8;
		if (bit + 5 < nbits)
			v += (uint8_t) src[(bit/8)+1];

		/* set u to the 5-bit value at the bit'th bit of src. */
		u = (v >> (11 - (bit % 8))) & 0x1F;
		dest[i] = base32_charset[u];

	}

	dest[i] = '\0';
}

/* Implements base32 decoding as in rfc3548. Requires that srclen*5 is a multiple of 8. */
static bool base32_decode(char *dest, unsigned destlen, const char *src, unsigned srclen)
{
	unsigned int i, j, bit;
	unsigned nbits = 0;
	char *tmp = new char[srclen];

	/* Convert base32 encoded chars to the 5-bit values that they represent. */
	for (i = 0, j = 0; j < srclen; ++j) {
		if (src[j] >= 50 && src[j] < 91)
			tmp[i++] = base32_charset_reverse[src[j] - 50];
		else if (src[j] == '-')
			continue;
		else
			tmp[i++] = -1;

		if (tmp[i-1] == -1) {
			delete[] tmp;
			return false;
		}
	
		nbits = i * 5;
	}

	if ((nbits % 8) || (nbits/8) > destlen) {
		delete[] tmp;
		return false;
	}

	/* Assemble result byte-wise by applying five possible cases. */
	for (i = 0, bit = 0; bit < nbits; ++i, bit += 8) {
		switch (bit % 40) {
		case 0:
			dest[i] = (((uint8_t)tmp[(bit/5)]) << 3) + (((uint8_t)tmp[(bit/5)+1]) >> 2);
			break;
		case 8:
			dest[i] = (((uint8_t)tmp[(bit/5)]) << 6) + (((uint8_t)tmp[(bit/5)+1]) << 1)
			          + (((uint8_t)tmp[(bit/5)+2]) >> 4);
			break;
		case 16:
			dest[i] = (((uint8_t)tmp[(bit/5)]) << 4) + (((uint8_t)tmp[(bit/5)+1]) >> 1);
			break;
		case 24:
			dest[i] = (((uint8_t)tmp[(bit/5)]) << 7) + (((uint8_t)tmp[(bit/5)+1]) << 2)
			          + (((uint8_t)tmp[(bit/5)+2]) >> 3);
			break;
		case 32:
			dest[i] = (((uint8_t)tmp[(bit/5)]) << 5) + ((uint8_t)tmp[(bit/5)+1]);
			break;
		}
	}

	delete[] tmp;
	return true;
}

#ifdef GENERATE_LICENSES
int bc_license_generate(char *dest, int dest_sz, int ndev)
{
	uint8_t raw[10] = { 0 };

	if (dest_sz < 21)
		return -EINVAL;

	for (int i = 5; i < sizeof(raw); ++i)
		raw[i] = uint8_t(rand());

	raw[2] = 'B' ^ raw[7];
	raw[3] = 0 ^ raw[8];
	raw[4] = uint8_t(ndev*4) ^ raw[9];

	uint16_t crc = crc16(raw+2, sizeof(raw)-2);
	raw[0] = uint8_t(crc >> 8) ^ raw[5];
	raw[1] = uint8_t(crc & 0xff) ^ raw[6];

	base32_encode(dest, 21, (const char*)raw, sizeof(raw));
	return strlen(dest);
}
#endif

int bc_license_check(const char *key)
{
	uint8_t data[10];
	if (!base32_decode((char*)data, sizeof(data), key, strlen(key)))
		return 0;

	unsigned short crc = crc16(data+2, sizeof(data)-2);
	unsigned short vcrc = data[0] ^ data[5];
	vcrc = (vcrc << 8) | (data[1] ^ data[6]);

	if (crc != vcrc)
		return 0;

	if ((data[2] ^ data[7]) != 'B' || (data[3] ^ data[8]) != 0)
		return 0;

	int ndev = data[4] ^ data[9];
	ndev /= 4;

	return ndev;
}

/* Ideally, auth should involve a one-way algorithm that is difficult
 * for the client to calculate, but for simplicity's sake, this works. */
#ifndef GENERATE_LICENSES
static
#endif
int bc_license_generate_auth(char *dest, int dest_sz, const char *key, const char *machine)
{
	char data[15];

	if (dest_sz < 10)
		return -EINVAL;

	if (!base32_decode(data, 10, key, strlen(key)))
		return 0;

	if (!base32_decode(data+10, 5, machine, strlen(machine)))
		return 0;

	uint16_t crcr = crc16((const uint8_t*)data, 15);
	uint8_t crc[2] = { uint8_t(crcr << 8), uint8_t(crcr & 0xff) };

	data[0] ^= uint8_t(data[14] * crc[1]);
	data[1] ^= uint8_t(data[13] * crc[0]);
	data[2] ^= uint8_t(data[12] * crc[1]);
	data[3] ^= uint8_t(data[11] * crc[0]);
	data[4] ^= uint8_t(data[10] * crc[1]);

	base32_encode(dest, 10, data, 5);
	return 9;
}

int bc_license_check_auth(const char *key, const char *auth)
{
	char calc[10], machine[10], raw_u[5], raw_c[5];

	if (!base32_decode(raw_u, 5, auth, strlen(auth)))
		return 0;

	if (bc_license_machine_id(machine, sizeof(machine)) < 9)
		return -1;
	if (bc_license_generate_auth(calc, sizeof(calc), key, machine) < 9)
		return 0;
	if (!base32_decode(raw_c, 5, calc, strlen(calc)))
		return 0;

	if (memcmp(raw_u, raw_c, 5) != 0)
		return 0;
	return 1;
}

int bc_license_machine_id(char *out, int out_sz)
{
	char buf[1024];
	char id_buf[6];
	struct ifconf ifc;
	struct ifreq *ifr;
	int fd;
	int re = -1;
	int if_count;
	int i;

	if (out_sz < 10)
		return -ENOBUFS;

	memset(id_buf, 0, sizeof(id_buf));

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return -errno;

	memset(&ifc, 0, sizeof(ifc));
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;

	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
		re = -errno;
		goto end;
	}

	ifr = ifc.ifc_req;
	if_count = ifc.ifc_len / sizeof(struct ifreq);

	for (i = 0; i < if_count; ++i) {
		if (ioctl(fd, SIOCGIFHWADDR, &ifr[i]) < 0) {
			re = -errno;
			continue;
		}

		if (i && strncmp(ifr[i].ifr_name, "eth", 3) && strncmp(ifr[i].ifr_name, "wlan", 4))
			continue;

		memcpy(id_buf, ifr[i].ifr_hwaddr.sa_data, 6);
		if (i)
			break;
	}

	base32_encode(out, 10, id_buf+1, 5);
	re = 9;

end:
	close(fd);
	return re;
}

#ifndef BC_KEY_STANDALONE
int bc_read_licenses(std::vector<bc_license> &licenses)
{
	std::set<std::string> existingset, newset;
	for (int i = 0; i < licenses.size(); i++)
		existingset.insert(licenses[i].license);

	BC_DB_RES dbres = bc_db_get_table("SELECT * FROM Licenses");
	if (!dbres)
		return -1;

	while (!bc_db_fetch_row(dbres)) {
		const char *license = bc_db_get_val(dbres, "license", NULL);
		const char *auth    = bc_db_get_val(dbres, "authorization", NULL);
		if (!license)
			break;

		if (newset.find(license) != newset.end()) {
			bc_log("W(License): Ignoring duplicate license %s", license);
			continue;
		}

		if (existingset.find(license) != existingset.end()) {
			newset.insert(license);
			continue;
		}

		bc_license l;
		strlcpy(l.license, license, sizeof(l.license));
		strlcpy(l.authorization, auth, sizeof(l.authorization));
		l.n_devices = bc_license_check(license);
		if (l.n_devices < 1) {
			bc_log("W(License): Invalid license key: %s", license);
			continue;
		}
		if (bc_license_check_auth(license, auth) < 1) {
			bc_log("W(License): Invalid license authorization for "
			       "'%s' (machine ID changed?)", license);
			continue;
		}

		licenses.push_back(l);
		newset.insert(license);
	}

	bc_db_free_table(dbres);

	// Anything that is not in newset is removed
	for (int i = 0; i < licenses.size(); ) {
		if (newset.find(licenses[i].license) == newset.end())
			licenses.erase(licenses.begin() + i);
		else
			i++;
	}
	return licenses.size(); 
}
#endif

