/*
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
 *
 */


/**
 * \file
 *      res-etx
 * \author
 *      Marcio Miguel <marcio.miguel@gmail.com> based on examples written
 *      by Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <string.h>
#include "rest-engine.h"
#include "er-coap.h"
#include "net/rpl/rpl.h"
#include "net/ipv6/uip-ds6.h"
/* debug */
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

typedef struct etx_s {
	uint8_t nbr_addr;
	uint16_t nbr_etx;
	rpl_parent_t * p;
}etx_s ;

etx_s etx_table[NBR_TABLE_CONF_MAX_NEIGHBORS]; // number of neighbors configured
uint8_t parent_index;

static void res_get_handler(void *request, void *response, uint8_t *buffer,
		uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);

PERIODIC_RESOURCE(res_etx,
		"title=\"etx\";obs",
		res_get_handler,
		NULL,
		NULL,
		NULL,
		60 * CLOCK_SECOND,
		res_periodic_handler);

static void
res_get_handler(void *request, void *response, uint8_t *buffer,
		uint16_t preferred_size, int32_t *offset)
{
	rpl_dag_t *dag;
	rpl_parent_t *parent;
	int32_t strpos = 0;
	const uip_ipaddr_t *addr;
	addr = &uip_ds6_if.addr_list[1].ipaddr;
	dag = rpl_get_any_dag();

	parent_index = 0;

	if (dag != NULL)
	{
		/* seek to the parents entry and return it */
		strpos += sprintf(&(buffer[strpos]),"{\"node\":\"n%x\"",addr->u8[15]); // last addr byte of mote
		strpos += sprintf(&(buffer[strpos]),",\"nbr\":{");
		parent = nbr_table_head(rpl_parents);  // addr of first neighbor
		while (parent != NULL)
		{
			etx_table[parent_index].nbr_addr = rpl_get_parent_ipaddr(parent)->u8[15];
			etx_table[parent_index].nbr_etx = rpl_get_parent_link_metric(parent);
			etx_table[parent_index].p = parent;
			strpos += sprintf(&(buffer[strpos]),"\"n%x\":%u,",etx_table[parent_index].nbr_addr,
					etx_table[parent_index].nbr_etx);
			parent = nbr_table_next(rpl_parents, parent);
			parent_index++;
		}
		PRINTF("parent_index:%d\n",parent_index);
	}
	else
	{ /* no DAG */
		strpos += sprintf(&(buffer[strpos]),"{}\n");
	}
	//PRINTF("strpos: %ld\n", strpos);
	//rpl_print_neighbor_list(); // get parents for debug purposes
	//PRINTF("buf_parents: %s\n", buffer);
	strpos += sprintf(&(buffer[strpos-1]),"}}\n"); //replace the last comma
	//PRINTF("strpos-after: %ld\n", strpos);
	REST.set_header_content_type(response, APPLICATION_JSON);
	REST.set_header_max_age(response, res_etx.periodic->period / CLOCK_SECOND);
	//*offset = -1;  // try to fix Copper response
	REST.set_response_payload(response, buffer, snprintf((char *)buffer,
			preferred_size, "%s", buffer));

	/* The REST.subscription_handler() will be called for observable resources by the REST framework. */
}
/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
static void
res_periodic_handler()
{
	uint8_t parent_counter = 0;
	uint16_t etx_temp;
	uint8_t etx_changed = 0;

	while(parent_counter < parent_index) {
		etx_temp = rpl_get_parent_link_metric(etx_table[parent_counter].p);
		PRINTF("parent: %d ",etx_table[parent_counter].nbr_addr);
		PRINTF("etx_temp:%d\n",etx_temp);

		if(etx_temp > etx_table[parent_counter].nbr_etx * 2
				|| etx_temp < etx_table[parent_counter].nbr_etx / 2 ) {
					etx_table[parent_counter].nbr_etx = etx_temp ;
					etx_changed = 1;
		}
		parent_counter++;
	}
	/* Notify the registered observers which will trigger the res_get_handler to create the response. */
	if (etx_changed) {
		PRINTF("etx_changed !\n");
		REST.notify_subscribers(&res_etx);
	}
}
