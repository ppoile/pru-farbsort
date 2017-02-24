#include "controller_state_normal_state.h"

ControllerStateNormalState::ControllerStateNormalState(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg)
    :hw(hw), timer(timer), rpmsg(rpmsg)
{

}


