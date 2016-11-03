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

	if (_isValidId())
	{
	    uint8_t msg = _getMsgFromIdAndInterrupted(newIsInterrupted);
	    rpmsg->post_info(msg);
	}
    }
}

bool LightBarrier::_isValidId() const
{
  return (id > 0) && (id < 3);
}

uint8_t LightBarrier::_getMsgFromIdAndInterrupted(bool isInterrupted)
{
  uint8_t msg = INFO_LIGHT_BARRIER_1_BRIGHT; // base msg id
  msg += 2 * (id - 1); // consider light-barrier id...
  msg += isInterrupted ? 1 : 0; // ...and value
  return msg;
}
