/*****
*
* Copyright (C) 2004-2005 Yoann Vandoorselaere <yoann@prelude-ids.org>
* All Rights Reserved
*
* This file is part of the Prelude program.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; see the file COPYING.  If not, write to
* the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*
*****/

#ifndef _LIBPRELUDE_CLIENT_PROFILE_H
#define _LIBPRELUDE_CLIENT_PROFILE_H

#include <unistd.h>
#include <gnutls/gnutls.h>

#include "prelude-inttypes.h"

typedef struct prelude_client_profile prelude_client_profile_t;

int _prelude_client_profile_init(prelude_client_profile_t *cp);

int _prelude_client_profile_new(prelude_client_profile_t **ret);

int prelude_client_profile_new(prelude_client_profile_t **ret, const char *name);

void prelude_client_profile_destroy(prelude_client_profile_t *cp);

void prelude_client_profile_get_analyzerid_filename(prelude_client_profile_t *cp, char *buf, size_t size);

void prelude_client_profile_get_tls_key_filename(prelude_client_profile_t *cp, char *buf, size_t size);

void prelude_client_profile_get_tls_server_ca_cert_filename(prelude_client_profile_t *cp, char *buf, size_t size);

void prelude_client_profile_get_tls_server_keycert_filename(prelude_client_profile_t *cp, char *buf, size_t size);

void prelude_client_profile_get_tls_server_trusted_cert_filename(prelude_client_profile_t *cp, char *buf, size_t size);

void prelude_client_profile_get_tls_client_keycert_filename(prelude_client_profile_t *cp, char *buf, size_t size);

void prelude_client_profile_get_tls_client_trusted_cert_filename(prelude_client_profile_t *cp, char *buf, size_t size);

void prelude_client_profile_get_backup_dirname(prelude_client_profile_t *cp, char *buf, size_t size);

void prelude_client_profile_set_uid(prelude_client_profile_t *cp, uid_t uid);

uid_t prelude_client_profile_get_uid(prelude_client_profile_t *cp);

void prelude_client_profile_set_gid(prelude_client_profile_t *cp, uid_t gid);

gid_t prelude_client_profile_get_gid(prelude_client_profile_t *cp);

int prelude_client_profile_set_name(prelude_client_profile_t *cp, const char *name);

const char *prelude_client_profile_get_name(prelude_client_profile_t *cp);

uint64_t prelude_client_profile_get_analyzerid(prelude_client_profile_t *cp);

void prelude_client_profile_set_analyzerid(prelude_client_profile_t *cp, uint64_t analyzerid);

int prelude_client_profile_get_credentials(prelude_client_profile_t *cp, gnutls_certificate_credentials *credentials);

#endif
