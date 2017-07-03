#ifndef RECEIVE_MESSAGE_HANDLER_INTERFACE
#define RECEIVE_MESSAGE_HANDLER_INTERFACE

class RpMsgRxInterface
{
public:
    virtual void processesMessage(uint8_t *msg, uint16_t size) = 0;
};

#endif // RECEIVE_MESSAGE_HANDLER_INTERFACE

