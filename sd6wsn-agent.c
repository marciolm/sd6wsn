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
 *      Erbium (Er) REST Engine example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-icmp6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/rpl/rpl.h"
#include "sys/clock.h"

#define DEBUG DEBUG_FULL
#include "net/ip/uip-debug.h"

#ifndef PING_PROBE
#define PING_PROBE 0
#endif
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern resource_t res_hello, res_separate, res_push, res_event;
#ifdef CPU_HAS_MSP430X
extern resource_t res_cc2520_txpower;
#else
extern resource_t res_cc2538_txpower;
#endif
extern resource_t res_cc2538_radio;
extern resource_t res_rssi;
extern resource_t res_etx;
extern resource_t res_routes;
extern resource_t res_flow_mod;

#if PLATFORM_HAS_BUTTON
#include "dev/button-sensor.h"
extern resource_t res_event;
#endif

#if PLATFORM_HAS_LEDS
extern resource_t res_leds, res_toggle;
#endif

#if PLATFORM_HAS_BATTERY
#include "dev/battery-sensor.h"
extern resource_t res_battery;
#endif

#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"
extern resource_t res_radio;
#endif

PROCESS(er_example_server, "Erbium Example Server");
PROCESS(ping_sender_process, "Ping sender process");
PROCESS(udp_client_process, "UDP sender process");
#if PING_PROBE
AUTOSTART_PROCESSES(&er_example_server, &ping_sender_process, &udp_client_process);
#else
AUTOSTART_PROCESSES(&er_example_server, &udp_client_process);
#endif

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define UDP_EXAMPLE_ID  190

#ifndef PERIOD
#define PERIOD 30 //10 // 60
#endif

#define START_INTERVAL		(180 * CLOCK_SECOND)  //delay before start the test
#define SEND_INTERVAL		(PERIOD * CLOCK_SECOND)
#define SEND_TIME		(random_rand() % (SEND_INTERVAL))
#define MAX_PAYLOAD_LEN		30

static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;

static int seq_id;
static int reply;

static void
tcpip_handler(void)
{
	char *str;

	if(uip_newdata()) {
		str = uip_appdata;
		str[uip_datalen()] = '\0';
		reply++;
		printf("DATA recv '%s' (s:%d, r:%d)\n", str, seq_id, reply);
	}
}
/*---------------------------------------------------------------------------*/
static void
send_packet(void *ptr)
{
	char buf[MAX_PAYLOAD_LEN];

#ifdef SERVER_REPLY
	uint8_t num_used = 0;
	uip_ds6_nbr_t *nbr;

	nbr = nbr_table_head(ds6_neighbors);
	while(nbr != NULL) {
		nbr = nbr_table_next(ds6_neighbors, nbr);
		num_used++;
	}

	if(seq_id > 0) {
		ANNOTATE("#A r=%d/%d,color=%s,n=%d %d\n", reply, seq_id,
				reply == seq_id ? "GREEN" : "RED", uip_ds6_route_num_routes(), num_used);
	}
#endif /* SERVER_REPLY */

	seq_id++;
	PRINTF("DATA send to %d 'Hello %d'\n",
			server_ipaddr.u8[sizeof(server_ipaddr.u8) - 1], seq_id);
	sprintf(buf, "Hello %d from client", seq_id);
	uip_udp_packet_sendto(client_conn, buf, strlen(buf),
			&server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
}

/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
	int i;
	uint8_t state;

	PRINTF("Client IPv6 addresses: ");
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused &&
				(state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
			PRINTF("\n");
			/* hack to make address "final" */
			if (state == ADDR_TENTATIVE) {
				uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
			}
		}
	}
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
	static struct etimer periodic;
	static struct etimer periodic_init;
	static struct ctimer backoff_timer;
#if WITH_COMPOWER
	static int print = 0;
#endif

	PROCESS_BEGIN();

	PROCESS_PAUSE();

	/* Set the UDP server address */
	uip_ip6addr(&server_ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0x200, 0, 0, 1);

	PRINTF("UDP client process started nbr:%d routes:%d\n",
			NBR_TABLE_CONF_MAX_NEIGHBORS, UIP_CONF_MAX_ROUTES);

	print_local_addresses();

	/* new connection with remote host */
	client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL);
	if(client_conn == NULL) {
		PRINTF("No UDP connection available, exiting the process!\n");
		PROCESS_EXIT();
	}
	udp_bind(client_conn, UIP_HTONS(UDP_CLIENT_PORT));

	PRINTF("Created a connection with the server ");
	PRINT6ADDR(&client_conn->ripaddr);
	PRINTF(" local/remote port %u/%u\n",
			UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));

#if WITH_COMPOWER
	powertrace_sniff(POWERTRACE_ON);
