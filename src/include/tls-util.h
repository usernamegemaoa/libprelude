/*****
*
* Copyright (C) 2004 Yoann Vandoorselaere <yoann@prelude-ids.org>
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

#ifndef _LIBPRELUDE_TLS_UTIL_H
#define _LIBPRELUDE_TLS_UTIL_H

void tls_unload_file(gnutls_datum *data);

int tls_load_file(const char *filename, gnutls_datum *data);

gnutls_x509_crt *tls_certificates_load(const char *filename, gnutls_x509_privkey *key);

gnutls_x509_crt *tls_certificates_search(gnutls_x509_crt *tbl, const char *dn, size_t size);

#endif
