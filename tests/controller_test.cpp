#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../common.h"
#include "../hw.h"
#include "../queue.h"
#include "../motor_interface.h"
#include "../piston_interface.h"
#include "../light_barrier.h"
#include "../adc_interface.h"
#include "../color_detect.h"
#include "../timer_interface.h"
#include "../command_interface.h"
#include "../controller.h"
#include "../controller_state_diagnostic.h"
#include "../controller_state_normal.h"
#include "../controller_state_normal_state_stopped.h"
#include "../controller_state_normal_state_started.h"
#include "../rpmsg_tx_interface.h"
#include "../rpmsg_rx_interface.h"
#include "../msg_definition.h"

using namespace ::testing;

class MockMotor: public MotorInterface {
public:
    MOCK_METHOD0(start, void());
    MOCK_METHOD0(stop, void());
    MOCK_METHOD0(isRunning, bool());

};

class MockPiston: public PistonInterface {
public:
    MOCK_METHOD0(push, void());
    MOCK_METHOD0(pull,void());
    MOCK_METHOD0(isPushed, bool());

};

class MockTimer: public TimerInterface
{
public:
    MOCK_METHOD2(registerCommand, void(CommandInterface *command, int ticks) );
    MOCK_METHOD1(unregisterCommand, void(CommandInterface *command) );
};


class MockLightBarrier: public LightBarrierInterface
{
public:
    MOCK_METHOD0(isInterrupted, bool (void));
};


class MockRpMsgTx : public RpMsgTxInterface
{
public:
    MOCK_METHOD2(post_msg, int16_t (char const *event, uint16_t length));
    MOCK_METHOD1(post_info, int16_t (char info));
    MOCK_METHOD1(registerReceiver, void(RpMsgRxInterface* messageHandler));
};

class MockCommand : public CommandInterface
{
public:
    MOCK_METHOD0(execute, void ());
};

class ControllerTest2: public ::testing::Test
{
protected:
    ControllerTest2()
        : hw(&motor,&p1,&p2,&p3,&lb1,&lb2,&lbEmergencyStop, 0),
          stateNormal(hw, &timer, &rpmsgtx, stateStarted, stateStopped),
          stateDiagnostic(hw, &timer, &rpmsgtx),
          stateStarted(hw, &timer, &rpmsgtx, queue, &colorDetectCommand, ejectCommandPool),
          stateStopped(hw, &timer, &rpmsgtx),
          ctrl(hw, &rpmsgtx, stateDiagnostic, stateNormal)
    {};

    MockMotor motor;
    MockPiston p1;
    MockPiston p2;
    MockPiston p3;
    MockLightBarrier lb1;
    MockLightBarrier lb2;
    MockLightBarrier lbEmergencyStop;
    MockTimer timer;
    NiceMock<MockRpMsgTx> rpmsgtx;  // use nice mock becaust ctrl-ctor calls it
    Queue<Color,COLOR_QUEUE_SIZE> queue;
    MockCommand colorDetectCommand;
    ObjectPool<BrickEjectCommand, 5> ejectCommandPool;


    Hw hw;
    ControllerStateNormal stateNormal;
    ControllerStateDiagnostic stateDiagnostic;
    ControllerStateNormalStateStarted stateStarted;
    ControllerStateNormalStateStopped stateStopped;

    Controller ctrl;
};

TEST(ControllerTest, Construction_shallRegisterForIncommingMessages)
{
    Hw hw{0,0,0,0,0,0,0,0};
    MockRpMsgTx rpmsgtx;
    MockTimer timer;
    MockCommand colorDetectCommand;
    Queue<Color,COLOR_QUEUE_SIZE> queue;
    ObjectPool<BrickEjectCommand, 5> ejectCommandPool;
    ControllerStateNormalStateStarted stateStarted{hw, &timer, &rpmsgtx, queue, &colorDetectCommand,ejectCommandPool};
    ControllerStateNormalStateStopped stateStopped{hw, &timer, &rpmsgtx};
    ControllerStateNormal stateNormal(hw, &timer, &rpmsgtx, stateStarted, stateStopped);
    ControllerStateDiagnostic stateDiagnostic(hw, &timer, &rpmsgtx);
    Controller *pCtrl;

    // controller shall observe incomming messages (thus register ctrl at rpmsg required)
    EXPECT_CALL(rpmsgtx, registerReceiver(_)).Times(1);

    pCtrl= new Controller(hw, &rpmsgtx, stateDiagnostic, stateNormal);
    delete pCtrl; // do not care for non-virtual destructor (there is no dynamic memory in pru)

}

TEST_F(ControllerTest2, Diagnostic_shallStopMotorAndAllowHWControll)
{
    // rpmsg Feedback requestd
    EXPECT_CALL(rpmsgtx, post_info(INFO_CTRL_STOP));
    // hw initialized
    EXPECT_CALL(motor, stop());
    EXPECT_CALL(p1, pull());
    EXPECT_CALL(p2, pull());
    EXPECT_CALL(p3, pull());

    ctrl.processCmd(CMD_MODE_DIAGNOSTIC);

    // can start and stop motor
    EXPECT_CALL(motor, start());
    ctrl.processCmd(CMD_MOTOR_START);
    EXPECT_CALL(motor, stop());
    ctrl.processCmd(CMD_MOTOR_STOP);

    // can controll piston1
    EXPECT_CALL(p1, push());
    ctrl.processCmd(CMD_VALVE_1_ON);
    EXPECT_CALL(p1, pull());
    ctrl.processCmd(CMD_VALVE_1_OFF);

    // can controll piston2
    EXPECT_CALL(p2, push());
    ctrl.processCmd(CMD_VALVE_2_ON);
    EXPECT_CALL(p2, pull());
    ctrl.processCmd(CMD_VALVE_2_OFF);

    // can controll piston3
    EXPECT_CALL(p3, push());
    ctrl.processCmd(CMD_VALVE_3_ON);
    EXPECT_CALL(p3, pull());
    ctrl.processCmd(CMD_VALVE_3_OFF);

}

TEST_F(ControllerTest2, Normal_shallStartMotorAndDoColorDetection)
{
    EXPECT_CALL(motor, start());
    ctrl.processCmd(CMD_START);
}
