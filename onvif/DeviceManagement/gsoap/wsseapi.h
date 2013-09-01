/*
	wsseapi.h

	WS-Security plugin.

	See wsseapi.c for documentation and details.

gSOAP XML Web services tools
Copyright (C) 2000-2005, Robert van Engelen, Genivia Inc., All Rights Reserved.
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
Copyright (C) 2000-2005, Robert van Engelen, Genivia Inc., All Rights Reserved.
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

#ifndef WSSEAPI_H
#define WSSEAPI_H

#include "soapH.h"	/* replace with soapcpp2-generated *H.h file */
#include "smdevp.h"
#include "mecevp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** plugin identification for plugin registry */
#define SOAP_WSSE_ID "SOAP-WSSE-1.2"

/** plugin identification for plugin registry */
extern const char soap_wsse_id[];

/**
@struct soap_wsse_data
@brief wsseapi plugin data

The signature key (private) and verification key (public) are kept in the
plugin data, together with other info.
*/
struct soap_wsse_data
{ const char *sigid;		/**< string with wsu:Id names to sign */
  const char *encid;		/**< string with wsu:Id names to encrypt */
  int sign_alg;			/**< The digest or signature algorithm used */
  const void *sign_key;		/**< EVP_PKEY or key string for HMAC */
  int sign_keylen;		/**< HMAC key length */
  int vrfy_alg;			/**< The signature verify algorithm used */
  const void *vrfy_key;		/**< EVP_PKEY or key string for HMAC verify */
  int vrfy_keylen;		/**< HMAC key length */
  int enco_alg;			/**< encrypt algorithm used */
  const void *enco_key;		/**< EVP_PKEY or secret key */
  int enco_keylen;		/**< secret key length */
  int deco_alg;			/**< decrypt algorithm used */
  const void *deco_key;		/**< EVP_PKEY or secret key */
  int deco_keylen;		/**< secret key length */
  struct soap_wsse_digest *digest;	/**< List of ID-hash pairs */
  int (*fpreparesend)(struct soap*, const char*, size_t);
  int (*fpreparefinalsend)(struct soap*);
  int (*fpreparefinalrecv)(struct soap*);
  int (*fheader)(struct soap*);
  struct soap_mec_data *mec;
  X509_STORE *store;
  const void *(*security_token_handler)(struct soap *soap, int alg, const char *keyname, int *keylen);
};

/**
@struct soap_wsse_digest
@brief Digest dictionary: linked list of ID-hash pairs

The digest dictionary is populated by the soap_wsse_preparesend callback. The
callback intercepts XML elements with wsu:Id attributes and computes the digest
during the preprocessing of a message transmission. The 'level' field is used
to determine when the end of an element is reached by handling inner wsu:Id
attributed elements, so that the outer wsu:Id attributed element can be hashed.
*/
struct soap_wsse_digest
{ struct soap_wsse_digest *next;	/**< Next in list */
  unsigned int level;			/**< XML parser depth level */
  struct soap_smd_data smd;		/**< smdevp engine context */
  unsigned char hash[SOAP_SMD_MAX_SIZE];/**< Digest hash value */
  char id[1];				/**< String flows down the struct */
};

extern const char *wsse_PasswordTextURI;
extern const char *wsse_PasswordDigestURI;
extern const char *wsse_Base64BinaryURI;
extern const char *wsse_X509v3URI;
extern const char *wsse_X509v3SubjectKeyIdentifierURI;

extern const char *ds_sha1URI;
extern const char *ds_hmac_sha1URI;
extern const char *ds_dsa_sha1URI;
extern const char *ds_rsa_sha1URI;

extern const char *ds_URI;
extern const char *c14n_URI;
extern const char *wsu_URI;

struct _wsse__Security* soap_wsse_add_Security(struct soap *soap);
struct _wsse__Security* soap_wsse_add_Security_actor(struct soap *soap, const char *actor);
void soap_wsse_delete_Security(struct soap *soap);
struct _wsse__Security* soap_wsse_Security(struct soap *soap);

