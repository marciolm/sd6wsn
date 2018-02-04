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
 *      Returns RPL routes
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
#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

uint16_t ipaddr_add(const uip_ipaddr_t *addr, char *buf);
uint16_t create_route_msg(char *buf, uip_ds6_route_t *r);
static void routes_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset);

RESOURCE(res_routes,
    "title=\"RPL route info\";rt=\"Text\"",
    routes_handler, //get
    routes_handler, //post
    NULL, //put
    NULL); //delete

uint16_t
ipaddr_add(const uip_ipaddr_t *addr, char *buf)
{
  uint16_t a, n;
  int i, f;
  n = 0;
  for (i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2)
    {
      a = (addr->u8[i] << 8) + addr->u8[i + 1];
      if (a == 0 && f >= 0)
        {
          if (f++ == 0)
            {
              n += sprintf(&buf[n], "::");
            }
        }
      else
        {
          if (f > 0)
            {
              f = -1;
            }
          else if (i > 0)
            {
              n += sprintf(&buf[n], ":");
            }
          n += sprintf(&buf[n], "%x", a);
        }
    }
  return n;
}

uint16_t
create_route_msg(char *buf, uip_ds6_route_t *r)
{
  uint8_t n = 0;
  n += sprintf(&(buf[n]), "{\"dest\":\"");
  n += ipaddr_add(&r->ipaddr, &(buf[n]));
  n += sprintf(&(buf[n]), "\",\"next\":\"");
  n += ipaddr_add(uip_ds6_route_nexthop(r), &(buf[n]));
  n += sprintf(&(buf[n]), "\"}");
  buf[n] = 0;
  PRINTF("buf_routes: %s\n", buf);
  return n;
}

void
routes_handler(void* request, void* response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset)
{
  int32_t strpos = 0;
  uip_ds6_route_t *r;
  volatile uint8_t i;

  size_t len = 0;
  uint8_t index;
  const char *pstr;
  uint8_t count;

  /* count the number of routes and return the total */
  count = uip_ds6_route_num_routes();
  if ((len = REST.get_query_variable(request, "index", &pstr)))
    {
      index = (uint8_t) atoi(pstr);
      if (index >= count)
        {
          strpos = snprintf((char *) buffer, preferred_size, "{}");
        }
      else
        {
          /* seek to the route entry and return it */
          i = 0;
          for (r = uip_ds6_route_head(); r != NULL;
              r = uip_ds6_route_next(r), i++)
            {
              if (i == index)
                {
                  break;
                }
            }
          strpos = create_route_msg((char *) buffer, r);

        }
      REST.set_header_content_type(response, APPLICATION_JSON);
    }
  else
    { /* index not provided */
      strpos += snprintf((char *) buffer, preferred_size, "{\"nr\":\"%d\"}", count);
		// PRINTF("buf_routes: %s\n", buffer);
      REST.set_header_content_type(response, APPLICATION_JSON);
    }
  *offset = -1;  // try to fix Copper response
  REST.set_response_payload(response, (char *) buffer, strpos);

}
