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
 *      Controls CC2538 TX Power
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Marcio Miguel <marcio.miguel@gmail.com>
 */

#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "net/netstack.h"
#include "dev/radio.h"
/*---------------------------------------------------------------------------*/

static radio_value_t value;

/*---------------------------------------------------------------------------*/
static radio_result_t
get_param(radio_param_t param, radio_value_t *value)
{
  radio_result_t rv;

  rv = NETSTACK_RADIO.get_value(param, value);

  switch(rv) {
  case RADIO_RESULT_ERROR:
    printf("Radio returned an error\n");
    break;
  case RADIO_RESULT_INVALID_VALUE:
    printf("Value %d is invalid\n", *value);
    break;
  case RADIO_RESULT_NOT_SUPPORTED:
    printf("Param %u not supported\n", param);
    break;
  case RADIO_RESULT_OK:
    break;
  default:
    printf("Unknown return value\n");
    break;
  }

  return rv;
}
/*---------------------------------------------------------------------------*/
static radio_result_t
set_param(radio_param_t param, radio_value_t value)
{
  radio_result_t rv;

  rv = NETSTACK_RADIO.set_value(param, value);

  switch(rv) {
  case RADIO_RESULT_ERROR:
    printf("Radio returned an error\n");
    break;
  case RADIO_RESULT_INVALID_VALUE:
    printf("Value %d is invalid\n", value);
    break;
  case RADIO_RESULT_NOT_SUPPORTED:
    printf("Param %u not supported\n", param);
    break;
  case RADIO_RESULT_OK:
    break;
  default:
    printf("Unknown return value\n");
    break;
  }

  return rv;
}
/*---------------------------------------------------------------------------*/


static void
get_cc2538_txpower_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset);
static void
set_cc2538_txpower_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset);

RESOURCE(res_cc2538_txpower,
    "title=\"CC2538 TX Power\";rt=\"Text\"",
    get_cc2538_txpower_handler, //get
    NULL,//post
    set_cc2538_txpower_handler,//put
    NULL); //delete

static void
get_cc2538_txpower_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset)
{
   get_param(RADIO_PARAM_TXPOWER, &value);
   REST.set_response_payload(response, buffer,
      snprintf((char *) buffer, preferred_size, "%u", value));
}
set_cc2538_txpower_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset)
{
  uint8_t index;
  const char *new_txpower;

  REST.get_query_variable(request, "index", &new_txpower);
  index = (uint8_t) atoi(new_txpower);

  //set_tx_power(index);
  set_param(RADIO_PARAM_TXPOWER, index);
  REST.set_response_status(response, REST.status.CHANGED);
}

/* TX Power table values
  {  7, 0xFF },
  {  5, 0xED },
  {  3, 0xD5 },
  {  1, 0xC5 },
  {  0, 0xB6 },
  { -1, 0xB0 },
  { -3, 0xA1 },
  { -5, 0x91 },
  { -7, 0x88 },
  { -9, 0x72 },
  {-11, 0x62 },
  {-13, 0x58 },
  {-15, 0x42 },
  {-24, 0x00 },

*/