#endif
	etimer_set(&periodic_init, START_INTERVAL);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_init));
	etimer_reset(&periodic_init);
	etimer_set(&periodic, SEND_INTERVAL);
	while(1) {
		PROCESS_YIELD();
		if(ev == tcpip_event) {
			tcpip_handler();
		}

		if(etimer_expired(&periodic)) {
			etimer_reset(&periodic);
			ctimer_set(&backoff_timer, SEND_TIME, send_packet, NULL);

#if WITH_COMPOWER
			if (print == 0) {
				powertrace_print("#P");
			}
			if (++print == 3) {
				print = 0;
			}
#endif

		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(er_example_server, ev, data)
{
	PROCESS_BEGIN() ;

	PROCESS_PAUSE();

	PRINTF("Starting Erbium Example Server\n");

#ifdef RF_CHANNEL
	PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
	PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

	PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
	PRINTF("LL header: %u\n", UIP_LLH_LEN);
	PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
	PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

	/* Initialize the REST engine. */
	rest_init_engine();
	/*
	 * Bind the resources to their Uri-Path.
	 * WARNING: Activating twice only means alternate path, not two instances!
	 * All static variables are the same for each URI path.
	 */
	rest_activate_resource(&res_hello, "test/hello");
	rest_activate_resource(&res_push, "test/push");
	rest_activate_resource(&res_rssi, "sd6wsn/info-get/rssi");
	rest_activate_resource(&res_etx, "sd6wsn/info-get/nbr-etx");
	rest_activate_resource(&res_flow_mod, "sd6wsn/flow-mod");
	rest_activate_resource(&res_routes, "sd6wsn/info-get/routes");
#ifdef CPU_HAS_MSP430X
	rest_activate_resource(&res_cc2520_txpower, "sd6wsn/info-get/txpower");
#else
	rest_activate_resource(&res_cc2538_txpower, "sd6wsn/info-get/txpower");
#endif
	rest_activate_resource(&res_cc2538_radio, "sd6wsn/info-get/radio");

#if PLATFORM_HAS_LEDS
	rest_activate_resource(&res_toggle, "actuators/toggle");
#endif
#if PLATFORM_HAS_BATTERY
	rest_activate_resource(&res_battery, "sd6wsn/info-get/battery");
	SENSORS_ACTIVATE(battery_sensor);
#endif

	/* Define application-specific events here. */
	while (1)
	{
		PROCESS_WAIT_EVENT();
#if PLATFORM_HAS_BUTTON
		if (ev == sensors_event && data == &button_sensor)
		{
			PRINTF("*******BUTTON*******\n");

			/* Call the event_handler for this application-specific event. */
			res_event.trigger();

			/* Also call the separate response example handler. */
			res_separate.resume();
		}
#endif /* PLATFORM_HAS_BUTTON */
	} /* while (1) */

	PROCESS_END();
}
PROCESS_THREAD(ping_sender_process, ev, data)
{
#define UIP_IP_BUF                ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define ECHO_REQ_PAYLOAD_LEN   20
#define SEND_INTERVAL_PING           (120 * CLOCK_SECOND)  // Time before start pings
#define SEND_TIME_PING               (random_rand() % (SEND_INTERVAL_PING)) //rand to avoid net synchronization
#define PINGS_DELAY (CLOCK_SECOND / 2)   // time between neighbor pings
	static struct etimer periodic_timer;
	static struct etimer send_timer;
	static struct etimer ping_timer;
	rpl_dag_t *dag;
	static rpl_parent_t *parent;
	dag = rpl_get_any_dag();

	PROCESS_BEGIN() ;

	etimer_set(&periodic_timer, SEND_INTERVAL_PING);
	etimer_set(&ping_timer, PINGS_DELAY);
	while (1)
	{

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
		etimer_reset(&periodic_timer);
		etimer_set(&send_timer, SEND_TIME_PING);

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
		if (dag != NULL)
		{
			parent = nbr_table_head(rpl_parents);
			while (parent != NULL)
			{
				etimer_set(&ping_timer, PINGS_DELAY);
				PROCESS_WAIT_EVENT_UNTIL (etimer_expired(&ping_timer));
				uip_icmp6_send(rpl_get_parent_ipaddr(parent),
						ICMP6_ECHO_REQUEST, 0, ECHO_REQ_PAYLOAD_LEN);
				PRINTF("Sending Ping To: ");
				PRINT6ADDR(&UIP_IP_BUF->destipaddr);
				PRINTF("\n");
				parent = nbr_table_next(rpl_parents, parent);
			}
		}
		else
		{
			PRINTF("DAG not found\n");
		}
	}

	PROCESS_END();
}
