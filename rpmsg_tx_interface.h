#ifndef RPMSG_INTERFACE
#define RPMSG_INTERFACE

class RpMsgRxInterface;

class RpMsgTxInterface{
    public:

    virtual int16_t post_msg(char const *event, uint16_t length) = 0;
    virtual int16_t post_info(char info) = 0;
    virtual void registerReceiver(RpMsgRxInterface* messageHandler) = 0;

};

#endif // RPMSG_INTERFACE

