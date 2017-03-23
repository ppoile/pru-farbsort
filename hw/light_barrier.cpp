#include <stdint.h>

#include "light_barrier.h"
#include "rpmsg_tx_interface.h"
#include "msg_definition.h"

LightBarrier::LightBarrier(int id, int mask, RpMsgTxInterface *rpmsg) : Gpi(mask), id(id), bOldIsInterrupted(false), rpmsg(rpmsg)
{

}

bool LightBarrier::isInterrupted(void)
{
    return getStatus();
}

void LightBarrier::poll()
{
    bool newIsInterrupted = isInterrupted();

    if(newIsInterrupted != bOldIsInterrupted)
    {
        bOldIsInterrupted = newIsInterrupted;

        if((id > 0) && (id < 3)) // message only for lb1 & lb2
        {
            uint8_t interrupted = (newIsInterrupted == true) ? 1 : 0;
            uint8_t msg = INFO_LIGHT_BARRIER_1_BRIGHT + 2 * (id - 1) + interrupted;
            rpmsg->post_info(msg);
        }
    }
}
