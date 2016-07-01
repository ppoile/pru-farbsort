/*
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/ 
 *  
 *  
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * 	* Redistributions of source code must retain the above copyright 
 * 	  notice, this list of conditions and the following disclaimer.
 * 
 * 	* Redistributions in binary form must reproduce the above copyright
 * 	  notice, this list of conditions and the following disclaimer in the 
 * 	  documentation and/or other materials provided with the   
 * 	  distribution.
 * 
 * 	* Neither the name of Texas Instruments Incorporated nor the names of
 * 	  its contributors may be used to endorse or promote products derived
 * 	  from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <pru_cfg.h>

#include "resource_table_0.h"

static char hex(uint8_t value)
{
  if (value > 0xf) {
    return '?';
  }
  if (value >= 0xa) {
    return value - 0xa + 'a';
  }
  return value + '0';
}

static void sleepMs(uint32_t value)
{
	uint32_t i;
	for (i = 0; i < value; i++) {
		__delay_cycles(200000);
	}
}

int main(void)
{
	const uint32_t traceOffset = am335x_pru_remoteproc_ResourceTable.trace.da - am335x_pru_remoteproc_ResourceTable.memory.da;
	const uint32_t traceAddr = am335x_pru_remoteproc_ResourceTable.memory.pa + traceOffset;

	volatile char *trace = (char*)traceAddr;
	const uint32_t traceSize = am335x_pru_remoteproc_ResourceTable.trace.len;

	uint8_t num = 0;
	uint32_t ti = 0;

	while(1){
		char buffer[20] = "hello nr. ";
		int end = strlen(buffer);
		buffer[end] = hex(num >> 4);
		buffer[end+1] = hex(num & 0xf);
		buffer[end+2] = '\n';
		int len = end+3;

		int i;
		for (i = 0; i < len; i++) {
			trace[ti] = buffer[i];
			ti = (ti + 1) % traceSize;
		}

		num++;

		sleepMs(1000);
	}
}
