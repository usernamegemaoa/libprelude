/*****
*
* Copyright (C) 2003 Nicolas Delon <delon.nicolas@wanadoo.fr>
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

#include "libmissing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "prelude-inttypes.h"
#include "prelude-string.h"
#include "idmef-data.h"


/*
 * Data structure may be free'd
 */
#define IDMEF_DATA_OWN_STRUCTURE  0x1

/*
 * Data content may be free'd
 */
#define IDMEF_DATA_OWN_DATA       0x2


#define IDMEF_DATA_DECL(idmef_data_type, c_type, name)		\
idmef_data_t *idmef_data_new_ ## name(c_type val)		\
{								\
        idmef_data_t *ptr;					\
								\
        ptr = idmef_data_new();					\
        if ( ! ptr )						\
                return NULL;					\
								\
        idmef_data_set_ ## name(ptr, val);			\
								\
        return ptr;						\
}								\
								\
void idmef_data_set_ ## name(idmef_data_t *ptr, c_type val)	\
{								\
        idmef_data_destroy_internal(ptr);			\
        ptr->type = idmef_data_type;				\
        ptr->len = sizeof(val);					\
	ptr->data.name ## _data = val;				\
}								\
								\
c_type idmef_data_get_ ## name(const idmef_data_t *ptr)		\
{								\
	return ptr->data.name ## _data;				\
}


IDMEF_DATA_DECL(IDMEF_DATA_TYPE_CHAR, char, char)
IDMEF_DATA_DECL(IDMEF_DATA_TYPE_BYTE, uint8_t, byte)
IDMEF_DATA_DECL(IDMEF_DATA_TYPE_UINT32, uint32_t, uint32)
IDMEF_DATA_DECL(IDMEF_DATA_TYPE_UINT64, uint64_t, uint64)
IDMEF_DATA_DECL(IDMEF_DATA_TYPE_FLOAT, float, float)



/**
 * idmef_data_new:
 *
 * Allocate a new idmef_data_t object.
 *
 * Returns: A new idmef_data_t object, or NULL if an error occured.
 */
idmef_data_t *idmef_data_new(void)
{
        idmef_data_t *ret;

        ret = calloc(1, sizeof(*ret));
        if ( ! ret )
                return NULL;

        ret->refcount = 1;
        ret->flags |= IDMEF_DATA_OWN_STRUCTURE;

        return ret;
}



idmef_data_t *idmef_data_ref(idmef_data_t *data)
{
        data->refcount++;
        return data;
}



int idmef_data_set_ptr_ref_fast(idmef_data_t *data, idmef_data_type_t type, const void *ptr, size_t len)
{
	idmef_data_destroy_internal(data);

	data->type = type;
	data->data.ro_data = ptr;
	data->len = len;
	
	return 0;
}



int idmef_data_set_ptr_dup_fast(idmef_data_t *data, idmef_data_type_t type, const void *ptr, size_t len)
{
	void *new;

	idmef_data_destroy_internal(data);

	new = malloc(len);
	if ( ! new )
		return -1;

	memcpy(new, ptr, len);

	data->type = type;
	data->data.rw_data = new;
	data->len = len;
	data->flags |= IDMEF_DATA_OWN_DATA;

	return 0;
}



int idmef_data_set_ptr_nodup_fast(idmef_data_t *data, idmef_data_type_t type, void *ptr, size_t len)
{
	idmef_data_destroy_internal(data);

	data->type = type;
	data->data.rw_data= ptr;
	data->len = len;
	data->flags |= IDMEF_DATA_OWN_DATA;

	return 0;
}



idmef_data_t *idmef_data_new_ptr_ref_fast(idmef_data_type_t type, const void *ptr, size_t len)
{
	idmef_data_t *data;

	data = idmef_data_new();
	if ( ! data )
		return NULL;

	if ( idmef_data_set_ptr_ref_fast(data, type, ptr, len) < 0 ) {
		idmef_data_destroy(data);
		return NULL;
	}

	return data;
}



idmef_data_t *idmef_data_new_ptr_dup_fast(idmef_data_type_t type, const void *ptr, size_t len)
{
	idmef_data_t *data;

	data = idmef_data_new();
	if ( ! data )
		return NULL;

	if ( idmef_data_set_ptr_dup_fast(data, type, ptr, len) < 0 ) {
		idmef_data_destroy(data);
		return NULL;
	}

	return data;
}



