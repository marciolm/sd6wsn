/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Example resource
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <string.h>
#include "rest-engine.h"
#include "er-coap.h"
#include "dev/cc2520/cc2520.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-icmp6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/rpl/rpl.h"
#include "sys/timer.h"
#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"
#define ECHO_REQ_PAYLOAD_LEN   20

static struct uip_icmp6_echo_reply_notification echo_reply_notification;
static void
res_get_handler(void *request, void *response, uint8_t *buffer,
		uint16_t preferred_size, int32_t *offset);
static void
res_periodic_handler(void);
static void
echo_reply_handler(uip_ipaddr_t *source, uint8_t ttl, uint8_t *data,
		uint16_t datalen);

PERIODIC_RESOURCE(res_rssi,
		"title=\"RSSI\";obs",
		res_get_handler,
		NULL,
		NULL,
		NULL,
		5 * CLOCK_SECOND,
		res_periodic_handler);

/*
 * Use local resource state that is accessed by res_get_handler() and altered by res_periodic_handler() or PUT or POST.
 */
static uint8_t event_counter;
int def_rt_rssi = 0;
int rssi_parent[256];

void
res_get_handler(void *request, void *response, uint8_t *buffer,
		uint16_t preferred_size, int32_t *offset)
{
	char buf[1024];
	int32_t strpos = 0;
	rpl_dag_t *dag;
	static rpl_parent_t *parent;
	int last_octect_end_parent;
	static struct timer ping_timer;
	timer_set(&ping_timer, CLOCK_SECOND / 2);
	dag = rpl_get_any_dag();
	uip_icmp6_echo_reply_callback_add(&echo_reply_notification,
			echo_reply_handler); // Register ICMPv6 Echo Replay Callback
	if (dag != NULL)
	{
		parent = nbr_table_head(rpl_parents);
		strpos += sprintf(&(buf[strpos]), "{\"neighbors\":[");
		while (parent != NULL)
		{
			if (timer_expired(&ping_timer))
			{
				uip_icmp6_send(rpl_get_parent_ipaddr(parent), ICMP6_ECHO_REQUEST,
						0, ECHO_REQ_PAYLOAD_LEN);
				PRINTF("Sending Ping\n");

				last_octect_end_parent = rpl_get_parent_ipaddr(parent)->u8[15];
				if (parent == dag->preferred_parent)
				{
					strpos += sprintf(&(buf[strpos]),
							"{\"nb\":\"%x\",\"rssi\":%d,\"prf\":1},",
							last_octect_end_parent,
							rssi_parent[last_octect_end_parent]);
				}
				else
				{
					strpos += sprintf(&(buf[strpos]),
							"{\"nb\":\"%x\",\"rssi\":%d,\"prf\":0},",
							last_octect_end_parent,
							rssi_parent[last_octect_end_parent]);

				}
				parent = nbr_table_next(rpl_parents, parent);
				timer_restart(&ping_timer);
			}
		}
	}
	else
	{ /* no DAG */
		event_counter = 0;
	} PRINTF("strpos: %ld\n", strpos);
	//rpl_print_neighbor_list(); // get parents for debug purposes
	PRINTF("buf_parents: %s\n", buf);
	strpos += sprintf(&(buf[strpos - 1]), "]}\n"); //replace the last comma
	PRINTF("strpos-after: %ld\n", strpos);
	REST.set_header_content_type(response, APPLICATION_JSON);
	REST.set_header_max_age(response, res_rssi.periodic->period / CLOCK_SECOND);
	REST.set_response_payload(response, buffer,
			snprintf((char *) buffer, preferred_size, "%s", buf));
}

void
res_periodic_handler()
{
	/* Usually a condition is defined under with subscribers are notified, e.g., large enough delta in sensor reading. */
	if (1)
	{
		/* Notify the registered observers which will trigger the res_get_handler to create the response. */
		REST.notify_subscribers(&res_rssi);
	}
}

/*---------------------------------------------------------------------------*/

static void
echo_reply_handler(uip_ipaddr_t *source, uint8_t ttl, uint8_t *data,
		uint16_t datalen)
{
	PRINTF("Handler Called\n");
	PRINTF("Source Address: %u\n", source->u8[15]);
	rssi_parent[source->u8[15]] = sicslowpan_get_last_rssi();
}

