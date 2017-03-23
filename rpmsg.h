#ifndef RPMSG_H
#define RPMSG_H
#include "rpmsg_tx_interface.h"

class RpMsgRxInterface;


class RpMsgTrx : public RpMsgTxInterface
{
public:
    RpMsgTrx();
    void start();
    int16_t post_msg(char const *event, uint16_t length);
    int16_t post_info(char info);
    void registerReceiver(RpMsgRxInterface* messageHandler);
    void processMessages();

private:
    RpMsgRxInterface* messageHandler;
    uint8_t payload[RPMSG_BUF_SIZE];
    struct pru_rpmsg_transport transport;
    bool rpmsg_connected;

    // A message will need to be sent to the PRU before the PRU will know the destination address.
    // https://e2e.ti.com/support/arm/sitara_arm/f/791/p/543704/1983379
    uint16_t src;   // from where the message was received
    uint16_t dst;   // where the message is sent to
    uint16_t len;
    bool verbose;

};

#endif // RPMSG_H
