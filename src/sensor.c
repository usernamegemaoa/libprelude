/*****
*
* Copyright (C) 2001, 2002 Yoann Vandoorselaere <yoann@mandrakesoft.com>
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
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <grp.h>
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>

#include "config.h"

#include "prelude-path.h"
#include "prelude-list.h"
#include "config-engine.h"
#include "plugin-common.h"
#include "prelude-log.h"

#include "prelude-io.h"
#include "prelude-message.h"
#include "prelude-client.h"
#include "prelude-client-mgr.h"
#include "prelude-getopt.h"
#include "prelude-async.h"
#include "prelude-auth.h"
#include "sensor.h"
#include "client-ident.h"
#include "timer.h"

#include "idmef-tree.h"
#include "idmef-tree-func.h"

/*
 * send an heartbeat every hours.
 */
#define DEFAULT_HEARTBEAT_REPEAT_TIME 60 * 60


/*
 * Path to the defalt sensor configuration file.
 */
#define DEFAULT_SENSOR_CONFIG SENSORS_CONFIG_DIR"/sensors-default.conf"

/*
 * default heartbeat repeat time is 60 minutes.
 */
static void *heartbeat_data;
static prelude_timer_t heartbeat_timer;
static int heartbeat_repeat_time = 60 * 60;
static void (*send_heartbeat_cb)(void *data) = NULL;

/*
 * Analyzer informations
 */
static idmef_node_t node;
static idmef_address_t address;
static char *process_name = NULL, *process_path = NULL;

/*
 *
 */
static prelude_client_mgr_t *manager_list = NULL;
 

static int setup_analyzer_node_location(const char *arg) 
{
        idmef_string_set(&node.location, arg);
        return prelude_option_success;
}



static int setup_analyzer_node_name(const char *arg) 
{
        idmef_string_set(&node.name, arg);
        return prelude_option_success;
}



static int setup_analyzer_node_category(const char *arg) 
{
        node.category = atoi(arg);
        return prelude_option_success;
}



static int setup_analyzer_node_address_address(const char *arg)
{
        idmef_string_set(&address.address, arg);
        return prelude_option_success;
}


static int setup_analyzer_node_address_netmask(const char *arg)
{
        idmef_string_set(&address.netmask, arg);
        return prelude_option_success;
}


static int setup_analyzer_node_address_category(const char *arg) 
{
        address.category = atoi(arg);
        return prelude_option_success;
}


static int setup_analyzer_node_address_vlan_num(const char *arg) 
{
        address.vlan_num = atoi(arg);
        return prelude_option_success;
}



static int setup_analyzer_node_address_vlan_name(const char *arg) 
{        
        idmef_string_set(&address.vlan_name, arg);
        return prelude_option_success;
}


static int setup_address(const char *arg) 
{
        list_add_tail(&address.list, &node.address_list);
        return prelude_option_success;
}



static int setup_manager_addr(const char *arg) 
{               
        manager_list = prelude_client_mgr_new(PRELUDE_CLIENT_TYPE_SENSOR, arg);
        if ( ! manager_list ) 
                return prelude_option_error;
                
        return prelude_option_success;
}




static int setup_heartbeat_repeat_time(const char *arg) 
{
        heartbeat_repeat_time = atoi(arg) * 60;
        return prelude_option_success;
}



