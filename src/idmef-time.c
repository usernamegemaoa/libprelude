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


#include <inttypes.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef __linux__
# define __USE_XOPEN
#endif

#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "ntp.h"
#include "prelude-log.h"
#include "idmef-time.h"



/* 
 * from man (3) timegm on Debian GNU/Linux
 */
static time_t my_timegm(struct tm *tm)
{
	time_t retval;
	char *tz;

	tz = getenv("TZ");
	setenv("TZ", "", 1);
	tzset();

	retval = mktime(tm);

	if ( tz )
		setenv("TZ", tz, 1);
	else
		unsetenv("TZ");

	tzset();

	return retval;
}



static char *parse_time_ymd(struct tm *tm, const char *buf)
{
	char *ptr;

	ptr = strptime(buf, "%Y-%m-%d", tm);
	if ( ! ptr )
		return NULL;

	/*
         * The IDMEF draft only permits the 'T' variant here
         */
        while ( isspace(*ptr) )
                ptr++;
        
        if ( *ptr == 'T' )
                ptr++;

	return ptr;
}



static char *parse_time_hmsu(struct tm *tm, uint32_t *usec, const char *buf)
{
	char *ptr;
	int fraction;

	ptr = strptime(buf, "%H:%M:%S", tm);
	if ( ! ptr )
		return NULL;

	if ( *ptr == '.' || *ptr == ',' ) {
		ptr++;

		if ( sscanf(ptr, "%u", &fraction) < 1 )
			return NULL;

		*usec = fraction * 10000;

		while ( isdigit(*ptr) )
			ptr++;
	}

	return ptr;
}



static int parse_time_gmt(struct tm *tm, const char *buf)
{
        int ret;
        unsigned int offset_hour, offset_min;

        /*
         * if UTC, do nothing.
         */
        if ( *buf == 'Z' )
                return 0;
        
        ret = sscanf(buf + 1, "%2u:%2u", &offset_hour, &offset_min);
        if ( ret != 2 )
                return -1;

        if ( *buf == '+' ) {
                tm->tm_min -= offset_min;
                tm->tm_hour -= offset_hour;
        }

        else if ( *buf == '-' ) {
                tm->tm_min += offset_min;
                tm->tm_hour += offset_hour;
        }
        
        else
                return -1;
        
        return 0;
}



/*
 * Convert a string to a time. The input format is the one specified
 * by the IDMEF draft (v. 0.10, section 3.2.6), with the following
 * changes:
 *
 *  - Date and time can be separated with a white space, instead of 'T'
 *
 *  - With timezone specification, only the first two digits are analyzed
 *    (e.g. both +01:23 and +0123 are valid, but both evaluate to UTC+01). 
 *
 *  - Timezone specification may be skipped (will assume UTC). 
 */
int idmef_time_set_string(idmef_time_t *time, const char *buf)
{
	char *ptr;
	struct tm tm;
        int is_localtime = 1, ret;

        ret = idmef_time_set_ntp_timestamp(time, buf);
	if ( ret == 0 )
		return 0;
        
	memset(&tm, 0, sizeof(tm));
	tm.tm_isdst = -1;

	ptr = parse_time_ymd(&tm, buf);
	if ( ! ptr )
		return -1;
        
	if ( *ptr ) {
		ptr = parse_time_hmsu(&tm, &time->usec, ptr);
		if ( ! ptr )
			return -1;

		if ( *ptr ) {
                        ret = parse_time_gmt(&tm, ptr);
                        if ( ret < 0 )
                                return -1;

                        is_localtime = 0;
		}
	}

	time->sec = (is_localtime) ? mktime(&tm) : my_timegm(&tm);

	return 0;
}



idmef_time_t *idmef_time_new_string(const char *buf)
{
	idmef_time_t *time;

	time = idmef_time_new();
	if ( ! time )
		return NULL;

	if ( idmef_time_set_string(time, buf) < 0 ) {
		free(time);
		return NULL;
	}

	return time;
}



