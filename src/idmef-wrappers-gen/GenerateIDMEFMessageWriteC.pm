# Copyright (C) 2003,2004 Nicolas Delon <nicolas@prelude-ids.org>
# All Rights Reserved
#
# This file is part of the Prelude program.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

package GenerateIDMEFMessageWriteC;

use Generate;
@ISA = qw/Generate/;

use strict;
use IDMEFTree;

sub	header
{
     my	$self = shift;

     $self->output("
/*****
*
* Copyright (C) 2001-2005 Yoann Vandoorselaere <yoann\@prelude-ids.org>
* Copyright (C) 2003,2004 Nicolas Delon <nicolas\@prelude-ids.org>
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

/* Auto-generated by the GenerateIDMEFMessageWriteC package */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <assert.h>

#include \"prelude-inttypes.h\"
#include \"prelude-list.h\"
#include \"prelude-log.h\"
#include \"prelude-io.h\"
#include \"prelude-ident.h\"
#include \"prelude-message-id.h\"
#include \"idmef-message-id.h\"
#include \"idmef.h\"
#include \"idmef-tree-wrap.h\"
#include \"idmef-message-write.h\"
#include \"prelude-client.h\"
#include \"common.h\"



/*
 * If you wonder why we do this, and why life is complicated,
 * then wonder why the hell the guys that wrote IDMEF choose to use XML.
 * XML is dog slow. And XML'll never achieve performance needed for real time IDS.
 *
 * Here we are trying to communicate using a home made, binary version of IDMEF.
 */


static inline void prelude_string_write(prelude_string_t *string, prelude_msgbuf_t *msg, uint8_t tag)
\{
        if ( ! string || prelude_string_is_empty(string) )
                return;

        prelude_msgbuf_set(msg, tag, prelude_string_get_len(string) + 1, prelude_string_get_string(string));
\}



static inline void uint64_write(uint64_t data, prelude_msgbuf_t *msg, uint8_t tag) 
\{
        uint64_t dst;
        
        dst = prelude_hton64(data);
        
        prelude_msgbuf_set(msg, tag, sizeof(dst), &dst);
\}



static inline void uint32_write(uint32_t data, prelude_msgbuf_t *msg, uint8_t tag) 
\{        
        data = htonl(data);
        prelude_msgbuf_set(msg, tag, sizeof(data), &data);
\}



static inline void int32_write(uint32_t data, prelude_msgbuf_t *msg, uint8_t tag) 
\{
	uint32_write(data, msg, tag);
\}



static inline void uint8_write(uint8_t data, prelude_msgbuf_t *msg, uint8_t tag)
\{
	prelude_msgbuf_set(msg, tag, sizeof (data), &data);
\}



static inline void uint16_write(uint16_t data, prelude_msgbuf_t *msg, uint8_t tag) 
\{
        data = htons(data);
        prelude_msgbuf_set(msg, tag, sizeof(data), &data);
\}



static inline void float_write(float data, prelude_msgbuf_t *msg, uint8_t tag)
\{
	if ( data == 0.0 )
		return;

	prelude_msgbuf_set(msg, tag, sizeof (data), &data);
\}



inline void idmef_time_write(idmef_time_t *data, prelude_msgbuf_t *msg, uint8_t tag) 
\{
        uint32_t tmp;
        unsigned char buf[12];

        if ( ! data )
                return;
      
        tmp = htonl(idmef_time_get_sec(data));
        memcpy(buf, &tmp, sizeof(tmp));

        tmp = htonl(idmef_time_get_usec(data));
        memcpy(buf + 4, &tmp, sizeof(tmp));

        tmp = htonl(idmef_time_get_gmt_offset(data));
        memcpy(buf + 8, &tmp, sizeof(tmp));

        prelude_msgbuf_set(msg, tag, sizeof (buf), buf);
\}



static inline void idmef_data_write(idmef_data_t *data, prelude_msgbuf_t *msg, uint8_t tag)
\{
	idmef_data_type_t type;

	if ( ! data )
		return;

	type = idmef_data_get_type(data);
	if ( type == IDMEF_DATA_TYPE_UNKNOWN )
		return;

	uint32_write(idmef_data_get_type(data), msg, tag);

	switch ( type ) \{
	case IDMEF_DATA_TYPE_CHAR: case IDMEF_DATA_TYPE_BYTE:
		uint8_write(* (const uint8_t *) idmef_data_get_data(data), msg, tag);
		break;

	case IDMEF_DATA_TYPE_UINT32:
		uint32_write(idmef_data_get_uint32(data), msg, tag);
		break;

	case IDMEF_DATA_TYPE_UINT64:
		uint64_write(idmef_data_get_uint64(data), msg, tag);
		break;

	case IDMEF_DATA_TYPE_FLOAT:
		float_write(idmef_data_get_uint64(data), msg, tag);
		break;

	case IDMEF_DATA_TYPE_CHAR_STRING: case IDMEF_DATA_TYPE_BYTE_STRING:
		prelude_msgbuf_set(msg, tag, idmef_data_get_len(data), idmef_data_get_data(data));
		break;

	case IDMEF_DATA_TYPE_UNKNOWN:
		/* nop */;
	\}
\}

");
}

sub	struct_field_normal
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;
    my	$type = shift || $field->{short_typename};
    my	$function;

    $function = "${type}_write";

    if ( $field->{metatype} & &METATYPE_OPTIONAL_INT ) {
	$self->output("
	{
		$field->{typename} *tmp;

		tmp = idmef_$struct->{short_typename}_get_$field->{name}($struct->{short_typename});
		if ( tmp )
			$function(*tmp, msg, MSG_",  uc($struct->{short_typename}), "_", uc($field->{name}), ");
	}");

    } else {
	$self->output(" " x 8, 
		      "$function(idmef_$struct->{short_typename}_get_$field->{name}($struct->{short_typename}), ", 
		      "msg, MSG_",  uc($struct->{short_typename}), "_", uc($field->{name}),
		      ");\n");
    }
}

sub	struct_field_struct
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;

    $self->output(" " x 8, 
		  "idmef_$field->{short_typename}_write(idmef_$struct->{short_typename}_get_$field->{name}($struct->{short_typename}), msg);",
		  "\n");
}