static int parse_argument(const char *filename, int argc, char **argv) 
{
        int ret;
        int old_flags;
        char *ptr;
        prelude_option_t *opt;

        ptr = strrchr(argv[0], '/');
        if ( ptr ) {
                *ptr = '\0';
                process_path = argv[0];
                process_name = strdup(ptr + 1);

        } else
                process_name = argv[0];
                
        /*
         * Declare library options.
         */
        opt = prelude_option_add(NULL, CLI_HOOK|CFG_HOOK|WIDE_HOOK, 0, "manager-addr",
                                 "Address where manager is listening (addr:port)",
                                 required_argument, setup_manager_addr, NULL);

        /*
         * run this one last, so that application that wish to change
         * their userID won't get a false message for sensor-adduser (--uid option).
         */
        prelude_option_set_priority(opt, option_run_last);
        
        prelude_option_add(NULL, CLI_HOOK|CFG_HOOK|WIDE_HOOK, 0, "heartbeat-time",
                           "Send hearbeat at the specified time (default 60 minutes)",
                           required_argument, setup_heartbeat_repeat_time, NULL);

        prelude_option_add(NULL, CFG_HOOK, 0, "node-name",
                           NULL, required_argument, setup_analyzer_node_name, NULL);

        prelude_option_add(NULL, CFG_HOOK, 0, "node-location",
                           NULL, required_argument, setup_analyzer_node_location, NULL);
        
        prelude_option_add(NULL, CFG_HOOK, 0, "node-category",
                           NULL, required_argument, setup_analyzer_node_category, NULL);
        
        opt = prelude_option_add(NULL, CFG_HOOK, 0, "node address",
                                 NULL, required_argument, setup_address, NULL);

        prelude_option_add(NULL, CFG_HOOK, 0, "address",
                           NULL, required_argument, setup_analyzer_node_address_address, NULL);
        
        prelude_option_add(opt, CFG_HOOK, 0, "netmask",
                           NULL, required_argument, setup_analyzer_node_address_netmask, NULL);

        prelude_option_add(opt, CFG_HOOK, 0, "category",
                           NULL, required_argument, setup_analyzer_node_address_category, NULL);

        prelude_option_add(opt, CFG_HOOK, 0, "vlan-name",
                           NULL, required_argument, setup_analyzer_node_address_vlan_name, NULL);

        prelude_option_add(opt, CFG_HOOK, 0, "vlan-num",
                           NULL, required_argument, setup_analyzer_node_address_vlan_num, NULL);
        
        /*
         * When parsing our own option, we don't want libprelude to whine
         * about unknow argument on command line (theses can be the own application
         * arguments).
         */
        prelude_option_set_warnings(~(OPT_INVAL|OPT_INVAL_ARG), &old_flags);

        /*
         * Parse configuration and command line arguments.
         */        
        ret = prelude_option_parse_arguments(NULL, filename, argc, argv);
        if ( ret == prelude_option_error ) {
                log(LOG_INFO, "error processing sensor options.\n", filename);
                goto out;
        }
        else if ( ret == prelude_option_end )
                goto out;
        
        /*
         * The sensors configuration file we just parsed doesn't contain
         * entry to specify the Manager address we should connect to.
         *
         * Here we try using the default sensors configuration file.
         */
        if ( ! manager_list ) {
           
                ret = prelude_option_parse_arguments(NULL, DEFAULT_SENSOR_CONFIG, 0, NULL);
                if ( ret < 0 ) {
                        log(LOG_INFO, "error processing generic sensors configuration file.\n");
                        goto out;
                }
                
                if ( manager_list )
                        goto out;
                
                log(LOG_INFO,
                    "No Manager were configured. You need to setup a Manager for this Sensor\n"
                    "to report events. Please use the \"manager-addr\" entry in the Sensor\n"
                    "config file or the -a and eventually -p command line options.\n");
        }

 out:
        /*
         * Destroy option list and restore old option flags.
         */
        prelude_option_set_warnings(old_flags, NULL);

        return ret;
}







/**
 * libprelude_sensor_init:
 * @sname: Name of the sensor.
 * @filename: Configuration file of the calling sensor.
 * @argc: Argument count provided to the calling sensor.
 * @argv: Argument array provided to the calling sensor.
 *
 * Init the sensor library,
 * connect to the manager.
 *
 * Returns: 0 on success, -1 on error.
 */