int idmef_time_set_ntp_timestamp(idmef_time_t *time, const char *buf)
{
	l_fp ts;
        struct timeval tv;
        unsigned ts_mask = TS_MASK;
	unsigned ts_roundbit = TS_ROUNDBIT;
	
        
	if ( sscanf(buf, "%lx.%lx", (unsigned long *) &ts.l_ui, (unsigned long *) &ts.l_uf) < 2 )
		return -1;

	/* 
	 * This transformation is a reverse form of the one found in
	 *  idmef_get_ntp_timestamp()
	 */
	ts.l_ui -= JAN_1970;
	ts.l_uf -= ts_roundbit;
	ts.l_uf &= ts_mask;
	TSTOTV(&ts, &tv);

	time->sec = tv.tv_sec;
	time->usec = tv.tv_usec;

	return 0;
}




int idmef_time_get_ntp_timestamp(const idmef_time_t *time, char *outptr, size_t size)
{
        l_fp ts;
        struct timeval tv;
        unsigned ts_mask = TS_MASK;             /* defaults to 20 bits (us) */
        unsigned ts_roundbit = TS_ROUNDBIT;     /* defaults to 20 bits (us) */

        tv.tv_sec = idmef_time_get_sec(time);
        tv.tv_usec = idmef_time_get_usec(time);
        
        sTVTOTS(&tv, &ts);

        ts.l_ui += JAN_1970;                    /* make it time since 1900 */
        ts.l_uf += ts_roundbit;
        ts.l_uf &= ts_mask;
        
        return snprintf(outptr, size, "0x%08lx.0x%08lx", (unsigned long) ts.l_ui, (unsigned long) ts.l_uf);
}





/**
 * idmef_get_db_timestamp:
 * @time: Pointer to an IDMEF time structure.
 * @outptr: Output buffer.
 * @size: size of the output buffer.
 *
 * Translate @time to a string suitable for insertion into a database field
 * of type DATETIME.
 */
int idmef_time_get_db_timestamp(const idmef_time_t *time, char *outptr, size_t size) 
{
        int ret;
        time_t sec;
        struct tm utc;
        
	sec = idmef_time_get_sec(time);
        
        /*
         * Convert from localtime to UTC.
         */
        if ( ! gmtime_r(&sec, &utc) ) {
                log(LOG_ERR, "error converting timestamp to gmt time.\n");
                return -1;
        }
        
        ret = strftime(outptr, size, "%Y-%m-%d%H:%M:%S", &utc);
        if ( ret == 0 ) {
                log(LOG_ERR, "error converting UTC time to string.\n");
                return -2;
        }
        
        return ret;
}




/**
 * idmef_get_idmef_timestamp:
 * @time: Pointer to an IDMEF time structure.
 * @outptr: Output buffer.
 * @size: size of the output buffer.
 *
 * Translate @time to an user readable string following the IDMEF
 * specification.
 *
 * Returns: 0 on success, -1 if an error occured.
 */
int idmef_time_get_idmef_timestamp(const idmef_time_t *time, char *outptr, size_t size)
{
        char gmt_sign;
        time_t time_utc, sec;
        int ret, len = 0, gmt_offset;
	struct tm tm_localtime, tm_utc;

	sec = idmef_time_get_sec(time);
	if ( ! localtime_r(&sec, &tm_localtime) ) {
		log(LOG_ERR, "error converting timestamp to localtime.\n");
		return -1;
	}
                
        if ( ! gmtime_r(&sec, &tm_utc) ) {
                log(LOG_ERR, "error converting timestamp to utc time.\n");
                return -2;
        }

	tm_utc.tm_isdst = -1;
	time_utc = mktime(&tm_utc);
	gmt_offset = sec - time_utc;

        len += ret = strftime(outptr, size, "%Y-%m-%dT%H:%M:%S", &tm_localtime);
        if ( ret < 0 ) {
                log(LOG_ERR, "error converting localtime to string.\n");
                return -3;
        }

        len += ret = snprintf(outptr + len, size - len, ".%02u", idmef_time_get_usec(time) / 10000 % 60);
	if ( ret < 0 )
		return -4;

	if ( gmt_offset < 0 ) {
		gmt_sign = '-';
		gmt_offset = - gmt_offset;

	} else {
		gmt_sign = '+';
	}

	len += ret = snprintf(outptr + len, size - len, "%c%.2u:%.2u", 
			      gmt_sign, gmt_offset / 3600, gmt_offset % 3600 / 60);
	if ( ret < 0 )
		return -5;

        return len;
}




