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


using namespace testing;

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

class CtrlStateNormalStartedTest : public ::testing::Test
{
protected:
    CtrlStateNormalStartedTest():
        hw(&motor,&p1,&p2,&p3,&lb1,&lb2,&lbEmergencyStop, 0),
        state(hw, &timer, &rpmsgtx, queue, &colorDetectCommand, ejectCommandPool)
    {};

    Hw hw;
    ControllerStateNormalStateStarted state;
    MockMotor motor;
    MockPiston p1;
    MockPiston p2;
    MockPiston p3;
    MockLightBarrier lb1;
    MockLightBarrier lb2;
    MockLightBarrier lbEmergencyStop;
    MockTimer timer;
    MockRpMsgTx rpmsgtx;
    Queue<Color,COLOR_QUEUE_SIZE> queue;
    MockCommand colorDetectCommand;
    ObjectPool<BrickEjectCommand, 5> ejectCommandPool;

};


TEST_F(CtrlStateNormalStartedTest, stateEntry_shallStartMotorAndColorDetection)
{

    // motor for conveyor belt shall be started
    EXPECT_CALL(motor, start()).Times(1);
    // color dectection shall be started in 50ms
    EXPECT_CALL(timer, registerCommand(&colorDetectCommand,5)).Times(1);

    state.onEntry();
}

TEST_F(CtrlStateNormalStartedTest, stateExit_shallStopMotorAndColorDetection)
{

    // motor for conveyor belt shall be stopped
    EXPECT_CALL(motor, stop()).Times(1);
    // color dectection shall be stopped
    EXPECT_CALL(timer, unregisterCommand(&colorDetectCommand)).Times(1);

    state.onExit();
}
