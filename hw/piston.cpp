#include "piston.h"
#include "msg_definition.h"
#include "rpmsg_tx_interface.h"

Piston::Piston(int id, uint32_t mask, RpMsgTxInterface *rpmsg): Gpo(mask), id(id), rpmsg(rpmsg)
{

}

void Piston::push()
{
    if(!getStatus())
    {
        set();
        rpmsg->post_info(INFO_VALVE_1_ON + 2 * (id - 1));
    }
}

void Piston::pull()
{
    if(getStatus())
    {
        clear();
        rpmsg->post_info(INFO_VALVE_1_OFF + 2 * (id - 1));
    }
}

bool Piston::isPushed()
{
    return getStatus();
}