int prelude_sensor_init(const char *sname, const char *filename, int argc, char **argv)
{
        int ret;

        memset(&node, 0, sizeof(node));
        memset(&address, 0, sizeof(address));
        INIT_LIST_HEAD(&node.address_list);
        
        if ( ! sname ) {
                errno = EINVAL;
                return -1;
        }

        prelude_set_program_name(sname);
       
        ret = prelude_client_ident_init(sname);
        if ( ret < 0 )
                return -1;
        
        ret = parse_argument(filename, argc, argv);
        if ( ret == prelude_option_end || ret == prelude_option_error )
                return ret;
        
        return ret;
}



/** 
 * prelude_sensor_send_msg_async:
 * @msg: Pointer on a message to send.
 *
 * Asynchronously send @msg to all Manager server we're connected to.
 * When this function return, @msg is invalid and shouldn't be used
 * anymore. prelude_async_init() should be called prior to using this function.
 */
void prelude_sensor_send_msg_async(prelude_msg_t *msg) 
{
        prelude_client_mgr_broadcast_async(manager_list, msg);
}



/**
 * prelude_sensor_send_msg:
 * @msg: Pointer on a message to send.
 *
 * Send @msg to all Manager server we're connected to.
 */
void prelude_sensor_send_msg(prelude_msg_t *msg) 
{
        prelude_client_mgr_broadcast(manager_list, msg);
}




/**
 * prelude_sensor_get_client_list:
 *
 * Returns: a list of Manager connection (prelude_client_t).
 */
struct list_head *prelude_sensor_get_client_list(void)
{
        return prelude_client_mgr_get_client_list(manager_list);
}



/**
 * prelude_sensor_notify_mgr_connection:
 * @cb: Callback function to call on notice.
 *
 * Tell the prelude library to call the @cb callback whenever the connection
 * state change.
 */
void prelude_sensor_notify_mgr_connection(void (*cb)(struct list_head *clist)) 
{
        prelude_client_mgr_notify_connection(manager_list, cb);
}




int prelude_heartbeat_send(void) 
{        
        if ( ! send_heartbeat_cb ) 
                return -1;
        
        /*
         * trigger the application callback.
         */
        send_heartbeat_cb(heartbeat_data);

        return 0;
}



static void heartbeat_timer_expire(void *null) 
{
        prelude_heartbeat_send();
        
        /*
         * reset our timer.
         */
        timer_reset(&heartbeat_timer);
}




/**
 * prelude_heartbeat_register_cb:
 * @cb: callback function for heartbeat sending.
 * @data: Pointer to data to be passed to the callback.
 *
 * prelude_heartbeat_register_cb() will make @cb called
 * each time the heartbeat timeout expire.
 */
void prelude_heartbeat_register_cb(void (*cb)(void *data), void *data) 
{
        heartbeat_data = data;
        send_heartbeat_cb = cb;

        if ( heartbeat_repeat_time == 0 )
                return;
        
        timer_set_callback(&heartbeat_timer, heartbeat_timer_expire);
        timer_set_expire(&heartbeat_timer, heartbeat_repeat_time);
        timer_init(&heartbeat_timer);

        send_heartbeat_cb(NULL);
}




int prelude_analyzer_fill_information(idmef_analyzer_t *analyzer) 
{
        int ret;
        char *ptr;
        static struct utsname uts;
        static idmef_process_t process;
        
        ret = uname(&uts);
        if ( ret < 0 ) {
                log(LOG_ERR, "uname returned an error.\n");
                return -1;
        }

        memset(&process, 0, sizeof(process));
        
        idmef_string_set(&analyzer->ostype, uts.sysname);
        idmef_string_set(&analyzer->osversion, uts.release);

        INIT_LIST_HEAD(&process.arg_list);
        INIT_LIST_HEAD(&process.env_list);
        
        process.pid = getpid();

        if ( process_name )
                idmef_string_set(&process.name, process_name);

        if ( process_path )
                idmef_string_set(&process.path, process_path);
        
        analyzer->node = &node;
        analyzer->process = &process;
        
        return 0;
}
