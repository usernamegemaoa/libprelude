/*****
*
* Copyright (C) 2003 Yoann Vandoorselaere <yoann@prelude-ids.org>
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


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include "idmef-time.h"
#include "idmef-data.h"
#include "idmef-string.h"
#include "idmef-value-type.h"


#define GENERIC_RW_FUNC(fmt, name, type)                                           \
        static int name ## _read(idmef_value_type_t *dst, const char *buf)         \
        {                                                                          \
                return sscanf(buf, (fmt), &(dst)->data. name ##_val);              \
        }                                                                          \
                                                                                   \
        static int name ## _write(char *buf, size_t size, idmef_value_type_t *src) \
        {                                                                          \
                return snprintf(buf, size, (fmt), src->data.name ##_val);          \
        }                                                                          \



typedef struct {

        size_t len;
        
        int (*copy)(void *dst, idmef_value_type_t *src, size_t size);
        int (*clone)(idmef_value_type_t *dst, idmef_value_type_t *src, size_t size);
        
        void (*destroy)(idmef_value_type_t *type);
        int (*compare)(idmef_value_type_t *t1, idmef_value_type_t *t2, size_t size, idmef_relation_t relation);
        
        int (*read)(idmef_value_type_t *dst, const char *buf);
        int (*write)(char *buf, size_t size, idmef_value_type_t *src);

} idmef_value_type_operation_t;


GENERIC_RW_FUNC("%hd", int16, int16_t)
GENERIC_RW_FUNC("%hu", uint16, uint16_t)
GENERIC_RW_FUNC("%d", int32, int32_t)
GENERIC_RW_FUNC("%u", uint32, uint32_t)
GENERIC_RW_FUNC("%lld", int64, int64_t)
GENERIC_RW_FUNC("%llu", uint64, uint64_t)
GENERIC_RW_FUNC("%f", float, float)
GENERIC_RW_FUNC("%lf", double, double)




/*
 * generic functions.
 */
static int generic_copy(void *dst, idmef_value_type_t *src, size_t size)
{
        memcpy(dst, &src->data, size);
        return 0;
}




static int generic_clone(idmef_value_type_t *dst, idmef_value_type_t *src, size_t size)
{
        memcpy(&dst->data, &src->data, size);
        return 0;
}



static int generic_compare(idmef_value_type_t *t1, idmef_value_type_t *t2, size_t size, idmef_relation_t relation)
{
        int ret;

        ret = memcmp(&t1->data, &t2->data, size);
        
        if ( ret == 0 && relation & IDMEF_RELATION_EQUAL ) 
                return 0;

        else if ( ret < 0 && relation & (IDMEF_RELATION_NOT_EQUAL|IDMEF_RELATION_LESSER) )
                return 0;

        else if ( ret > 0 && relation & (IDMEF_RELATION_NOT_EQUAL|IDMEF_RELATION_GREATER) )
                return 0;

        return -1;
}




/*
 * time specific function.
 */
static int time_compare(idmef_value_type_t *t1, idmef_value_type_t *t2, size_t size, idmef_relation_t relation)
{
        double time1, time2;

        time1 = idmef_time_get_time(t1->data.time_val);
        time2 = idmef_time_get_time(t2->data.time_val);

        if ( relation & IDMEF_RELATION_EQUAL && time1 == time2 )
                return 0;

        else if ( relation & (IDMEF_RELATION_NOT_EQUAL|IDMEF_RELATION_LESSER) && time1 < time2 )
                return 0;

        else if ( relation & (IDMEF_RELATION_NOT_EQUAL|IDMEF_RELATION_GREATER) && time1 > time2 )
                return 0;

        return -1;
}



static int time_read(idmef_value_type_t *dst, const char *buf)
{
	dst->data.time_val = idmef_time_new_string(buf);
        if ( ! dst->data.time_val )
                return -1;

        return 0;
}



static int time_write(char *buffer, size_t size, idmef_value_type_t *src)
{
	return idmef_time_get_idmef_timestamp(src->data.time_val, buffer, size);
}