idmef_data_t *idmef_data_new_ptr_nodup_fast(idmef_data_type_t type, void *ptr, size_t len)
{
	idmef_data_t *data;

	data = idmef_data_new();
	if ( ! data )
		return NULL;
	
	if ( idmef_data_set_ptr_nodup_fast(data, type, ptr, len) < 0 ) {
		idmef_data_destroy(data);
		return NULL;
	}

	return data;
}



int idmef_data_set_char_string_dup_fast(idmef_data_t *data, const char *str, size_t len)
{
	char *new;

	new = strndup(str, len);
	if ( ! new )
		return -1;

	return idmef_data_set_ptr_nodup_fast(data, IDMEF_DATA_TYPE_CHAR_STRING, new, len + 1);
}



idmef_data_t *idmef_data_new_char_string_dup_fast(const char *str, size_t len)
{
	idmef_data_t *data;

	data = idmef_data_new();
	if ( ! data )
		return NULL;

	if ( idmef_data_set_char_string_dup_fast(data, str, len) < 0 ) {
		idmef_data_destroy(data);
		return NULL;
	}

	return data;		
}



/**
 * idmef_data_copy_ref:
 * @dst: Destination #idmef_data_t object.
 * @src: Source #idmef_data_t object.
 *
 * Make @dst reference the same buffer as @src.
 *
 * Returns: 0 on success, a negative value if an error occured.
 */
int idmef_data_copy_ref(idmef_data_t *dst, const idmef_data_t *src)
{
        idmef_data_destroy_internal(dst);

	dst->type = src->type;
        dst->len = src->len;
        dst->data = src->data;
        dst->flags &= ~IDMEF_DATA_OWN_DATA;

        return 0;
}




/**
 * idmef_data_copy_dup:
 * @dst: Destination #idmef_data_t object.
 * @src: Source #idmef_data_t object.
 *
 * Copy @src to @dst, including the associated buffer
 * this is an alternative to idmef_data_clone().
 *
 * Returns: 0 on success, a negative value if an error occured.
 */
int idmef_data_copy_dup(idmef_data_t *dst, const idmef_data_t *src)
{
        idmef_data_destroy_internal(dst);

	dst->type = src->type;
	dst->flags |= IDMEF_DATA_OWN_DATA;
	dst->len = src->len;

	if ( src->type == IDMEF_DATA_TYPE_CHAR_STRING || src->type == IDMEF_DATA_TYPE_BYTE_STRING ) {
		dst->data.rw_data = malloc(src->len);
		if ( ! dst->data.rw_data )
			return -1;
		memcpy(dst->data.rw_data, src->data.ro_data, src->len);
	} else {
		dst->data = src->data;
	}

        return 0;
}




/**
 * idmef_data_clone:
 * @data: Pointer to an #idmef_data_t object.
 *
 * Make up an exact copy of the @data object, and it's content.
 *
 * Returns: A new #idmef_data_t object, or NULL if an error occured.
 */
idmef_data_t *idmef_data_clone(const idmef_data_t *data)
{
        idmef_data_t *ret;
        
        ret = idmef_data_new();
        if ( ! ret )
                return NULL;

	if ( idmef_data_copy_dup(ret, data) < 0 ) {
		idmef_data_destroy(ret);
		return NULL;
	}

	return ret;
}



const char *idmef_data_get_char_string(const idmef_data_t *data)
{
	return data->data.ro_data;
}



const unsigned char *idmef_data_get_byte_string(const idmef_data_t *data)
{
	return data->data.ro_data;
}



/**
 * idmef_data_get_type
 * @data: Pointer to an #idmef_data_t object.
 *
 * Returns: the type of the embedded data.
 */
idmef_data_type_t idmef_data_get_type(const idmef_data_t *data)
{
	return data->type;
}




/**
 * idmef_data_get_len:
 * @data: Pointer to an #idmef_data_t object.
 *
 * Returns: the len of data contained within @data object.
 */
size_t idmef_data_get_len(const idmef_data_t *data)
{
        return data->len;
}




/**
 * idmef_data_get_data:
 * @data: Pointer to an #idmef_data_t object.
 *
 * Returns: the data contained within @data object.
 */
