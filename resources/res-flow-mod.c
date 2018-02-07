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
 *      res-flow-mod
 * \author
 *      Marcio Miguel <marcio.miguel@gmail.com> based on examples written
 *      by Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"
#include "res-flow-mod.h"

flow_s flow_table[32];
static uint8_t table_entry = 0;
uip_ipaddr_t tmp_addr;
static int table_pos;
uip_ipaddr_t ipaddr;
static int table_index;

uip_ipaddr_t * get_next_hop_by_flow(uip_ipaddr_t *srcaddress,uip_ipaddr_t *dstaddress,uint16_t *srcport,uint16_t *dstport,uint8_t *proto){

	table_pos = 0;

	//PRINTF("\nget_next_hop_by_flow srcaddress:");
	//PRINT6ADDR(srcaddress);
	//PRINTF("\nget_next_hop_by_flow dstaddress:");
	//PRINT6ADDR(dstaddress);
	//PRINTF("\n");
	if(dstport == 5683 && srcport == 5683 ) {
		return NULL;
	}
	PRINTF("\nnumber of table_entries: %d\n",table_entry);
	while(table_pos<=table_entry){
		if(uip_ipaddr_cmp(dstaddress,&flow_table[table_pos].ipv6dst)) {
			if(uip_ipaddr_cmp(srcaddress,&flow_table[table_pos].ipv6src)){
				if(srcport == flow_table[table_pos].srcport || flow_table[table_pos].srcport == NULL ){
					if(dstport == flow_table[table_pos].dstport || flow_table[table_pos].dstport == NULL){
						if(proto == flow_table[table_pos].ipproto || flow_table[table_pos].ipproto == NULL){
							PRINTF("flow found !\n");
							break;
						}
					}
				}
			}
		}
		table_pos++;
	}

	PRINTF("table_pos: %d\n",table_pos);
	PRINTF("\nget_next_hop_by_flow ipv6dst:");
	PRINT6ADDR(&flow_table[table_pos].ipv6dst);
	PRINTF("\n");
	if(table_pos>table_entry) {
		PRINTF("flow not found\n");
		return NULL;
	}else {
		return &flow_table[table_pos].nhipaddr;
	}
}

static void
flow_mod_handler(void *request, void *response, char *buffer,
		uint16_t preferred_size, int32_t *offset);

RESOURCE(res_flow_mod, "title=\"Flow-mod\";rt=\"Text\"", NULL, //get
		NULL,//post
		flow_mod_handler,//put
		NULL);//delete

static void
flow_mod_handler(void *request, void *response, char *buffer,
		uint16_t preferred_size, int32_t *offset) {

	const char *str = NULL;
	uint8_t len = 0;
	uint8_t flowid_temp;
	uint8_t existing_flow = 0;

	table_index = 0;

	len = REST.get_query(request, &str);
	snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
	PRINTF("len %d\n", len); PRINTF("Query-all: %s\n", buffer);

	len = REST.get_query_variable(request, "action", &str);
	snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
	PRINTF("action: %s\n", buffer);

	if ((len = REST.get_query_variable(request, "flowid", &str))) {
		snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
		flowid_temp=atoi(buffer);
		while(table_index<=table_entry){
			if(flowid_temp == flow_table[table_index].flowid ) {
				PRINTF("flowid entry found!\n");
				existing_flow = 1;
				break;
			}
			table_index++;
		}
		if(!existing_flow) {  //if is a new flow, use the next empty table entry
			table_index = table_entry;
			table_entry++;
		}
		flow_table[table_index].flowid=flowid_temp;
	}
	if ((len = REST.get_query_variable(request, "ipv6src", &str))) {
		snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
		uiplib_ip6addrconv(buffer, &tmp_addr);
		flow_table[table_index].ipv6src=tmp_addr;
	}
	if ((len = REST.get_query_variable(request, "ipv6dst", &str))) {
		snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
		uiplib_ip6addrconv(buffer, &tmp_addr);
		flow_table[table_index].ipv6dst=tmp_addr;
	}
	if ((len = REST.get_query_variable(request, "srcport", &str))) {
		snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
		flow_table[table_index].srcport=atoi(buffer);
	}
	if ((len = REST.get_query_variable(request, "dstport", &str))) {
		snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
		flow_table[table_index].dstport=atoi(buffer);
	}
	if ((len = REST.get_query_variable(request, "ipproto", &str))) {
		snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
		flow_table[table_index].ipproto=atoi(buffer);
	}
	if ((len = REST.get_query_variable(request, "nhipaddr", &str))) {
		snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
		uiplib_ip6addrconv(buffer, &tmp_addr);
		flow_table[table_index].nhipaddr=tmp_addr;
	}
	if ((len = REST.get_query_variable(request, "txpwr", &str))) {
		snprintf((char *) buffer, REST_MAX_CHUNK_SIZE - 1, "%.*s", len, str);
		flow_table[table_index].txpwr=atoi(buffer);
	}
	PRINTF("flowid: %d\n", flow_table[table_index].flowid);
	PRINTF("ipv6src: ");
	PRINT6ADDR(&flow_table[table_index].ipv6src);
	PRINTF("\n");
	PRINTF("ipv6dst: ");
	PRINT6ADDR(&flow_table[table_index].ipv6dst);
	PRINTF("\n");
	PRINTF("nhipaddr: ");
	PRINT6ADDR(&flow_table[table_index].nhipaddr);
	PRINTF("\n");
	PRINTF("txpwr: %d\n", flow_table[table_index].txpwr);
	PRINTF("table entries=%d\n",table_entry);
	// REST.set_response_status(response, REST.status.CHANGED);
}
