#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint16_t

extern "C" {
#include <pru_rpmsg.h>  // struct pru_rpmsg_transport
}


void rpmsg_handler_init();
void rpmsg_handler_register_on_rx_callback(void (*func)(char *payload, size_t length));
bool rpmsg_handler_is_connected();
void rpmsg_handler_set_disconnected();
void rpmsg_handler_poll();
int16_t rpmsg_handler_send(char const *payload, size_t length);