sub	struct_field_list
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;

    $self->output("\n");
    $self->output(" " x 8, "{\n");
    $self->output(" " x 16, "$field->{typename} *$field->{short_name} = NULL;", "\n\n");
    $self->output(" " x 16, "while ( ($field->{short_name} = idmef_$struct->{short_typename}_get_next_$field->{short_name}($struct->{short_typename}, $field->{short_name})) ) {", "\n");

    if ( $field->{metatype} & &METATYPE_PRIMITIVE ) {
	$self->output(" " x 24,
		      "$field->{short_typename}_write($field->{short_name}, msg, ",
		      "MSG_", uc($struct->{short_typename}), "_", uc($field->{short_name}),
		      ");\n");

    } else {
	$self->output(" " x 24, "idmef_$field->{short_typename}_write($field->{short_name}, msg);", "\n");
    }

    $self->output(" " x 16, "}\n");
    $self->output(" " x 8, "}\n");
    $self->output("\n");
}

sub	struct_field_union
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;

    $self->output("\n");
    $self->output(" " x 8, "switch ( idmef_$struct->{short_typename}_get_$field->{var}($struct->{short_typename}) ) {", "\n\n");

    foreach my $member ( @{$field->{member_list}} ) {
	$self->output(" " x 16, "case $member->{value}:", "\n");
	$self->output(" " x 24, "idmef_$member->{short_typename}_write(idmef_$struct->{short_typename}_get_$member->{name}($struct->{short_typename}), msg);", "\n");
	$self->output(" " x 24, "break;", "\n\n");
    }

    $self->output(" " x 16, "default:", "\n",
		  " " x 24, "/* nop */;", "\n\n");
    $self->output(" " x 8, "}\n");
}

sub	pre_declared
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;

    $self->output("void idmef_$struct->{short_typename}_write($struct->{typename} *, prelude_msgbuf_t *);", "\n\n");
}

sub	struct
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;

    $self->output("void idmef_$struct->{short_typename}_write($struct->{typename} *$struct->{short_typename}, prelude_msgbuf_t *msg)\n\{\n");

    $self->output(" " x 8, "if ( ! $struct->{short_typename} )", "\n",
		  " " x 16, "return;",
		  "\n\n");

    if ( ! $struct->{toplevel} ) {
	$self->output(" " x 8, "prelude_msgbuf_set(msg, ", "MSG_" . uc($struct->{short_typename}) . "_TAG", ", 0, NULL);", "\n\n");
    }

    foreach my $field ( @{ $struct->{field_list} } ) {

	if ( $field->{metatype} & &METATYPE_NORMAL ) {

	    if ( $field->{metatype} & &METATYPE_PRIMITIVE ) {
		$self->struct_field_normal($tree, $struct, $field);

	    } elsif ( $field->{metatype} & &METATYPE_STRUCT ) {
		$self->struct_field_struct($tree, $struct, $field);

	    } elsif ( $field->{metatype} & &METATYPE_ENUM ) {
		$self->struct_field_normal($tree, $struct, $field, "uint32");
	    }

	} elsif ( $field->{metatype} & &METATYPE_LIST ) {
	    $self->struct_field_list($tree, $struct, $field);

	} elsif ( $field->{metatype} & &METATYPE_UNION ) {
	    $self->struct_field_union($tree, $struct, $field);
	}
    }

    $self->output("\n", " " x 8, "prelude_msgbuf_set(msg, MSG_END_OF_TAG, 0, NULL);", "\n");
  

    $self->output("\}\n\n\n");
}

1;
