# Copyright (C) 2003, 2004, 2005 PreludeIDS Technologies. All Rights Reserved.
# Author: Nicolas Delon <nicolas.delon@prelude-ids.com>
#
# This file is part of the Prelude library.
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

package GenerateIDMEFTreeWrapC;

use Generate;
@ISA = qw/Generate/;

use strict;
use IDMEFTree;

sub     header
{
    my  $self = shift;
    my  $file = $self->{file};

    $self->output("
/*****
*
* Copyright (C) 2001,2002,2003,2004,2005 PreludeIDS Technologies. All Rights Reserved.
* Author: Yoann Vandoorselaere <yoann.v\@prelude-ids.com>
* Author: Nicolas Delon <nicolas.delon\@prelude-ids.com>
*
* This file is part of the Prelude library.
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

/* Auto-generated by the GenerateIDMEFTreeWrapC package */

#include \"config.h\"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#include \"prelude-inttypes.h\"
#include \"prelude-list.h\"
#include \"prelude-msg.h\"
#include \"prelude-string.h\"

#define PRELUDE_ERROR_SOURCE_DEFAULT PRELUDE_ERROR_SOURCE_IDMEF_TREE_WRAP
#include \"prelude-error.h\"

#include \"idmef-time.h\"
#include \"idmef-data.h\"
#include \"idmef-class.h\"
#include \"idmef-value.h\"

#include \"idmef-tree-wrap.h\"
#include \"libmissing.h\"

#define LISTED_OBJECT(name, type) prelude_list_t name

#define IS_LISTED prelude_list_t list

#define UNION(type, var) type var; union

#define UNION_MEMBER(value, type, name) type name

#define ENUM(...) typedef enum

#define PRE_DECLARE(type, class)

#define TYPE_ID(type, id) type

#define PRIMITIVE_TYPE(type)
#define PRIMITIVE_TYPE_STRUCT(type)

#define HIDE(type, name) type name

#define REFCOUNT int refcount

#define DYNAMIC_IDENT(x) uint64_t x

#define OPTIONAL_INT(type, name) type name; unsigned int name ## _is_set:1

#define IDENT(name) uint64_t name



static int get_value_from_string(idmef_value_t **value, prelude_string_t *str)
{
        int ret;
        
        if ( ! str ) {
                *value = NULL;
                return 0;
        }

        ret = idmef_value_new_string(value, str);
        if ( ret < 0 )
                return ret;

        idmef_value_dont_have_own_data(*value);

        return 0;
}



static int get_value_from_data(idmef_value_t **value, idmef_data_t *data)
{
        int ret;
        
        if ( ! data ) {
                *value = NULL;
                return 0;
        }

        ret = idmef_value_new_data(value, data);
        if ( ret < 0 )
                return ret;

        idmef_value_dont_have_own_data(*value);

        return 0;
}


static int get_value_from_time(idmef_value_t **value, idmef_time_t *time)
{
        int ret;
        
        if ( ! time ) {
                *value = NULL;
                return 0;
        }

        ret = idmef_value_new_time(value, time);
        if ( ret < 0 )
                return ret;

        idmef_value_dont_have_own_data(*value);

        return 0;
}


static void list_insert(prelude_list_t *head, prelude_list_t *item, int pos)
{
        int i = 0;
        prelude_list_t *tmp;
        
        if ( pos == IDMEF_LIST_APPEND || pos == -1 /* FIXME: deprecated */ )
                prelude_list_add_tail(head, item);

        else if ( pos == IDMEF_LIST_PREPEND || pos == 0 /* FIXME: deprecated */ )
                prelude_list_add(head, item);

        else if ( pos > 0 ) {
                prelude_list_for_each(head, tmp) {
                        if ( i == pos )
                                break;
                        i++;
                }

                prelude_list_add_tail(tmp, item);
        }

        else if ( pos < 0 ) {
                pos = -pos;
                pos--;

                prelude_list_for_each_reversed(head, tmp) {
                        if ( i == pos )
                                break;
                        i++;
                }

                prelude_list_add(tmp, item);
        }
}

");
}

sub     struct_desc
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;
    my $line;

    $self->output("\n\nstruct idmef_$struct->{short_typename} \{");

    foreach ( @{ $struct->{desc} } ) {
        $line = $_;
        $line =~ s/.*TYPE_ID.*//;
        $line =~ s/struct {//;
    $self->output(" $line\n");

    }
    #$self->output(" $_\n") foreach ( @{ $struct->{desc} } ~ s/.*TYPE_ID.*// );
    $self->output("\};\n\n");
}

sub     struct_constructor
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;

    $self->output("
/**
 * idmef_$struct->{short_typename}_new:
 * \@ret: Pointer where to store the created #$struct->{typename} object.
 *
 * Create a new #$struct->{typename} object.
 *
 * Returns: 0 on success, a negative value if an error occured.
 */
int idmef_$struct->{short_typename}_new($struct->{typename} **ret)
\{
        *ret = calloc(1, sizeof(**ret));
        if ( ! *ret )
                return prelude_error_from_errno(errno);
");

    if ( $struct->{is_listed} ) {
        $self->output("
        prelude_list_init(&(*ret)->list);
");
    }

    if ( $struct->{refcount} ) {
        $self->output("
        (*ret)->refcount = 1;
");
    }

    foreach my $field ( map { $_->{metatype} & &METATYPE_LIST ? $_ : () } @{ $struct->{field_list} } ) {
        $self->output("
        prelude_list_init(&(*ret)->$field->{name});
    
");

    }
    $self->output("
        return 0;

\}
");
}

sub     struct_get_child
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;
    my  $n = 0;

    $self->output("
int idmef_$struct->{short_typename}_get_child(void *p, idmef_class_child_id_t child, void **childptr)
\{
        $struct->{typename} *ptr = p;

        *childptr = NULL;
        
        switch ( child ) \{
");

    foreach my $field ( @{ $struct->{field_list} } ) {

        if ( $field->{metatype} & &METATYPE_LIST ) {

            $self->output("
                case $n:
                        *childptr = &ptr->$field->{name};
                        return 0;
");
        } elsif ( $field->{metatype} & &METATYPE_UNION ) {

            foreach my $member ( @{ $field->{member_list} } ) {
                $self->output("
                case $n:
                        *childptr = ( ptr->$field->{var} == $member->{value} ) ? ptr->$field->{name}.$member->{name} : NULL;
                        return 0;          
");
                $n++;
            }
        } 

        elsif ( $field->{metatype} & &METATYPE_ENUM ) {
            $self->output("        
                case $n:");

            if ( $field->{metatype} & &METATYPE_OPTIONAL_INT ) {
                $self->output("
                       if ( ! ptr->$field->{name}_is_set )
                               return 0;");
            }

            $self->output("
                       return idmef_value_new_enum_from_numeric((idmef_value_t **) childptr, 
                                                                IDMEF_CLASS_ID_" . uc($field->{short_typename}) . ", ptr->$field->{short_name});
");
        } elsif ( $field->{metatype} & (&METATYPE_OPTIONAL_INT) ) {
           if ( $field->{metatype} & &METATYPE_OPTIONAL_INT ) {
            $self->output("
                case $n:
                       return (ptr->$field->{name}_is_set) ? idmef_value_new_$field->{value_type}((idmef_value_t **) childptr, ptr->$field->{name}) : 0;
");
        }

        } elsif ( $field->{metatype} & (&METATYPE_PRIMITIVE) ) {
            $self->output("        
                case $n:");

            if ( $field->{metatype} & &METATYPE_OPTIONAL_INT ) {
                $self->output("
                       if ( ! ptr->$field->{name}_is_set )
                               return 0;");
            }

            if ( $field->{typename} eq "prelude_string_t" ) {
                my $refer = $field->{ptr} ? "" : "&";
                $self->output("
                       return get_value_from_string((idmef_value_t **) childptr, $refer ptr->$field->{name});");
            } 
            elsif ( $field->{typename} eq "idmef_data_t" ) {
                my $refer = $field->{ptr} ? "" : "&";
                $self->output("
                       return get_value_from_data((idmef_value_t **) childptr, $refer ptr->$field->{name});");
            } elsif ( $field->{typename} eq "idmef_time_t" ) {
                my $refer = $field->{ptr} ? "" : "&";
                $self->output("
                       return get_value_from_time((idmef_value_t **) childptr, $refer ptr->$field->{name});");
            } 

            else {
                $self->output("
                       return idmef_value_new_$field->{value_type}((idmef_value_t **) childptr, ptr->$field->{name});");
            }

        } else {
            my $refer = $field->{ptr} ? "" : "&";

            $self->output("
                case ${n}:
                        *childptr = ${refer}ptr->$field->{name};
                        return 0;
");
        }

        $n++;
    }

    $self->output("
                default:
                        return prelude_error(PRELUDE_ERROR_IDMEF_TYPE_UNKNOWN_CHILD);
        \}
\}
");
}

sub     struct_new_child
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;
    my  $n = 0;

    $self->output("
int idmef_$struct->{short_typename}_new_child(void *p, idmef_class_child_id_t child, int n, void **ret)
\{
        $struct->{typename} *ptr = p;
    
        switch ( child ) \{
");

    foreach my $field ( @{ $struct->{field_list} } ) {

        if ( $field->{metatype} & &METATYPE_LIST ) {
            $self->output("
                case $n: \{
                        int i = 0;
                        prelude_list_t *tmp;

                        if ( n == IDMEF_LIST_APPEND || n == IDMEF_LIST_PREPEND || n == -1 /* FIXME: deprecated */ )
                               return idmef_$struct->{short_typename}_new_$field->{short_name}(ptr, ($field->{typename} **) ret, n);

                        if ( n >= 0 ) {
                               prelude_list_for_each(&ptr->$field->{name}, tmp) {                
                                       if ( i++ == n ) {
                                               *ret = prelude_list_entry(tmp, $field->{typename}, list);
                                               return 0;
                                       }
                               }

                               if ( i != n )
                                       return prelude_error(PRELUDE_ERROR_IDMEF_TREE_INDEX_UNDEFINED);
                        } else {
                               int pos = (-n) - 1; /* With negative value, -1 is the base, translate to 0 */
                
                               prelude_list_for_each_reversed(&ptr->$field->{name}, tmp) {
                                       if ( i++ == pos ) {
                                               *ret = prelude_list_entry(tmp, $field->{typename}, list);
                                               return 0;
                                       }
                               }

                               if ( i != pos )
                                       return prelude_error(PRELUDE_ERROR_IDMEF_TREE_INDEX_UNDEFINED);
                        }

                        return idmef_$struct->{short_typename}_new_$field->{short_name}(ptr, ($field->{typename} **) ret, n);
                \}
");
        } elsif ( $field->{metatype} & &METATYPE_UNION ) {

            foreach my $member ( @{ $field->{member_list} } ) {
                $self->output("
                case $n:
                        return idmef_$struct->{short_typename}_new_$member->{name}(ptr, ($member->{typename} **) ret);
");
                $n++;
            }
            
        } elsif ( $field->{metatype} & &METATYPE_NORMAL ) {
            $self->output("
                case $n:
                        return idmef_$struct->{short_typename}_new_$field->{name}(ptr, ($field->{typename} **) ret);
");
        }
        
        $n++;
    }

    $self->output("
                default:
                        return prelude_error(PRELUDE_ERROR_IDMEF_TYPE_UNKNOWN_CHILD);
        \}
\}
");
}

sub     struct_destroy_internal
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;

    $self->output("
static void idmef_$struct->{short_typename}_destroy_internal($struct->{typename} *ptr)
\{
");
    
    $self->output("
       if ( ! prelude_list_is_empty(&ptr->list) )
               prelude_list_del_init(&ptr->list);
    ") if ( $struct->{is_listed} );

    foreach my $field ( @{ $struct->{field_list} } ) {
        my $destroy_func = "$field->{short_typename}_destroy";
        my $destroy_internal_func = "${destroy_func}_internal";

        if ( ! ($field->{metatype} & &METATYPE_PRIMITIVE) ) {
            $destroy_func = "idmef_${destroy_func}";
            $destroy_internal_func = "idmef_$destroy_internal_func";
        }
        
        if ( $field->{metatype} & &METATYPE_LIST ) {

            $self->output("
        \{
                prelude_list_t *n, *tmp;
                $field->{typename} *entry;

                prelude_list_for_each_safe(&ptr->$field->{name}, tmp, n) \{
                        entry = prelude_list_entry(tmp, $field->{typename}, list);
                        prelude_list_del_init(&entry->list);
                        $destroy_func(entry);
                \}
        \}
");

        } elsif ( $field->{metatype} & &METATYPE_UNION ) {
            $self->output("
        switch ( ptr->$field->{var} ) {
");

            foreach my $member ( @{ $field->{member_list} } ) {

                $self->output("
                case $member->{value}:
                        idmef_$member->{short_typename}_destroy(ptr->$field->{name}.$member->{name});
                        ptr->$field->{name}.$member->{name} = NULL;
                        break;
");
            }
            $self->output("
                default:
                        break;
        }
");
        } elsif ( $field->{metatype} & &METATYPE_STRUCT ) {
            if ( $field->{ptr} ) {
                $self->output("
        if ( ptr->$field->{name} ) \{
                ${destroy_func}(ptr->$field->{name});
                ptr->$field->{name} = NULL;
        \}
");
            } else {
                $self->output("
        ${destroy_internal_func}(&ptr->$field->{name});
");
            }
        } elsif ( $field->{metatype} & &METATYPE_PRIMITIVE ) {

            if ( $field->{ptr} ) {
                $self->output("
        if ( ptr->$field->{name} )
                free(ptr->$field->{name});
");
            }
        }
    }

    $self->output("

        /* free() should be done by the caller */
\}
");
}

sub     struct_destroy
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;

    return if ( $struct->{toplevel} );

    $self->output("
/**
 * idmef_$struct->{short_typename}_destroy:
 * \@ptr: pointer to a #$struct->{typename} object.
 * 
 * Destroy \@ptr and all of it's children.
 * The objects are only destroyed if their reference count reach zero.
 */
");

    if ( $struct->{refcount} ) {
        $self->output("
void idmef_$struct->{short_typename}_destroy($struct->{typename} *ptr)
\{
        if ( --ptr->refcount )
                return;

        idmef_$struct->{short_typename}_destroy_internal(ptr);
        free(ptr);
\}
");

    } else {
        $self->output("
void idmef_$struct->{short_typename}_destroy($struct->{typename} *ptr)
\{
        idmef_$struct->{short_typename}_destroy_internal(ptr);
        free(ptr);
\}
");
    }
}

sub     struct_ref
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;

    $struct->{refcount} or return;

    $self->output("
/**
 * idmef_$struct->{short_typename}_ref:
 * \@ptr: pointer to a #$struct->{typename} object.
 *
 * Increase \@ptr reference count, so that it can be referenced
 * multiple time.
 *
 * Returns: a pointer to \@ptr.
 */
$struct->{typename} *idmef_$struct->{short_typename}_ref($struct->{typename} *ptr)
\{
        ptr->refcount++;

        return ptr;
\}
");
}

sub     struct_field_normal
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;
    my  $field = shift;
    my  $name = shift || $field->{name};
    my  $ptr = "";
    my  $refer = "";

    if ( $field->{ptr} ) {
        if ( $field->{metatype} & &METATYPE_STRUCT ) {
            $ptr = "*";

        } else {
            $refer = "*";
        }

    } else {
        if ( $field->{metatype} & (&METATYPE_STRUCT|&METATYPE_OPTIONAL_INT) ) {
            $ptr = "*";
            $refer = "&";
        }
    }

    ##############################
    # Generate *_get_* functions #
    ##############################

    $self->output("
/**
 * ${ptr}idmef_$struct->{short_typename}_get_${name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 *
 * Get ${name} children of the #$struct->{typename} object.
 *
 * Returns: a pointer to a $field->{typename} object, or NULL if the children object is not set.
 */
$field->{typename} ${ptr}idmef_$struct->{short_typename}_get_${name}($struct->{typename} *ptr)
\{");

    if ( $field->{metatype} & &METATYPE_OPTIONAL_INT ) {
        $self->output("
        return ptr->$field->{name}_is_set ? &ptr->$field->{name} : NULL;");
        
    } elsif ( $field->{metatype} & &METATYPE_PRIMITIVE && !($field->{metatype} & &METATYPE_STRUCT) && $field->{ptr}) {
        $self->output("
        return ptr->$field->{name} ? *ptr->$field->{name} : ($field->{typename}) 0;");

    } else {
        $self->output("
        return ${refer}ptr->$field->{name};");
    }

    $self->output("
\}
");
    
    
    ##############################
    # Generate *_set_* functions #
    ##############################

    my $field_name = ($field->{"name"} eq "class") ? "class_str" : $field->{name};


    $self->output("
/**
 * idmef_$struct->{short_typename}_set_$field->{name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 * \@$field_name: pointer to a #$field->{typename} object.
 *
 * Set \@$field_name object as a children of \@ptr.
 * if \@ptr already contain an \@$field_name object, then it is destroyed,
 * and updated to point to the provided \@$field_name object.
 */
");

    if ( $field->{metatype} & &METATYPE_OPTIONAL_INT ) {
        $self->output("
void idmef_$struct->{short_typename}_set_$field->{name}($struct->{typename} *ptr, $field->{typename} $field_name)
\{
        ptr->$field->{name} = $field_name;
        ptr->$field->{name}_is_set = 1;
\}


void idmef_$struct->{short_typename}_unset_$field->{name}($struct->{typename} *ptr)
\{
        ptr->$field->{name}_is_set = 0;
\}

");

    } elsif ( $field->{metatype} & &METATYPE_STRUCT ) {
        if ( $field->{ptr} ) {
            my $destroy_func = "$field->{short_typename}_destroy";

            $destroy_func = "idmef_${destroy_func}" if ( ! ($field->{metatype} & &METATYPE_PRIMITIVE) );

            $self->output("
void idmef_$struct->{short_typename}_set_$field->{name}($struct->{typename} *ptr, $field->{typename} *$field_name)
\{
        if ( ptr->$field->{name} )
                ${destroy_func}(ptr->$field->{name});

        ptr->$field->{name} = $field_name;
\}
");
        } else {
            my $destroy_internal_func = "$field->{short_typename}_destroy_internal";

            $destroy_internal_func = "idmef_${destroy_internal_func}" if ( ! ($field->{metatype} & &METATYPE_PRIMITIVE) );

            $self->output("
void idmef_$struct->{short_typename}_set_$field->{name}($struct->{typename} *ptr, $field->{typename} *$field_name)
\{
        ${destroy_internal_func}(&ptr->$field->{name});
        if ( $field_name ) {
                memcpy(&ptr->$field->{name}, $field_name, sizeof (ptr->$field->{name}));
                free($field->{name});
        }
\}
");
        }
    } else {
        if ( $field->{ptr} ) {
            $self->output("
void idmef_$struct->{short_typename}_set_$field->{name}($struct->{typename} *ptr, $field->{typename} $field_name)
\{
        if ( ptr->$field->{name} )
                free(ptr->$field->{name});

        ptr->$field->{name} = malloc(sizeof (*ptr->$field->{name}));
        *ptr->$field->{name} = $field_name;
\}
");

        } else {
            $self->output("
void idmef_$struct->{short_typename}_set_$field->{name}($struct->{typename} *ptr, $field->{typename} $field_name)
\{
        ptr->$field->{name} = $field_name;
\}
");
        }
    }

    ##############################
    # Generate *_new_* functions #
    ##############################

    $self->output("
/**
 * idmef_$struct->{short_typename}_new_${name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 * \@ret: pointer to an address where to store the created #$field->{typename} object.
 *
 * Create a new ${name} object, children of #$struct->{typename}.
 * If \@ptr already contain a #$field->{typename} object, then it is destroyed.
 *
 * Returns: 0 on success, or a negative value if an error occured.
 */
int idmef_$struct->{short_typename}_new_${name}($struct->{typename} *ptr, $field->{typename} **ret)
\{");
    if ( $field->{metatype} & &METATYPE_OPTIONAL_INT ) {
        $self->output("
        ptr->$field->{name}_is_set = 1;
");
    } elsif ( $field->{metatype} & &METATYPE_PRIMITIVE ) {

        if ( $field->{metatype} & &METATYPE_STRUCT ) {

            if ( $field->{ptr} ) {

                $self->output("
        int retval;

        if ( ptr->$field->{name} )
                $field->{short_typename}_destroy(ptr->$field->{name});
                
        retval = $field->{short_typename}_new(&ptr->$field->{name});
        if ( retval < 0 )
               return retval;
");
            } else {

                $self->output("
        $field->{short_typename}_destroy_internal(&ptr->$field->{name});
");
            }

        } else {
            if ( $field->{ptr} ) {
                $self->output("
        if ( ptr->$field->{name} )
                free(ptr->$field->{name});

        ptr->$field->{name} = calloc(1, sizeof (*ptr->$field->{name}));
");
            }
        }

    } else {
        if ( $field->{metatype} & &METATYPE_STRUCT ) {
            if ( $field->{ptr} ) {
                $self->output("
        int retval;

        if ( ! ptr->$field->{name} )
                retval = idmef_$field->{short_typename}_new(&ptr->$field->{name});
");
            } else {
                if ( $tree->{objs}->{$field->{typename}}->{is_listed} ) {
                     $self->output("        
        prelude_list_init(&ptr->$field->{name}.list);");
                 }
            }
        }
    }

    $refer = $field->{ptr} ? "" : "&";

    $self->output("
        *ret = ${refer}ptr->$field->{name};
        return 0;
\}
");
}

sub     struct_field_union
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;
    my  $field = shift;

    $self->output("
/** 
 * idmef_$struct->{short_typename}_get_$field->{var}:
 * \@ptr: pointer to a #$struct->{typename} object.
 *
 * Access the $field->{var} children of \@ptr.
 *
 * Returns: a pointer to the #$field->{typename} children, or NULL if it is not set.
 */
$field->{typename} idmef_$struct->{short_typename}_get_$field->{var}($struct->{typename} *ptr)
\{
        return ptr->$field->{var};
\}
");

    foreach my $member ( @{ $field->{member_list} } ) {
        $self->output("
/** 
 * idmef_$struct->{short_typename}_get_$member->{name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 *
 * Access the $member->{name} children of \@ptr.
 *
 * Returns: a pointer to the #$member->{typename} children, or NULL if it is not set.
 */
$member->{typename} *idmef_$struct->{short_typename}_get_$member->{name}($struct->{typename} *ptr)
\{
        return (ptr->$field->{var} == $member->{value}) ? ptr->$field->{name}.$member->{name} : NULL;
\}
"
);

        $self->output("
/**
 * idmef_$struct->{short_typename}_set_$member->{name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 * \@$member->{name}: pointer to a #$member->{typename} object.
 *
 * Set \@$member->{name} object as a children of \@ptr.
 * if \@ptr already contain a \@$member->{name} object, then it is destroyed,
 * and updated to point to the provided \@$member->{name} object.
 */
void idmef_$struct->{short_typename}_set_$member->{name}($struct->{typename} *ptr, $member->{typename} *$member->{name})
\{
        switch ( ptr->$field->{var} ) \{
");
        foreach my $member ( @{ $field->{member_list} } ) {
            $self->output("
                case $member->{value}:
                        idmef_$member->{short_typename}_destroy(ptr->$field->{name}.$member->{name});
                        break;
");
        }

        $self->output("
                default:
                        break;
        \}

        ptr->$field->{name}.$member->{name} = $member->{name};
        ptr->$field->{var} = $member->{value};
\}
");
        
        $self->output("
/**
 * idmef_$struct->{short_typename}_new_$member->{name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 * \@ret: pointer where to store the created #$member->{typename} object.
 *
 * Create a new $member->{typename} object, children of #$struct->{typename}.
 * If \@ptr already contain a #$member->{typename} object, then it is destroyed.
 *
 * Returns: 0 on success, or a negative value if an error occured.
 */
int idmef_$struct->{short_typename}_new_$member->{name}($struct->{typename} *ptr, $member->{typename} **ret)
\{
        int retval;

        switch ( ptr->$field->{var} ) \{
");
        foreach my $other_member ( @{ $field->{member_list} } ) {

            if ( $other_member == $member ) {
                $self->output("
                case $member->{value}:
                        *ret = ptr->$field->{name}.$member->{name};
                        return 0;
"); 
            } else {
                $self->output("
                case $other_member->{value}:
                        idmef_$other_member->{short_typename}_destroy(ptr->$field->{name}.$other_member->{name});
                        break;
");
            }
        }

        $self->output("
                default:
                        break;
        \}

        retval = idmef_$member->{short_typename}_new(ret);
        if ( retval < 0 )
                return retval;

        ptr->$field->{var} = $member->{value};
        ptr->$field->{name}.$member->{name} = *ret;

        return 0;
\}
");
    }
}

sub     struct_field_list
{
    my  $self = shift;
    my  $tree = shift;
    my  $struct = shift;
    my  $field = shift;
    my  $new_field_function = "$field->{short_typename}_new(ret)";

    $new_field_function = "idmef_${new_field_function}" if ( ! ($field->{metatype} & &METATYPE_PRIMITIVE) );

    $self->output("
/**
 * idmef_$struct->{short_typename}_get_next_$field->{short_name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 * \@object: pointer to a #$field->{typename} object.
 *
 * Get the next #$field->{typename} object listed in \@ptr.
 * When iterating over the $field->{typename} object listed in \@ptr,
 * \@object should be set to the latest returned #$field->{typename} object.
 * 
 * Returns: the next #$field->{typename} in the list.
 */
$field->{typename} *idmef_$struct->{short_typename}_get_next_$field->{short_name}($struct->{typename} *ptr, $field->{typename} *object)
\{
        prelude_list_t *tmp = (object) ? &object->list : NULL;

        prelude_list_for_each_continue(&ptr->$field->{name}, tmp)
                return prelude_list_entry(tmp, $field->{typename}, list);

        return NULL;
\}


/**
 * idmef_$struct->{short_typename}_set_$field->{short_name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 * \@object: pointer to a #$field->{typename} object.
 * \@pos: Position in the list.
 *
 * Add \@object to position \@pos of \@ptr list of #$field->{typename} object.
 *
 * If \@pos is #IDMEF_LIST_APPEND, \@object will be inserted at the tail of the list.
 * If \@pos is #IDMEF_LIST_PREPEND, \@object will be inserted at the head of the list.
 */
void idmef_$struct->{short_typename}_set_$field->{short_name}($struct->{typename} *ptr, $field->{typename} *object, int pos)
\{
        if ( ! prelude_list_is_empty(&object->list) )
                prelude_list_del_init(&object->list);

        list_insert(&ptr->$field->{name}, &object->list, pos);
\}


/**
 * idmef_$struct->{short_typename}_new_$field->{short_name}:
 * \@ptr: pointer to a #$struct->{typename} object.
 * \@ret: pointer to an address where to store the created #$field->{typename} object.
 * \@pos: position in the list.
 *
 * Create a new #$field->{typename} children of \@ptr, and add it to position \@pos of 
 * \@ptr list of #$field->{typename} object. The created #$field->{typename} object is
 * stored in \@ret.
 *
 * If \@pos is #IDMEF_LIST_APPEND, \@object will be inserted at the tail of the list.
 * If \@pos is #IDMEF_LIST_PREPEND, \@object will be inserted at the head of the list.
 *
 * Returns: 0 on success, or a negative value if an error occured.
 */
int idmef_$struct->{short_typename}_new_$field->{short_name}($struct->{typename} *ptr, $field->{typename} **ret, int pos)
\{	
        int retval;

	retval = $new_field_function;
	if ( retval < 0 )
		return retval;
	
        list_insert(&ptr->$field->{name}, &(*ret)->list, pos);

        return 0;
\}

");
}

sub	struct_fields
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;

    foreach my $field ( @{ $struct->{field_list} } ) {
	$self->struct_field_normal($tree, $struct, $field) if ( $field->{metatype} & &METATYPE_NORMAL );
 	$self->struct_field_list($tree, $struct, $field) if ( $field->{metatype} & &METATYPE_LIST );
 	$self->struct_field_union($tree, $struct, $field) if ( $field->{metatype} & &METATYPE_UNION );
    }
}

sub	struct
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;

    $self->struct_desc($tree, $struct);
}

sub	struct_func
{
    my	$self = shift;
    my	$tree = shift;
    my	$struct = shift;

    $self->struct_constructor($tree, $struct);
    $self->struct_ref($tree, $struct);
    $self->struct_get_child($tree, $struct);
    $self->struct_new_child($tree, $struct);
    $self->struct_destroy_internal($tree, $struct);
    $self->struct_destroy($tree, $struct);
    $self->struct_fields($tree, $struct);
}

sub	enum
{
    my	$self = shift;
    my	$tree = shift;
    my	$enum = shift;

    $self->output("
/**
 * idmef_$enum->{short_typename}_to_numeric:
 * \@name: pointer to an IDMEF string representation of a #$enum->{typename} value.
 *
 * Returns: the numeric equivalent of \@name, or -1 if \@name is not valid.
 */
$enum->{typename} idmef_$enum->{short_typename}_to_numeric(const char *name)
\{
        int i;
        const struct {
              $enum->{typename} val;
              const char *name;
        } tbl[] = {");

    foreach my $field ( @{ $enum->{field_list} } ) {
	my $fieldname;

	if ( $field->{text} ) {
	    $fieldname = $field->{text};
	} else {
            my $fullname = 'IDMEF_' . uc($enum->{short_typename});

	    $fieldname = $field->{name};
	    $fieldname =~ s/^${fullname}_//;
	    $fieldname = lc($fieldname);
	    $fieldname =~ tr/_/-/;
        }

	$self->output("
            { $field->{name}, \"$fieldname\" },");
    }

   $self->output("
        };

        for ( i = 0; i < sizeof(tbl) / sizeof(*tbl); i++ ) {
                if ( strcasecmp(name, tbl[i].name) == 0 )
                        return tbl[i].val;
        }

        return prelude_error(PRELUDE_ERROR_IDMEF_UNKNOWN_ENUM_STRING);
\}	
");

        $self->output("
/**
 * idmef_$enum->{short_typename}_to_string:
 * \@val: an enumeration value for #$enum->{typename}.
 *
 * Return the IDMEF string equivalent of \@val provided #$enum->{typename} value.
 *
 * Returns: a pointer to the string describing \@val, or NULL if \@val is invalid.
 */
const char *idmef_$enum->{short_typename}_to_string($enum->{typename} val)
\{
        const struct {
              $enum->{typename} val;
              const char *name;
        } tbl[] = {");

    my $cnt = 0;

    foreach my $field ( @{ $enum->{field_list} } ) {

	if ( $cnt == 0 && $field->{value} != 0 ) {
	    $self->output("{ 0, NULL },");
	}

	my $fieldname;

	if ( $field->{text} ) {
	    $fieldname = $field->{text};
	} else {
	    my $fullname = 'IDMEF_' . uc($enum->{short_typename});
	    
	    $fieldname = $field->{name};
	    $fieldname =~ s/^${fullname}_//;
	    $fieldname = lc($fieldname);
	    $fieldname =~ tr/_/-/;
        }

	$self->output("
                { $field->{name}, \"$fieldname\" },");

	$cnt++;
    }

    $self->output("
        };

        if ( val < 0 || val >= (sizeof(tbl) / sizeof(*tbl)) )
	        return NULL;

        return tbl[val].name;
\}
");
}


sub footer 
{
    my $self = shift;

    $self->output("
void idmef_message_set_pmsg(idmef_message_t *message, prelude_msg_t *msg)
\{
        message->pmsg = msg;
\}


prelude_msg_t *idmef_message_get_pmsg(idmef_message_t *message)
\{
        return message->pmsg;
\}


/**
 * idmef_message_destroy:
 * \@ptr: pointer to a #idmef_message_t object.
 *
 * Destroy \@ptr and all of it's children.
 * The objects are only destroyed if their reference count reach zero.
 */
void idmef_message_destroy(idmef_message_t *ptr)
\{
        if ( --ptr->refcount )
                return;

        idmef_message_destroy_internal(ptr);

        if ( ptr->pmsg )
                prelude_msg_destroy(ptr->pmsg);

        free(ptr);
\}
");

}
1;
