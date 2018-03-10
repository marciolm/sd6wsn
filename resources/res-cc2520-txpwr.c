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
 *      Controls CC2520 TX Power
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Marcio Miguel <marcio.miguel@gmail.com>
 */

#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "dev/cc2520/cc2520.h"
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"


static void
get_txpower_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset);
static void
put_txpower_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset);

RESOURCE(res_cc2520_txpower,
    "title=\"CC2520 TX Power\";rt=\"Text\"",
    get_txpower_handler, //get
    NULL,//post
    put_txpower_handler,//put
    NULL); //delete

static void
get_txpower_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset)
{
   REST.set_response_payload(response, buffer,
      snprintf((char *) buffer, preferred_size, "%d", cc2520_get_txpower()));
}
put_txpower_handler(void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset)
{
  uint8_t index;
  const char *new_txpower;

  REST.get_query_variable(request, "index", &new_txpower);
  index = (uint8_t) atoi(new_txpower);
  PRINTF("TXpower: %d\n", index);

  cc2520_set_txpower(index);
  REST.set_response_status(response, REST.status.CHANGED);
}

/* TX Power table values
        valeurs de TXPOWER
          0x03 -> -18 dBm 03
          0x2C -> -7 dBm 44
          0x88 -> -4 dBm 136
          0x81 -> -2 dBm 129
          0x32 -> 0 dBm 50
          0x13 -> 1 dBm 19
          0xAB -> 2 dBm 171
          0xF2 -> 3 dBm 242
          0xF7 -> 5 dBm 247

*/

