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
 *      Returns registered nodes
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Modified by Marcio Miguel <marcio.miguel@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/rpl/rpl.h"
#include "net/rpl/rpl-private.h"
#include "rest-engine.h"
#include "er-coap-engine.h"
/* debug */
#define DEBUG DEBUG_FULL
#include "net/ip/uip-debug.h"

uint16_t ipaddr_last_chunk(const uip_ipaddr_t *addr, char *buffer);
static void node_mod_handler(void *request, void *response, char *buffer,
		uint16_t preferred_size, int32_t *offset);
static void res_periodic_node_mod_handler(void);

PERIODIC_RESOURCE(res_node_mod, "title=\"Nodes List\";rt=\"Text\";obs",
		node_mod_handler, //get
		NULL,//post
		NULL,//put
		NULL,//delete
		120 * CLOCK_SECOND,
		res_periodic_node_mod_handler);

uint16_t ipaddr_last_chunk(const uip_ipaddr_t *addr, char *buffer) {
	uint16_t a, n;
	n = 0;
	a = (addr->u8[14] << 8) + addr->u8[15]; //only the end of address
	n += sprintf(&buffer[n], "%x", a); // %d for decimal, %x for hex
	return n;
}

void node_mod_handler(void* request, void* response, char *buffer,
		uint16_t preferred_size, int32_t *offset) {

	uip_ds6_route_t *r;
	volatile uint8_t i;
	uint16_t n = 0;
	uint8_t count;

	/* count the number of routes and return the total */
	count = uip_ds6_route_num_routes();
	/* seek to the route entry and return it */
	i = 1;
	n += sprintf(&(buffer[n]), "{\"nodes\":\"");
	if(count > 0) {
		for (r = uip_ds6_route_head(); r != NULL;
				r = uip_ds6_route_next(r), i++) {
			n += ipaddr_last_chunk(&r->ipaddr, &(buffer[n]));
			n += sprintf(&(buffer[n]), ",");
		}
		n += sprintf(&(buffer[n - 1]), "\"}"); // replace last comma
	} else {
		n += sprintf(&(buffer[n]), "\"}");
	}
	PRINTF("buf_nodes: %s\n", buffer);
	REST.set_header_content_type(response, APPLICATION_JSON);
	REST.set_header_max_age(response, res_node_mod.periodic->period / CLOCK_SECOND);
	//*offset = -1;  // try to fix Copper response
	REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "%s", buffer));
}

/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
static void
res_periodic_node_mod_handler()
{
	/* Do a periodic task here, e.g., sampling a sensor. */

	/* Usually a condition is defined under with subscribers are notified, e.g., large enough delta in sensor reading. */
	if(1) {
		/* Notify the registered observers which will trigger the res_get_handler to create the response. */
		REST.notify_subscribers(&res_node_mod);
	}
}

