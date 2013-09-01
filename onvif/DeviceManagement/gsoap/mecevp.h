/*
	mecevp.h

	gSOAP interface for streaming message encryption and decryption

gSOAP XML Web services tools
Copyright (C) 2000-2010, Robert van Engelen, Genivia Inc., All Rights Reserved.
This part of the software is released under one of the following licenses:
GPL, the gSOAP public license, or Genivia's license for commercial use.
--------------------------------------------------------------------------------
gSOAP public license.

The contents of this file are subject to the gSOAP Public License Version 1.3
(the "License"); you may not use this file except in compliance with the
License. You may obtain a copy of the License at
http://www.cs.fsu.edu/~engelen/soaplicense.html
Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Initial Developer of the Original Code is Robert A. van Engelen.
Copyright (C) 2000-2010, Robert van Engelen, Genivia, Inc., All Rights Reserved.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org

This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#ifndef MECEVP_H
#define MECEVP_H

#include "stdsoap2.h"

#ifdef WITH_OPENSSL
#include "../openssl/include/openssl/evp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Expose EVP_PKEY in a portable representation */
#define SOAP_MEC_KEY_TYPE	EVP_PKEY

/******************************************************************************\
 *
 * Supported algorithms
 *
\******************************************************************************/

/** Engine off */
#define SOAP_MEC_NONE			(0)

/** Cipher type (triple DES CBC) */
#define SOAP_MEC_DES_CBC		(0x0010)

/** Cipher type (AES128) */
#define SOAP_MEC_AES128 		(0x0020)

/** Cipher type (AES256) */
#define SOAP_MEC_AES256 		(0x0040)

/** Cipher type (AES512) */
#define SOAP_MEC_AES512 		(0x0080)

/** Encode or decode mode */
#define SOAP_MEC_ENC			(0x1000)

/** Envelope mode */
#define SOAP_MEC_ENV			(0x2000)

/** Enable store (in buffer) instead of streaming mode */
#define SOAP_MEC_STORE			(0x8000)

/* Encode and decode types */

/** Symmetric secret key encryption */
#define SOAP_MEC_ENC_DES_CBC		(SOAP_MEC_DES_CBC | SOAP_MEC_ENC)
#define SOAP_MEC_ENC_AES128		(SOAP_MEC_AES128  | SOAP_MEC_ENC)
#define SOAP_MEC_ENC_AES256		(SOAP_MEC_AES256  | SOAP_MEC_ENC)
#define SOAP_MEC_ENC_AES512		(SOAP_MEC_AES512  | SOAP_MEC_ENC)
/** Symmetric secret key decryption */
#define SOAP_MEC_DEC_DES_CBC		(SOAP_MEC_DES_CBC)
#define SOAP_MEC_DEC_AES128		(SOAP_MEC_AES128)
#define SOAP_MEC_DEC_AES256		(SOAP_MEC_AES256)
#define SOAP_MEC_DEC_AES512		(SOAP_MEC_AES512)

/* Envelope types */

/** Envelope (using RSA public key) encryption */
#define SOAP_MEC_ENV_ENC_DES_CBC	(SOAP_MEC_ENC_DES_CBC | SOAP_MEC_ENV)
#define SOAP_MEC_ENV_ENC_AES128         (SOAP_MEC_ENC_AES128  | SOAP_MEC_ENV)
#define SOAP_MEC_ENV_ENC_AES256         (SOAP_MEC_ENC_AES256  | SOAP_MEC_ENV)
#define SOAP_MEC_ENV_ENC_AES512         (SOAP_MEC_ENC_AES512  | SOAP_MEC_ENV)
/** Envelope (using RSA private key) decryption */
#define SOAP_MEC_ENV_DEC_DES_CBC	(SOAP_MEC_DEC_DES_CBC | SOAP_MEC_ENV)
#define SOAP_MEC_ENV_DEC_AES128 	(SOAP_MEC_DEC_AES128  | SOAP_MEC_ENV)
#define SOAP_MEC_ENV_DEC_AES256 	(SOAP_MEC_DEC_AES256  | SOAP_MEC_ENV)
#define SOAP_MEC_ENV_DEC_AES512 	(SOAP_MEC_DEC_AES512  | SOAP_MEC_ENV)

/** Decryption engine states */
enum SOAP_MEC_STATE { SOAP_MEC_STATE_NONE, SOAP_MEC_STATE_INIT, SOAP_MEC_STATE_IV, SOAP_MEC_STATE_DECRYPT, SOAP_MEC_STATE_FINAL, SOAP_MEC_STATE_FLUSH };

/**
@struct soap_mec_data
@brief The mecevp engine context data
*/
struct soap_mec_data
{ int alg;			/**< The algorithm used */
  enum SOAP_MEC_STATE state;	/**< Decryption state */
  EVP_CIPHER_CTX *ctx;		/**< EVP_CIPHER_CTX context */
  const EVP_CIPHER *type;	/**< type for OpenInit/DecryptInit */
  const EVP_PKEY *pkey;		/**< private key for OpenInit */
  unsigned char ekey[EVP_MAX_KEY_LENGTH];/**< ephemeral key */
  const unsigned char *key;	/**< secret key */
  int keylen;			/**< secret key length */
  char *buf;			/**< iv and stream buffer */
  size_t bufidx;		/**< current buffer index */
  size_t buflen;		/**< buffer max length */
  char *rest;			/**< non-encryption "flush" buffer */
  size_t restlen;		/**< non-encryption "flush" buffer length */
  int i;			/**< base64 conversion */
  unsigned long m;		/**< base64 conversion */
  soap_mode mode;
  int (*ffiltersend)(struct soap*, const char**, size_t*);
  int (*ffilterrecv)(struct soap*, char*, size_t*, size_t);
};

/******************************************************************************\
 *
 * soap_mec API functions
 *
\******************************************************************************/

int soap_mec_begin(struct soap *soap, struct soap_mec_data *data, int alg, SOAP_MEC_KEY_TYPE *pkey, unsigned char *key, int *keylen);
int soap_mec_start(struct soap *soap, const unsigned char *key);
int soap_mec_stop(struct soap *soap);
int soap_mec_end(struct soap *soap, struct soap_mec_data *data);

size_t soap_mec_size(int alg, SOAP_MEC_KEY_TYPE *pkey);

int soap_mec_init(struct soap *soap, struct soap_mec_data *data, int alg, SOAP_MEC_KEY_TYPE *pkey, unsigned char *key, int *keylen);
int soap_mec_update(struct soap *soap, struct soap_mec_data *data, const char **s, size_t *n);
int soap_mec_final(struct soap *soap, struct soap_mec_data *data, const char **s, size_t *n);
void soap_mec_cleanup(struct soap *soap, struct soap_mec_data *data);

#ifdef __cplusplus
}
#endif

#endif
