# Copyright (C) 2003 Nicolas Delon <delon.nicolas@wanadoo.fr>
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

package IDMEFTree;

BEGIN
{
    use Exporter;

    @ISA = qw/Exporter/;
    @EXPORT = qw(METATYPE_PRIMITIVE METATYPE_STRUCT METATYPE_ENUM
		 METATYPE_NORMAL METATYPE_LIST METATYPE_UNION
		 OBJ_STRUCT OBJ_ENUM OBJ_PRE_DECLARED);
}

use	strict;

sub	METATYPE_PRIMITIVE		{ 0x01 }
sub	METATYPE_STRUCT			{ 0x02 }
sub	METATYPE_ENUM			{ 0x04 }
sub	METATYPE_NORMAL			{ 0x08 }
sub	METATYPE_LIST			{ 0x10 }
sub	METATYPE_UNION			{ 0x20 }

sub	OBJ_STRUCT			{ 0 }
sub	OBJ_ENUM			{ 1 }
sub	OBJ_PRE_DECLARED		{ 2 }

sub	get_short_typename
{
    my	$short_typename = shift;

    $short_typename =~ s/^idmef_//;
    $short_typename =~ s/_t$//;

    return $short_typename;
}

sub	debug
{
    (shift)->{debug} and print @_;
}

sub	new
{
    my	$class = shift;
    my	%opt = @_;
    my	$self = { };

    $self->{debug} = $opt{-debug} || 0;
    $self->{filename} = $opt{-filename};

    $self->{lineno} = 0;
    $self->{primitives} = { };
    $self->{objs} = { };
    $self->{obj_list} = [ ];
    $self->{structs} = { };
    $self->{struct_list} = [ ];
    $self->{enums} = { };
    $self->{enum_list} = [ ];
    $self->{pre_declareds} = { };
    $self->{pre_declared_list} = [ ];

    bless($self, $class);

    return $self;
}

sub	load
{
    my	$self = shift;
    my	$file;
    my	$line;

    unless ( open(FILE, "cpp -D_GENERATE -E $self->{filename} |") ) {
	$self->debug("Cannot process file $self->{filename}: $!\n");
	return 0;
    }

    $self->{file} = \*FILE;

    $self->parse;

    close FILE;
    undef $self->{file};

    return 1;
}

sub	get_line
{
    my	$self = shift;
    my	$file = $self->{file};
    my	$line;
    my	$line2;

    defined($line = <$file>) or return undef;
    chomp $line;
    $self->{lineno}++;
    return $line;
}

