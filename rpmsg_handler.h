#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint16_t

extern "C" {
#include <pru_rpmsg.h>  // struct pru_rpmsg_transport
}


class rpmsg_handler
{
public:
  explicit rpmsg_handler();
  void init();
  void register_on_rx_callback(void (*func)(char *payload, size_t length));
  bool is_connected();
  void set_disconnected();
  void poll();
  int16_t send(char const *payload, size_t length);

private:
  struct pru_rpmsg_transport _transport;
  uint16_t _src;
  uint16_t _dst;
  uint8_t _payload[RPMSG_BUF_SIZE+1];
  uint16_t _len;
  bool _connected;
  void (*_on_rx_callback_func)(char *payload, size_t length);
};
