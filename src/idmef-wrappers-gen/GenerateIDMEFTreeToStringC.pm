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

package GenerateIDMEFTreeToStringC;

use Generate;
@ISA = qw/Generate/;

use strict;
use IDMEFTree; 

sub	header
{
    my	$self = shift;

    $self->output("
/* Auto-generated by the GenerateIDMEFTreeToStringC package */

#include <stdio.h>
#include <time.h>

#include \"libmissing.h\"
#include \"idmef.h\"
#include \"idmef-util.h\"
#include \"idmef-tree-wrap.h\"
#include \"idmef-tree-to-string.h\"


static int indent = 0;



static	int to_string_indent(int len, char *buffer, size_t size)
\{
	int offset = 0;

	while ( len-- > 0 )
		MY_SNPRINTF(buffer, size, offset, \" \");

	return offset;
\}



#define to_string_(name, type, format)					\\
static int to_string_ ## name( type var, char *buffer, size_t size)	\\
\{									\\
	int offset = 0;							\\
									\\
	MY_SNPRINTF(buffer, size, offset, format, var);			\\
									\\
	return offset;							\\
\}


to_string_(uint8, uint8_t, \"\%hhu\")
/*to_string_(int16, int16_t, \"\%hd\")*/
to_string_(uint16, uint16_t, \"\%hu\")
to_string_(int32, int32_t, \"\%d\")
to_string_(uint32, uint32_t, \"\%u\")
/*to_string_(int64, int64_t, \"%\" PRId64)*/
to_string_(uint64, uint64_t, \"%\" PRIu64)
to_string_(float, float, \"\%f\")



static	int to_string_string(prelude_string_t *string, char *buffer, size_t size)
\{
	const char *s;
	int offset = 0;

	s = prelude_string_get_string(string);

	MY_SNPRINTF(buffer, size, offset, \"\%s\", s ? s : \"<empty>\");

	return offset;
\}



static	int to_string_time(idmef_time_t *t, char *buffer, size_t size)
\{
	char _buf[32];
	time_t _time;
	struct tm _tm;
	int offset = 0;

	_time = idmef_time_get_sec(t);

	if ( ! localtime_r(&_time, &_tm) )
		return -1;

	if ( strftime(_buf, sizeof (_buf), \"%H:%M:%S %d/%m/%Y\", &_tm) == 0 )
		return -1;

	MY_SNPRINTF(buffer, size, offset, \"%s (\%u.\%u)\", _buf, idmef_time_get_sec(t), idmef_time_get_usec(t));

	return offset;
\}



/* 
 * FIXME: this function assume that data is a string
 */

static	int to_string_data(idmef_data_t *data, char *buffer, size_t size)
\{
	return idmef_data_to_string(data, buffer, size);
\}


");
}

sub	struct_field_normal
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;
    my $refer = "";

    $refer = "*" if ( $field->{metatype} & &METATYPE_OPTIONAL_INT );

    if ( $field->{metatype} & &METATYPE_ENUM ) {
	$self->output("
	\{
		int ${refer}i = idmef_$struct->{short_typename}_get_$field->{name}(ptr);
");

	if ( $field->{metatype} & &METATYPE_OPTIONAL_INT ) {
	    $self->output("
		if ( i )
");
	}

	$self->output("
		\{
			MY_CONCAT(to_string_indent, indent, buffer, size, offset);
			MY_SNPRINTF(buffer, size, offset, \"$field->{name}: \");
			MY_SNPRINTF(buffer, size, offset, \"\%s (\%d)\", idmef_$field->{short_typename}_to_string(${refer}i), ${refer}i);
			MY_SNPRINTF(buffer, size, offset, \"\\n\");
		\}
        \}
");

    } elsif ( $field->{metatype} & &METATYPE_PRIMITIVE ) {
	if ( $field->{metatype} & (&METATYPE_STRUCT|&METATYPE_OPTIONAL_INT) ) {
	    $self->output("
	\{
		$field->{typename} *field;

		field = idmef_$struct->{short_typename}_get_$field->{name}(ptr);

		if ( field ) \{
			MY_CONCAT(to_string_indent, indent, buffer, size, offset);
			MY_SNPRINTF(buffer, size, offset, \"$field->{name}: \");
			MY_CONCAT(to_string_$field->{value_type}, ${refer}field, buffer, size, offset);
			MY_SNPRINTF(buffer, size, offset, \"\\n\");
		\}
	\}
"); 

	} else {
	    $self->output("
	MY_CONCAT(to_string_indent, indent, buffer, size, offset);
	MY_SNPRINTF(buffer, size, offset, \"$field->{name}: \");
	MY_CONCAT(to_string_$field->{value_type},
		  idmef_$struct->{short_typename}_get_$field->{name}(ptr),
		  buffer, size, offset);
	MY_SNPRINTF(buffer, size, offset, \"\\n\");
");
	}

    } elsif ( $field->{metatype} & &METATYPE_STRUCT ) {
	$self->output("
	\{
		$field->{typename} *field;

		field = idmef_$struct->{short_typename}_get_$field->{name}(ptr);

		if ( field ) \{
			MY_CONCAT(to_string_indent, indent, buffer, size, offset);
			MY_SNPRINTF(buffer, size, offset, \"$field->{name}:\\n\");
			MY_CONCAT(idmef_$field->{short_typename}_to_string, field, buffer, size, offset);
		\}
	\}	
");
    }
}

sub	struct_field_union
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;

    $self->output("
	switch ( idmef_$struct->{short_typename}_get_$field->{var}(ptr) ) \{");

    foreach my $member ( @{ $field->{member_list} } ) {
	$self->output("
	case $member->{value}:
		MY_CONCAT(to_string_indent, indent, buffer, size, offset);
		MY_SNPRINTF(buffer, size, offset, (\"$member->{name}:\\n\"));
		MY_CONCAT(idmef_$member->{short_typename}_to_string,
			  idmef_$struct->{short_typename}_get_$member->{name}(ptr),
			  buffer, size, offset);
		break;
 ");
    }

    $self->output("
	default:
		break;
	\}
");

}

sub	struct_field_list
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;
    my	$field = shift;

    $self->output("
	\{
		$field->{typename} *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_$struct->{short_typename}_get_next_$field->{short_name}(ptr, elem)) ) \{
			MY_CONCAT(to_string_indent, indent, buffer, size, offset);
");

    if ( $field->{metatype} & &METATYPE_PRIMITIVE ) {
	$self->output("
			MY_SNPRINTF(buffer, size, offset, \"$field->{short_name}(%d): \", cnt);
			MY_CONCAT(to_string_$field->{value_type}, elem, buffer, size, offset);
			MY_SNPRINTF(buffer, size, offset, \"\\n\");
");

    } else {
	$self->output("
			MY_SNPRINTF(buffer, size, offset, \"$field->{short_name}(%d):\\n\", cnt);
			MY_CONCAT(idmef_$field->{short_typename}_to_string, elem, buffer, size, offset);
");
    }

    $self->output("
			cnt++;
		\}
	\}
");
}

sub	struct
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;

    $self->output("
int	idmef_$struct->{short_typename}_to_string($struct->{typename} *ptr, char *buffer, size_t size)
\{
	int offset = 0;

	if ( ! ptr )
		return -1;

	indent += 8;
");

    foreach my $field ( @{ $struct->{field_list} } ) {

	if ( $field->{metatype} & &METATYPE_NORMAL ) {
	    $self->struct_field_normal($tree, $struct, $field);

	} elsif ( $field->{metatype} & &METATYPE_UNION ) {
	    $self->struct_field_union($tree, $struct, $field);

	} elsif ( $field->{metatype} & &METATYPE_LIST ) {
	    $self->struct_field_list($tree, $struct, $field);
	}
    }

    $self->output("
	indent -= 8;

	return offset;
\}
");
}

sub	footer
{
    my	$self = shift;
    my	$tree = shift;
}

1;