static int time_copy(void *dst, idmef_value_type_t *src, size_t size)
{
        return idmef_time_copy(dst, src->data.time_val);
}



static int time_clone(idmef_value_type_t *dst, idmef_value_type_t *src, size_t size)
{
        dst->data.time_val = idmef_time_clone(src->data.time_val);
        if ( ! dst->data.time_val )
                return -1;

        return 0;
}



static void time_destroy(idmef_value_type_t *type)
{
	idmef_time_destroy(type->data.time_val);
}



/*
 *
 */
static int string_compare(idmef_value_type_t *t1, idmef_value_type_t *t2, size_t size, idmef_relation_t relation)
{
        const char *s1, *s2;
        
        s1 = idmef_string_get_string(t1->data.string_val);
        s2 = idmef_string_get_string(t2->data.string_val);
        
        if ( relation & IDMEF_RELATION_EQUAL && strcmp(s1, s2) == 0 )
                return 0;

        else if ( relation & IDMEF_RELATION_NOT_EQUAL && strcmp(s1, s2) != 0 )
                return 0;
        
        else if ( relation & IDMEF_RELATION_SUBSTR && strstr(s1, s2) )
                return 0;
        
        return -1;
}



static int string_read(idmef_value_type_t *dst, const char *buf)
{        
        dst->data.string_val = idmef_string_new_dup(buf);
        if ( ! dst->data.string_val )
                return -1;

        return 0;        
}



static int string_copy(void *dst, idmef_value_type_t *src, size_t size)
{        
        return idmef_string_copy_dup(dst, src->data.string_val);
}



static int string_clone(idmef_value_type_t *dst, idmef_value_type_t *src, size_t size)
{        
        dst->data.string_val = idmef_string_clone(src->data.string_val);
        if ( ! dst->data.string_val )
                return -1;

        return 0;
}


static void string_destroy(idmef_value_type_t *type)
{        
        idmef_string_destroy(type->data.string_val);
}



static int string_write(char *buf, size_t size, idmef_value_type_t *src)
{
        return snprintf(buf, size, "%s", idmef_string_get_string(src->data.string_val));
}



/*
 * data specific functions
 */
static int data_compare(idmef_value_type_t *t1, idmef_value_type_t *t2, size_t len, idmef_relation_t relation)
{
        int ret;
        size_t len1, len2;
        idmef_data_t *d1 = t1->data.data_val, *d2 = t2->data.data_val;
        
        len1 = idmef_data_get_len(d1);
        len2 = idmef_data_get_len(d2);

        if ( len1 == len2 )
                ret = memcmp(idmef_data_get_data(d1), idmef_data_get_data(d2), len2);
        else 
                ret = (len1 > len2) ? 1 : -1;   
        
        if ( ret == 0 && relation & IDMEF_RELATION_EQUAL )
                return 0;

        else if ( ret < 0 && relation & (IDMEF_RELATION_NOT_EQUAL|IDMEF_RELATION_LESSER) )
                return 0;

        else if ( ret > 0 && relation & (IDMEF_RELATION_NOT_EQUAL|IDMEF_RELATION_GREATER) )
                return 0;
                                
        return -1;
}



static int data_read(idmef_value_type_t *dst, const char *src)
{
        dst->data.data_val = idmef_data_new_dup(src, strlen(src) + 1);
        if ( ! dst->data.data_val )
                return -1;

        return 0;
}



static int data_write(char *buf, size_t size, idmef_value_type_t *src)
{
        return idmef_data_to_string(src->data.data_val, buf, size);
}



static int data_copy(void *dst, idmef_value_type_t *src, size_t size)
{
        return idmef_data_copy_dup(dst, src->data.data_val);
}



static int data_clone(idmef_value_type_t *dst, idmef_value_type_t *src, size_t size)
{
        dst->data.data_val = idmef_data_clone(src->data.data_val);
        if ( ! dst->data.data_val )
                return -1;

        return 0;
}