struct ds__SignatureType* soap_wsse_add_Signature(struct soap *soap);
void soap_wsse_delete_Signature(struct soap *soap);
struct ds__SignatureType* soap_wsse_Signature(struct soap *soap);

int soap_wsse_add_Timestamp(struct soap *soap, const char *id, time_t lifetime);
struct _wsu__Timestamp *soap_wsse_Timestamp(struct soap *soap);
int soap_wsse_verify_Timestamp(struct soap *soap);

int soap_wsse_add_UsernameTokenText(struct soap *soap, const char *id, const char *username, const char *password);
int soap_wsse_add_UsernameTokenDigest(struct soap *soap, const char *id, const char *username, const char *password);
struct _wsse__UsernameToken *soap_wsse_UsernameToken(struct soap *soap, const char *id);
const char* soap_wsse_get_Username(struct soap *soap);
int soap_wsse_verify_Password(struct soap *soap, const char *password);

int soap_wsse_add_BinarySecurityToken(struct soap *soap, const char *id, const char *valueType, const unsigned char *data, int size);
int soap_wsse_add_BinarySecurityTokenX509(struct soap *soap, const char *id, X509 *cert);
int soap_wsse_add_BinarySecurityTokenPEM(struct soap *soap, const char *id, const char *filename);
struct _wsse__BinarySecurityToken *soap_wsse_BinarySecurityToken(struct soap *soap, const char *id);
int soap_wsse_get_BinarySecurityToken(struct soap *soap, const char *id, char **valueType, unsigned char **data, int *size);
X509 *soap_wsse_get_BinarySecurityTokenX509(struct soap *soap, const char *id);
int soap_wsse_verify_X509(struct soap *soap, X509 *cert);

struct ds__SignedInfoType *soap_wsse_add_SignedInfo(struct soap *soap);
int soap_wsse_add_SignedInfo_Reference(struct soap *soap, const char *URI, const char *transform, const char *inclusiveNamespaces, const char *HA);
int soap_wsse_add_SignedInfo_SignatureMethod(struct soap *soap, const char *method, int canonical);
struct ds__SignedInfoType *soap_wsse_SignedInfo(struct soap *soap);
int soap_wsse_get_SignedInfo_SignatureMethod(struct soap *soap, int *alg);

int soap_wsse_add_SignatureValue(struct soap *soap, int alg, const void *key, int keylen);
int soap_wsse_verify_SignatureValue(struct soap *soap, int alg, const void *key, int keylen);
int soap_wsse_verify_SignedInfo(struct soap *soap);
int soap_wsse_verify_digest(struct soap *soap, int alg, int canonical, const char *id, unsigned char hash[SOAP_SMD_MAX_SIZE]);

struct ds__KeyInfoType *soap_wsse_add_KeyInfo(struct soap *soap);
struct ds__KeyInfoType *soap_wsse_KeyInfo(struct soap *soap);

int soap_wsse_add_KeyInfo_KeyName(struct soap *soap, const char *name);
const char *soap_wsse_get_KeyInfo_KeyName(struct soap *soap);

int soap_wsse_add_KeyInfo_SecurityTokenReferenceURI(struct soap *soap, const char *URI, const char *valueType);
int soap_wsse_add_KeyInfo_SecurityTokenReferenceX509(struct soap *soap, const char *URI);
const char *soap_wsse_get_KeyInfo_SecurityTokenReferenceURI(struct soap *soap);
const char *soap_wsse_get_KeyInfo_SecurityTokenReferenceValueType(struct soap *soap);
X509 *soap_wsse_get_KeyInfo_SecurityTokenReferenceX509(struct soap *soap);