const void *idmef_data_get_data(const idmef_data_t *data)
{
	switch ( data->type ) {
	case IDMEF_DATA_TYPE_UNKNOWN:
		return NULL;

	case IDMEF_DATA_TYPE_CHAR_STRING: case IDMEF_DATA_TYPE_BYTE_STRING:
		return data->data.ro_data;

	default:
		return &data->data;
	}

	return NULL;
}



/**
 * idmef_data_is_empty:
 * @data: Pointer to an #idmef_data_t object.
 *
 * Returns: TRUE if empty, FALSE otherwise.
 */
prelude_bool_t idmef_data_is_empty(const idmef_data_t *data)
{
        return (data->len == 0) ? TRUE : FALSE;
}



static int bytes_to_string(prelude_string_t *out, const unsigned char *src, size_t size)
{
        size_t i;
        int ret = 0;
        unsigned char c;

        for ( i = 0; i < size; i++ ) {
                c = src[i];
                
                if ( c >= 32 && c <= 127 ) {
                        ret = prelude_string_ncat(out, &c, 1);
                        if ( ret < 0 )
                                return ret;
                        
                        continue;
                }

                switch ( c ) {
                case '\\':
                        ret = prelude_string_cat(out, "\\\\");
                        break;

                case '\r':
                        ret = prelude_string_cat(out, "\\r");
                        break;

                case '\n':
                        ret = prelude_string_cat(out, "\\n");
                        break;

                case '\t':
                        ret = prelude_string_cat(out, "\\t");
                        break;

                default:
                        ret = prelude_string_cat(out, "\\x%02x");
                        break;
                }

                if ( ret < 0 )
                        return ret;
        }
        
        return ret;
}



/**
 * idmef_data_to_string:
 * @data: Pointer to an #idmef_data_t object.
 * @out: Pointer to a #prelude_string_t to store the formated data into.
 *
 * Format the data contained within @data to be printable,
 * and store the result in the provided @out buffer.
 *
 * Returns: 0 on success, a negative value if an error occured.
 */
int idmef_data_to_string(const idmef_data_t *data, prelude_string_t *out)
{
	int ret = 0;

	switch ( data->type ) {
	case IDMEF_DATA_TYPE_UNKNOWN:
		return 0;

	case IDMEF_DATA_TYPE_CHAR:
		ret = prelude_string_sprintf(out, "%c", data->data.char_data);
		break;

	case IDMEF_DATA_TYPE_BYTE:
		ret = prelude_string_sprintf(out, "%hhu", data->data.byte_data);
		break;

	case IDMEF_DATA_TYPE_UINT32:
		ret = prelude_string_sprintf(out, "%u", data->data.uint32_data);
		break;

	case IDMEF_DATA_TYPE_UINT64:
		ret = prelude_string_sprintf(out, "%llu", data->data.uint64_data);
		break;

	case IDMEF_DATA_TYPE_FLOAT:
		ret = prelude_string_sprintf(out, "%f", data->data.float_data);
		break;

	case IDMEF_DATA_TYPE_CHAR_STRING:
		ret = prelude_string_sprintf(out, "%s", (const char *) data->data.ro_data);
		break;

	case IDMEF_DATA_TYPE_BYTE_STRING:
		ret = bytes_to_string(out, data->data.ro_data, data->len);
		break;
	}

	return ret;
}



/*
 *  This function cannot be declared static because its invoked
 *  from idmef-tree-wrap.c
 */
void idmef_data_destroy_internal(idmef_data_t *ptr)
{        
        if ( (ptr->type == IDMEF_DATA_TYPE_CHAR_STRING || ptr->type == IDMEF_DATA_TYPE_BYTE_STRING) &&
	     ptr->flags & IDMEF_DATA_OWN_DATA ) {
                free(ptr->data.rw_data);
                ptr->data.rw_data = NULL;
        }

        /*
         * free() should be done by the caller
         */
}




/**
 * idmef_data_destroy:
 * @data: Pointer to an #idmef_data_t object.
 *
 * Free @data. The buffer pointed to by @data will be freed if
 * the @data object is marked as _dup or _nodup.
 */
void idmef_data_destroy(idmef_data_t *data)
{
        if ( --data->refcount )
                return;
        
        idmef_data_destroy_internal(data);
        
        if ( data->flags & IDMEF_DATA_OWN_STRUCTURE )
                free(data);
}