static void data_destroy(idmef_value_type_t *type)
{
        idmef_data_destroy(type->data.data_val);
}



static idmef_value_type_operation_t ops_tbl[] = {
        { 0, NULL, NULL, NULL, NULL, NULL, NULL                                                           },
        { sizeof(int16_t), generic_copy, generic_clone, NULL, generic_compare, int16_read, int16_write    },
        { sizeof(uint16_t), generic_copy, generic_clone, NULL, generic_compare, uint16_read, uint16_write },
        { sizeof(int32_t), generic_copy, generic_clone, NULL, generic_compare, int32_read, int32_write    },
        { sizeof(uint32_t), generic_copy, generic_clone, NULL, generic_compare, uint32_read, uint32_write },
        { sizeof(int64_t), generic_copy, generic_clone, NULL, generic_compare, int64_read, int64_write    },
        { sizeof(uint64_t), generic_copy, generic_clone, NULL, generic_compare, uint64_read, uint64_write },
        { sizeof(float), generic_copy, generic_clone, NULL, generic_compare, float_read, float_write      },
        { sizeof(double), generic_copy, generic_clone, NULL, generic_compare, double_read, double_write   },
        { 0, string_copy, string_clone, string_destroy, string_compare, string_read, string_write         },
        { 0, time_copy, time_clone, time_destroy, time_compare, time_read, time_write                     }, 
        { 0, data_copy, data_clone, data_destroy, data_compare, data_read, data_write                     },
        { sizeof(int), generic_copy, generic_clone, NULL, NULL, NULL, NULL  /* type_enum */               },
};



static int is_type_valid(idmef_value_type_id_t type) 
{
        if ( type < 0 || type > (sizeof(ops_tbl) / sizeof(*ops_tbl)) )
                return -1;

        return 0;
}




int idmef_value_type_clone(idmef_value_type_t *dst, idmef_value_type_t *src)
{
        int ret;

        assert(dst->id == src->id);
        
        ret = is_type_valid(dst->id);
        if ( ret < 0 )
                return -1;

        if ( ! ops_tbl[dst->id].clone )
                return -1;

        return ops_tbl[dst->id].clone(dst, src, ops_tbl[dst->id].len);
}




int idmef_value_type_copy(void *dst, idmef_value_type_t *src)
{
        int ret;

        ret = is_type_valid(src->id);
        if ( ret < 0 )
                return -1;

        if ( ! ops_tbl[src->id].copy )
                return -1;

        return ops_tbl[src->id].copy(dst, src, ops_tbl[src->id].len);
}




int idmef_value_type_compare(idmef_value_type_t *type1, idmef_value_type_t *type2, idmef_relation_t relation)
{
        int ret;

        assert(type1->id == type2->id);
                
        ret = is_type_valid(type1->id);
        if ( ret < 0 )
                return ret;

        if ( ! ops_tbl[type1->id].compare )
                return -1;
        
        return ops_tbl[type1->id].compare(type1, type2, ops_tbl[type1->id].len, relation);
}




int idmef_value_type_read(idmef_value_type_t *dst, const char *buf) 
{
        int ret;
        
        ret = is_type_valid(dst->id);
        if ( ret < 0 ) 
                return ret;

        if ( ! ops_tbl[dst->id].read )
                return -1;
        
        return ops_tbl[dst->id].read(dst, buf);
}




int idmef_value_type_write(char *dst, size_t size, idmef_value_type_t *src)
{
        int ret;
        
        ret = is_type_valid(src->id);
        if ( ret < 0 )
                return ret;

        if ( ! ops_tbl[src->id].write )
                return -1;
        
        return ops_tbl[src->id].write(dst, size, src);
}



void idmef_value_type_destroy(idmef_value_type_t *type)
{
        int ret;
        
        ret = is_type_valid(type->id);
        if ( ret < 0 )
                return;

        if ( ! ops_tbl[type->id].destroy )
                return;
        
        ops_tbl[type->id].destroy(type);
}