int soap_wsse_add_KeyInfo_SecurityTokenReferenceKeyIdentifier(struct soap *soap, const char *id, const char *valueType, unsigned char *data, int size);
const char *soap_wsse_get_KeyInfo_SecurityTokenReferenceKeyIdentifierValueType(struct soap *soap);
const unsigned char *soap_wsse_get_KeyInfo_SecurityTokenReferenceKeyIdentifier(struct soap *soap, int *size);

int soap_wsse_add_KeyInfo_SecurityTokenReferenceEmbedded(struct soap *soap, const char *id, const char *valueType);

int soap_wsse_sender_fault_subcode(struct soap *soap, const char *faultsubcode, const char *faultstring, const char *faultdetail);
int soap_wsse_receiver_fault_subcode(struct soap *soap, const char *faultsubcode, const char *faultstring, const char *faultdetail);
int soap_wsse_sender_fault(struct soap *soap, const char *faultstring, const char *faultdetail);
int soap_wsse_receiver_fault(struct soap *soap, const char *faultstring, const char *faultdetail);
int soap_wsse_fault(struct soap *soap, enum wsse__FaultcodeEnum fault, const char *detail);

int soap_wsse(struct soap *soap, struct soap_plugin *p, void *arg);

int soap_wsse_sign(struct soap *soap, int alg, const void *key, int keylen);
int soap_wsse_sign_body(struct soap *soap, int alg, const void *key, int keylen);
int soap_wsse_verify_init(struct soap *soap);
int soap_wsse_verify_auto(struct soap *soap, int alg, const void *key, size_t keylen);
int soap_wsse_verify_done(struct soap *soap);
size_t soap_wsse_verify_element(struct soap *soap, const char *URI, const char *tag);
int soap_wsse_verify_body(struct soap *soap);
int soap_wsse_set_wsu_id(struct soap *soap, const char *tags);
int soap_wsse_sign_only(struct soap *soap, const char *tags);

int soap_wsse_add_EncryptedKey(struct soap *soap, const char *URI, X509 *cert, const char *subjectkeyid);
int soap_wsse_add_EncryptedKey_encrypt_only(struct soap *soap, const char *URI, X509 *cert, const char *subjectkeyid, const char *tags);
int soap_wsse_verify_EncryptedKey(struct soap *soap);
void soap_wsse_delete_EncryptedKey(struct soap *soap);
struct xenc__EncryptedKeyType* soap_wsse_EncryptedKey(struct soap *soap);

int soap_wsse_add_EncryptedKey_DataReferenceURI(struct soap *soap, const char *URI);
int soap_wsse_add_DataReferenceURI(struct soap *soap, const char *URI);

int soap_wsse_encrypt_body(struct soap *soap, int alg, const void *key, int keylen);
int soap_wsse_encrypt_only(struct soap *soap, int alg, const void *key, int keylen, const char *tags);
int soap_wsse_encrypt(struct soap *soap, int alg, const void *key, int keylen);
int soap_wsse_decrypt_auto(struct soap *soap, int alg, const void *key, int keylen);

int soap_wsse_encrypt_begin(struct soap *soap, const char *id, const char *URI, const char *keyname, const unsigned char *key);
int soap_wsse_encrypt_end(struct soap *soap);

int soap_wsse_decrypt_begin(struct soap *soap, const unsigned char *key);
int soap_wsse_decrypt_end(struct soap *soap);

#ifdef __cplusplus
}
#endif

SOAP_FMAC1 void SOAP_FMAC2 soap_default_xsd__anyType(struct soap*, struct soap_dom_element *node);
SOAP_FMAC1 int SOAP_FMAC2 soap_out_xsd__anyType(struct soap *soap, const char *tag, int id, const struct soap_dom_element *node, const char *type);
SOAP_FMAC1 struct soap_dom_element * SOAP_FMAC2 soap_in_xsd__anyType(struct soap *soap, const char *tag, struct soap_dom_element *node, const char *type);

#endif
