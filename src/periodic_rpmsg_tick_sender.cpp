#include "periodic_rpmsg_tick_sender.h"
#include "msg_definition.h"

PeriodicRpmsgTickSender::PeriodicRpmsgTickSender(Timer& timer, RpMsgTxInterface& rpmsg)
  : _timer(timer)
  , _rpmsg(rpmsg)
{}

void PeriodicRpmsgTickSender::execute()
{
  _rpmsg.post_info(INFO_TICK);
  _timer.registerCommand(this, 100);
}