sub	parse
{
    my	$self = shift;
    my	$line;
    my	$struct;

    while ( defined($line = $self->get_line) ) {
	
	if ( $line =~ /typedef\s+struct\s+\{/ ) {
	    $self->debug("parse struct\n");
	    $self->parse_struct($line);

	} elsif ( $line =~ /^\s*ENUM\(\w*\)/ ) {
	    $self->debug("parse enum\n");
	    $self->parse_enum($line);

	} elsif ( $line =~ /^\s*PRE_DECLARE/ ) {
	    $self->debug("parse register\n");
	    $self->parse_pre_declared($line);

	} elsif ( $line =~ /^\s*PRIMITIVE_TYPE\(/) {
	    $self->debug("parse primitive type\n");
	    $self->parse_primitive($line);
	}

	elsif ( $line =~ /^\s*PRIMITIVE_TYPE_STRUCT\(/ ) {
	    $self->debug("parse primitive type struct\n");
	    $self->parse_primitive_struct($line);
	}
    }

    $struct = @{ $self->{struct_list} }[-1];
    $struct->{toplevel} = 1;
}

sub	parse_struct
{
    my	$self = shift;
    my	$line = shift;
    my	$struct = { obj_type => &OBJ_STRUCT, toplevel => 0, is_listed => 0, desc => [ $line ] };
    my	@field_list;
    my	$ptr;
    my	$typename;
    my	$name;
    my	$id;
    my	$var;

    while ( defined($line = $self->get_line) ) {
	push(@{ $struct->{desc} }, $line);

	if ( $line =~ /^\s*HIDE\(/ ) {
	    next;

	} elsif ( $line =~ /^\s*IS_LISTED\s*\;\s*$/ ) {
	    $struct->{is_listed} = 1;
	    $self->debug("struct is listed\n");

	} elsif ( ($typename, $ptr, $name) = $line =~ /^\s*(\w+)\s+(\**)(\w+)\;/ ) {
	    my $metatype;

	    if ( defined $self->{primitives}->{$typename} ) {
		$metatype = $self->{primitives}->{$typename};

	    } elsif ( defined $self->{structs}->{$typename} ) {
		$metatype = &METATYPE_STRUCT;

	    } elsif ( defined $self->{enums}->{$typename} ) {
		$metatype = &METATYPE_ENUM;

	    } elsif ( defined $self->{pre_declareds}->{$typename} ) {
		$metatype = $self->{pre_declareds}->{$typename}->{metatype};

	    }

	    push(@field_list, { metatype => &METATYPE_NORMAL | $metatype, 
				typename => $typename, 
				short_typename => get_short_typename($typename),
				name => $name,
				short_name => $name,
				ptr => ($ptr ? 1 : 0) });
	    $self->debug("parse struct field metatype:normal name:$name typename:$typename ptr:", ($ptr ? 1 : 0) ? "yes" : "no", "\n");
	    
	} elsif ( ($name, $typename) = $line =~ /\s*LISTED_OBJECT\(\s*(\w+)\s*,\s*(\w+)\s*\)/ ) {
	    my $short_name;

	    $short_name = $name;
	    $short_name =~ s/_list$//;
	    
	    push(@field_list, { metatype => &METATYPE_LIST | ($self->{primitives}->{$typename} ? &METATYPE_PRIMITIVE : 0),
				typename => $typename,
				short_typename => get_short_typename($typename),
				name => $name, 
				short_name => $short_name } );
	    $self->debug("parse struct field metatype:list name:$name typename:$typename\n");

	} elsif ( ($typename, $var) = $line =~ /^\s*UNION\(\s*(\w+)\s*,\s*(\w+)\s*\)\s*\{/ ) {
	    my $field = { metatype => &METATYPE_UNION, 
			  typename => $typename,
			  short_typename => get_short_typename($typename),
			  var => $var,
			  member_list => [ ] };
	    my $member;
	    my($value, $type, $name);

	    $self->debug("parse union...\n");
	    
	    while ( defined($line = $self->get_line) ) {
		push(@{ $struct->{desc} }, $line);
		
		if ( ($value, $typename, $ptr, $name) = $line =~ /^\s*UNION_MEMBER\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(\**)(\w+)\s*\);/ ) {
		    $member = { value => $value, 
				typename => $typename,
				short_typename => get_short_typename($typename),
				name => $name,
				ptr => ($ptr ? 1 : 0) };
		    push(@{ $field->{member_list} }, $member);
		    $self->debug("parse union member name:$name typename:$typename value:$value ptr:", ($ptr ? 1 : 0), "\n");

		} elsif ( ($name) = $line =~ /^\s*\}\s*(\w+)\;/ ) {
		    $field->{name} = $name;
		    push(@field_list, $field);
		    $self->debug("parsing of union $name finished\n");
		    last;
		}
	    }
	    
	} elsif ( ($typename, $id) = $line =~ /^\}\s*TYPE_ID\(\s*(\w+)\s*,\s*(\d+)\s*\)/ ) {
	    $struct->{typename} = $typename;
	    $struct->{short_typename} = get_short_typename($typename);
	    $struct->{id} = $id;
	    $struct->{field_list} = \@field_list;
	    $self->{structs}->{$typename} = $struct;
	    push(@{ $self->{struct_list} }, $struct);
	    $self->{objs}->{$typename} = $struct;
	    push(@{ $self->{obj_list} }, $struct);
	    $self->debug("parsing of struct $typename (id:$id) finished\n");
	    last;
	}
    }
}

sub	parse_enum
{
    my	$self = shift;
    my	$line = shift;
    my	$enum = { obj_type => &OBJ_ENUM, desc => [ $line ] };
    my	@field_list;
    my	$typename;
    my	$value;
    my	$name;
    my	$id;

    ($enum->{prefix}) = $line =~ /^\s*ENUM\((\w*)\)/;

    while ( defined($line = $self->get_line) ) {
	push(@{ $enum->{desc} }, $line);

	if ( ($name, $value) = $line =~ /^\s*(\w+)\s*\=\s*(\d+)/ ) {
	    push(@field_list, { name => $name, value => $value });
	    $self->debug("parse enum field name:$name value:$value\n");
	    
	} elsif ( ($typename, $id) = $line =~ /^\}\s*TYPE_ID\(\s*(\w+)\s*,\s*(\d+)\s*\)/ ) {
	    $enum->{typename} = $typename;
	    $enum->{short_typename} = get_short_typename($typename);
	    $enum->{id} = $id;
	    $enum->{field_list} = \@field_list;
	    $self->{enums}->{$typename} = $enum;
	    push(@{ $self->{enum_list} }, $enum);
	    $self->{objs}->{$typename} = $enum;
	    push(@{ $self->{obj_list} }, $enum);
	    $self->debug("parsing of enum $typename (id:$id)\n");
	    last;
	}
    }
}

sub	parse_pre_declared
{
    my	$self = shift;
    my	$line = shift;
    my	$pre_declared = { obj_type => &OBJ_PRE_DECLARED };
    my	$typename;
    my	$class;

    ($typename, $class) = $line =~ /^\s*PRE_DECLARE\(\s*(\w+)\s*,\s*(\w+)\s*\)/;

    $pre_declared->{typename} = $typename;
    $pre_declared->{short_typename} = get_short_typename($typename);

    $pre_declared->{metatype} = &METATYPE_STRUCT if ( $class eq "struct" );
    $pre_declared->{metatype} = &METATYPE_ENUM if ( $class eq "enum" );

    $self->{pre_declareds}->{$typename} = $pre_declared;
    push(@{ $self->{pre_declared_list} }, $pre_declared);

    push(@{ $self->{obj_list} }, $pre_declared);

    $self->debug("parse pre_declared type:$typename class:$class\n");
}

sub	parse_primitive
{
    my	$self = shift;
    my	$line = shift;
    my	$type;

    ($type) = $line =~ /^\s*PRIMITIVE_TYPE\(\s*(\w+)\s*\)/;
    $self->{primitives}->{$type} = &METATYPE_PRIMITIVE;
    $self->debug("parse primitive type:$type\n");
}

sub	parse_primitive_struct
{
    my	$self = shift;
    my	$line = shift;
    my	$type;

    ($type) = $line =~ /^\s*PRIMITIVE_TYPE_STRUCT\(\s*(\w+)\s*\)/;
    $self->{primitives}->{$type} = &METATYPE_PRIMITIVE | &METATYPE_STRUCT;
    $self->debug("parse primitive struct type:$type\n");
}

sub	process
{
    my	$self = shift;
    my	$generator = shift;

    $generator->header($self) if ( $generator->can("header") );

    foreach my $obj ( @{ $self->{obj_list} } ) {
	
	$generator->obj($self, $obj) if ( $generator->can("obj") );

	if ( $obj->{obj_type} == &OBJ_STRUCT ) {
	    $generator->struct($self, $obj) if ( $generator->can("struct") );
	}

	if ( $obj->{obj_type} == &OBJ_ENUM ) {
	    $generator->enum($self, $obj) if ( $generator->can("enum") );
	}

	if ( $obj->{obj_type} == &OBJ_PRE_DECLARED ) {
	    $generator->pre_declared($self, $obj) if ( $generator->can("pre_declared") );
	}
    }

    $generator->footer($self) if ( $generator->can("footer") );
}

1;