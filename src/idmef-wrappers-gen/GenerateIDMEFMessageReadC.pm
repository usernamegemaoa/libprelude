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

package GenerateIDMEFMessageReadC;

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
* Copyright (C) 2001-2004 Yoann Vandoorselaere <yoann\@prelude-ids.org>
* Copyright (C) 2003 Nicolas Delon <delon.nicolas\@wanadoo.fr>
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

/* Auto-generated by the GenerateIDMEFMessageReadC package */

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include \"prelude-error.h\"
#include \"prelude-inttypes.h\"
#include \"prelude-list.h\"
#include \"prelude-extract.h\"
#include \"prelude-io.h\"
#include \"idmef-message-id.h\"
#include \"idmef.h\"
#include \"idmef-tree-wrap.h\"

#include \"idmef-message-read.h\"
#include \"idmef-util.h\"

#define prelude_extract_string_safe(out, buf, len, msg) extract_string_safe_f(__FUNCTION__, __LINE__, out, buf, len)

static inline int extract_string_safe_f(const char *f, int line, prelude_string_t **out, char *buf, size_t len)
\{
        /*
         * we use len - 1 since len is supposed to include \0 to avoid making a dup.
         */

        *out = prelude_string_new_ref_fast(buf, len - 1);
        if ( ! *out ) \
                return prelude_error_from_errno(errno);

        return 0;
\}


static inline int prelude_extract_time_safe(idmef_time_t **out, void *buf, size_t len, prelude_msg_t *msg)
\{
        /*
         * sizeof(sec) + sizeof(usec) + sizeof(gmt offset).
         */
        if ( len != 12 )
                return prelude_error_make(PRELUDE_ERROR_SOURCE_EXTRACT, PRELUDE_ERROR_INVAL_IDMEF_TIME);

        *out = idmef_time_new();
        if ( ! *out )
                return prelude_error_from_errno(errno);

        idmef_time_set_sec(*out, prelude_extract_uint32(buf));
        idmef_time_set_usec(*out, prelude_extract_uint32(buf + 4));
        idmef_time_set_gmt_offset(*out, prelude_extract_int32(buf + 8));

        return 0;
\}


static inline int prelude_extract_data_safe(idmef_data_t **out, void *buf, size_t len, prelude_msg_t *msg)
\{
	idmef_data_type_t type;
	uint8_t tag;

	if ( prelude_extract_uint32_safe(&type, buf, len) < 0 )
		return -1;

	if ( prelude_msg_get(msg, &tag, &len, &buf) < 0 )
		return -1;

	*out = NULL;

	switch ( type ) \{
	case IDMEF_DATA_TYPE_CHAR: \{
		char tmp;

		if ( prelude_extract_uint8_safe(&tmp, buf, len) < 0 )
			return -1;
		*out = idmef_data_new_char(tmp);
		break;
	\}

	case IDMEF_DATA_TYPE_BYTE: \{
		uint8_t tmp;

		if ( prelude_extract_uint8_safe(&tmp, buf, len) < 0 )
			return -1;
		*out = idmef_data_new_byte(tmp);
		break;
	\}

	case IDMEF_DATA_TYPE_UINT32: \{
		uint32_t tmp;

		if ( prelude_extract_uint32_safe(&tmp, buf, len) < 0 )
			return -1;
		*out = idmef_data_new_uint32(tmp);
		break;
	\}

	case IDMEF_DATA_TYPE_UINT64: \{
		uint64_t tmp;

		if ( prelude_extract_uint64_safe(&tmp, buf, len) < 0 )
			return -1;
		*out = idmef_data_new_uint64(tmp);
		break;
	\}

	case IDMEF_DATA_TYPE_FLOAT: \{
		float tmp;

		if ( prelude_extract_float_safe(&tmp, buf, len) < 0 )
			return -1;
		*out = idmef_data_new_float(tmp);
		break;
	\}

	case IDMEF_DATA_TYPE_CHAR_STRING: case IDMEF_DATA_TYPE_BYTE_STRING: \{
		const char *tmp;

		if ( prelude_extract_characters_safe(&tmp, buf, len) < 0 )
			return -1;
		*out = idmef_data_new_ptr_ref_fast(type, tmp, len);
		break;		
	\}

	case IDMEF_DATA_TYPE_UNKNOWN:
		/* nop */;
	\}

	return *out ? 0 : prelude_error_from_errno(errno);
\}


");
}

sub	struct_field_normal
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;
    my	$ptr = ($field->{metatype} & (&METATYPE_STRUCT | &METATYPE_LIST)) ? "*" : "";
    my	$type = shift || $field->{value_type};
    my	$var_type = shift || "$field->{typename}";
    my	$extra_msg = "";

    $extra_msg = ", msg" if ( $field->{metatype} & (&METATYPE_STRUCT | &METATYPE_LIST) );

    $self->output("
			case MSG_",  uc($struct->{short_typename}), "_", uc($field->{short_name}), ": \{
                                ${var_type} ${ptr}tmp;

                                ret = prelude_extract_${type}_safe(&tmp, buf, len${extra_msg});
                                if ( ret < 0 )
                                        return ret;

				idmef_$struct->{short_typename}_set_$field->{short_name}($struct->{short_typename}, tmp);
				break;
			\}
");
}

sub	struct_field_struct
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;
    my	$name = shift || $field->{name};

    $self->output("
			case MSG_",  uc($field->{short_typename}), "_TAG", ": \{
				$field->{typename} *tmp;

				tmp = idmef_$struct->{short_typename}_new_${name}($struct->{short_typename});
				if ( ! tmp)
					return prelude_error_from_errno(errno);

                                ret = idmef_$field->{short_typename}_read(tmp, msg);
				if ( ret < 0 )
					return ret;

				break;
			\}
");
}

sub	struct_field_union
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;

    foreach my $member ( @{$field->{member_list}} ) {
	$self->struct_field_struct($tree, $struct, $member);
    }
}

sub	struct
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;

    $self->output("
int idmef_$struct->{short_typename}_read($struct->{typename} *$struct->{short_typename}, prelude_msg_t *msg)
\{
        int ret;
	void *buf;
	uint8_t tag;
	uint32_t len;

	while ( 1 ) \{
                ret = prelude_msg_get(msg, &tag, &len, &buf);
                if ( ret < 0 )
		        return ret;

		switch ( tag ) \{
");

    foreach my $field ( @{$struct->{field_list}} ) {

	if ( $field->{metatype} & &METATYPE_NORMAL ) {

	    if ( $field->{metatype} & &METATYPE_PRIMITIVE ) {
		$self->struct_field_normal($tree, $struct, $field);

	    } elsif ( $field->{metatype} & &METATYPE_ENUM ) {
		$self->struct_field_normal($tree, $struct, $field, "int32", "int32_t");

	    } else {
		$self->struct_field_struct($tree, $struct, $field);
	    }

	} elsif ( $field->{metatype} & &METATYPE_LIST ) {
	    if ( $field->{metatype} & &METATYPE_PRIMITIVE ) {
		$self->struct_field_normal($tree, $struct, $field);

	    } else {
		$self->struct_field_struct($tree, $struct, $field, $field->{short_name});
	    }

	} elsif ( $field->{metatype} & &METATYPE_UNION ) {
	    $self->struct_field_union($tree, $struct, $field);
	}
    }

    $self->output("
			case MSG_END_OF_TAG:
				return 0;

			default:
				return prelude_error(PRELUDE_ERROR_IDMEF_UNKNOWN_TAG);
		\}

	\}

	return 0;
\}
");
}

1;
