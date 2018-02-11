#include "command_interface.h"
#include "timer.h"
#include "rpmsg_tx_interface.h"

class PeriodicRpmsgTickSender : public CommandInterface
{
public:
  PeriodicRpmsgTickSender(Timer& timer, RpMsgTxInterface& rpmsg);
  void execute();

private:
  Timer& _timer;
  RpMsgTxInterface& _rpmsg;
};