/**
 * idmef_get_timestamp:
 * @time: Pointer to an IDMEF time structure.
 * @outptr: Output buffer.
 * @size: size of the output buffer.
 *
 * Translate @time to an user readable string.
 *
 * Returns: 0 on success, -1 if an error occured.
 */
int idmef_time_get_timestamp(const idmef_time_t *time, char *outptr, size_t size)
{
        time_t sec;
        struct tm lt;
        int ret, len = 0;

	sec = idmef_time_get_sec(time);
        
        if ( ! localtime_r( (const time_t *) &sec, &lt) ) {
                log(LOG_ERR, "error converting timestamp to local time.\n");
                return -1;
        }
        
        len += ret = strftime(outptr, size, "%Y-%m-%d %H:%M:%S", &lt);
        if ( ret == 0 ) {
                log(LOG_ERR, "error converting UTC time to string.\n");
                return -2;
        }

        len += ret = snprintf(outptr + len, size - len, ".%03u", idmef_time_get_usec(time) / 1000);
        
        len += ret = strftime(outptr + len, size - len, "%z", &lt);
        if ( ret == 0 ) {
                log(LOG_ERR, "error converting UTC time to string.\n");
                return -3;
        }

        return len;
}




idmef_time_t *idmef_time_new_ntp_timestamp(const char *buf)
{
	idmef_time_t *time;

	time = idmef_time_new();
	if ( ! time )
		return NULL;

	if ( idmef_time_set_ntp_timestamp(time, buf) < 0 ) {
		free(time);
		return NULL;
	}

	return time;	
}



idmef_time_t *idmef_time_new_gettimeofday(void)
{
        struct timeval tv;
	idmef_time_t *time;

        if ( gettimeofday(&tv, NULL) == -1 )
		return NULL;

	time = idmef_time_new();
	if ( ! time )
		return NULL;

	idmef_time_set_sec(time, tv.tv_sec);
	idmef_time_set_usec(time, tv.tv_usec);

	return time;	
}



idmef_time_t *idmef_time_new(void)
{
	idmef_time_t *time; 

	time = calloc(1, sizeof(*time));
	if ( ! time )
		log(LOG_ERR, "memory exhausted.\n");

	return time;
}



idmef_time_t *idmef_time_clone(const idmef_time_t *src)
{
	idmef_time_t *ret;

	ret = idmef_time_new();
	if ( ! ret )
		return NULL;

	ret->sec = src->sec;
	ret->usec = src->usec;

	return ret;
}




void idmef_time_set_sec(idmef_time_t *time, uint32_t sec)
{
	time->sec = sec;
}




void idmef_time_set_usec(idmef_time_t *time, uint32_t usec)
{
	time->usec = usec;
}




uint32_t idmef_time_get_sec(const idmef_time_t *time)
{
	return time->sec;
}




uint32_t idmef_time_get_usec(const idmef_time_t *time)
{
	return time->usec;
}



double idmef_time_get_time(const idmef_time_t *time)
{
        double ret;

        ret = time->sec;
        ret += time->usec * 1e-6;

        return ret;
}



int idmef_time_copy(idmef_time_t *dst, idmef_time_t *src)
{
	dst->sec = src->sec;
	dst->usec = src->usec;
	
	return 0;
}




void idmef_time_destroy_internal(idmef_time_t *time)
{
	/* nop */
}



void idmef_time_destroy(idmef_time_t *time)
{
	if ( time )
		free(time);
}

