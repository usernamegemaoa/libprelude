/*****
*
* Copyright (C) 2001, 2002 Yoann Vandoorselaere <yoann@prelude-ids.org>
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

#ifndef _LIBPRELUDE_PRELUDE_MESSAGE_BUFFERED_H
#define _LIBPRELUDE_PRELUDE_MESSAGE_BUFFERED_H

typedef struct prelude_message_buffered prelude_msgbuf_t;


#include "prelude-client.h"
#include "prelude-msg.h"


prelude_client_t *prelude_msgbuf_get_client(prelude_msgbuf_t *msgbuf);

prelude_msgbuf_t *prelude_msgbuf_new(prelude_client_t *client);

void prelude_msgbuf_close(prelude_msgbuf_t *msgbuf);

void prelude_msgbuf_mark_end(prelude_msgbuf_t *msgbuf);

void prelude_msgbuf_set(prelude_msgbuf_t *msgbuf, uint8_t tag, uint32_t len, const void *data);

prelude_msg_t *prelude_msgbuf_get_msg(prelude_msgbuf_t *msgbuf);

void prelude_msgbuf_set_callback(prelude_msgbuf_t *msgbuf, prelude_msg_t *(*send_msg)(prelude_msgbuf_t *msgbuf));

void prelude_msgbuf_set_data(prelude_msgbuf_t *msgbuf, void *data);

void *prelude_msgbuf_get_data(prelude_msgbuf_t *msgbuf);

#endif /* _LIBPRELUDE_PRELUDE_MESSAGE_BUFFERED_H */