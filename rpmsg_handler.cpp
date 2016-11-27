#include "rpmsg_handler.h"

extern "C" {
#include <pru_cfg.h>
#include <pru_intc.h>
#include <sys_mailbox.h>
#include "resource_table_0.h"
}


namespace {
/* PRU0 is mailbox module user 1 */
const uint32_t MB_USER = 1;
/* Mbox0 - mail_u1_irq (mailbox interrupt for PRU0) is Int Number 60 */
const uint32_t MB_INT_NUMBER = 60;
/* The mailboxes used for RPMsg are defined in the Linux device tree
 * PRU0 uses mailboxes 2 (From ARM) and 3 (To ARM)
 * PRU1 uses mailboxes 4 (From ARM) and 5 (To ARM)
 */
const uint32_t MB_TO_ARM_HOST = 3;
const uint32_t MB_FROM_ARM_HOST = 2;
/*
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
const char CHAN_NAME[] = "rpmsg-pru";
const char CHAN_DESC[] = "Channel 30";
const uint32_t CHAN_PORT = 30;
/* 
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
const uint32_t VIRTIO_CONFIG_S_DRIVER_OK = 4;
/* Host-0 Interrupt sets bit 30 in register R31 */
const uint32_t HOST_INT = 0x40000000;

struct pru_rpmsg_transport _transport;
uint16_t _src;
uint16_t _dst;
uint8_t _payload[RPMSG_BUF_SIZE+1];
uint16_t _len;
bool _connected;
void (*_on_rx_callback_func)(char *payload, size_t length);
}


volatile register uint32_t __R31;


void rpmsg_handler_init()
{
  _connected = false;
  _on_rx_callback_func = 0;

  /* allow OCP master port access by the PRU so the PRU can read external memories */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* clear the status of event MB_INT_NUMBER (the mailbox event) and enable the mailbox event */
  CT_INTC.SICR_bit.STS_CLR_IDX = MB_INT_NUMBER;
  CT_MBX.IRQ[MB_USER].ENABLE_SET |= 1 << (MB_FROM_ARM_HOST * 2);

  /* Make sure the Linux drivers are ready for RPMsg communication */
  volatile uint8_t *status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

  /* Initialize pru_virtqueue corresponding to vring0 (PRU to ARM Host direction) */
  pru_virtqueue_init(&_transport.virtqueue0, &resourceTable.rpmsg_vring0, &CT_MBX.MESSAGE[MB_TO_ARM_HOST], &CT_MBX.MESSAGE[MB_FROM_ARM_HOST]);

  /* Initialize pru_virtqueue corresponding to vring1 (ARM Host to PRU direction) */
  pru_virtqueue_init(&_transport.virtqueue1, &resourceTable.rpmsg_vring1, &CT_MBX.MESSAGE[MB_TO_ARM_HOST], &CT_MBX.MESSAGE[MB_FROM_ARM_HOST]);

  /* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &_transport, (char*)CHAN_NAME, (char*)CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
}

void rpmsg_handler_register_on_rx_callback(void (*func)(char *payload, size_t length))
{
  _on_rx_callback_func = func;
}

bool rpmsg_handler_is_connected()
{
  return _connected;
}

void rpmsg_handler_set_disconnected()
{
  _connected = false;
}

void rpmsg_handler_poll()
{
  if (!_on_rx_callback_func) {
    return;
  }

  /* Check bit 30 of register R31 to see if the mailbox interrupt has occurred */
  if (__R31 & HOST_INT)
  {
    /* Clear the mailbox interrupt */
    CT_MBX.IRQ[MB_USER].STATUS_CLR |= 1 << (MB_FROM_ARM_HOST * 2);

    /* Clear the event status, event MB_INT_NUMBER corresponds to the mailbox interrupt */
    CT_INTC.SICR_bit.STS_CLR_IDX = MB_INT_NUMBER;

    /* Use a while loop to read all of the current messages in the mailbox */
    while (CT_MBX.MSGSTATUS_bit[MB_FROM_ARM_HOST].NBOFMSG > 0)
    {
      /* Check to see if the message corresponds to a receive event for the PRU */
      if (CT_MBX.MESSAGE[MB_FROM_ARM_HOST] == 1)
      {
        /* Receive the message */
        if (pru_rpmsg_receive(&_transport, &_src, &_dst, _payload, &_len) == PRU_RPMSG_SUCCESS)
        {
          _connected = true;
          _on_rx_callback_func((char*)_payload, (size_t)_len);
        }
      }
    }
  }
}

int16_t rpmsg_handler_send(char const *payload, size_t length)
{
  if (!_connected) {
    return -4; //RPMSG_NOT_CONNECTED
  }
  return pru_rpmsg_send(&_transport, _dst, _src, (void*)payload, length);
}
