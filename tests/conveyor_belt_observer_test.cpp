#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "common.h"
#include "hw.h"
#include "queue.h"
#include "motor_interface.h"
#include "piston_interface.h"
#include "light_barrier.h"
#include "adc_interface.h"
#include "color_detect.h"
#include "timer_interface.h"
#include "command_interface.h"
#include "controller.h"
#include "controller_state_diagnostic.h"
#include "controller_state_normal_stopped.h"
#include "controller_state_normal_started.h"
#include "rpmsg_tx_interface.h"
#include "rpmsg_rx_interface.h"
#include "msg_definition.h"
#include "pulse_counter_interface.h"
#include "conveyor_belt_observer.h"

using namespace testing;

class MockTimer: public TimerInterface
{
public:
    MOCK_METHOD2(registerCommand, void(CommandInterface *command, int ticks) );
    MOCK_METHOD1(unregisterCommand, void(CommandInterface *command) );
};

class MockRpMsgTx : public RpMsgTxInterface
{
public:
    MOCK_METHOD2(post_msg, int16_t (char const *event, uint16_t length));
    MOCK_METHOD1(post_info, int16_t (char info));
    MOCK_METHOD1(registerReceiver, void(RpMsgRxInterface* messageHandler));
};

class MockPulseCounter : public PulseCounterInterface
{
public:
    MOCK_METHOD0(reset, int());
};


class ConveyorObserverTest : public ::testing::Test{
protected:
    MockTimer timer;
    MockPulseCounter pulseCounter;
    MockRpMsgTx rpmsg;
    ConveyorBeltObserver conveyorBeltObserver;

    ConveyorObserverTest():conveyorBeltObserver(&pulseCounter, &timer, &rpmsg)
    {

    }

};

TEST_F(ConveyorObserverTest, noPulses_shallBeRecognizedAsNotRunning)
{
    EXPECT_CALL(pulseCounter, reset())
            .Times(1)
            .WillOnce(Return(0));

    // observer shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(1);

    conveyorBeltObserver.execute();
}

TEST_F(ConveyorObserverTest, Pulses_shallBeRecognizedAsRunning)
{
    EXPECT_CALL(pulseCounter, reset())
            .Times(1)
            .WillOnce(Return(3));

    // observer shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(1);

    EXPECT_CALL(rpmsg, post_info(INFO_CONVEYOR_RUNNING)).Times(1);

    conveyorBeltObserver.execute();
}

TEST_F(ConveyorObserverTest, NoPulsesThenPulsesThenNoPulses_shallBeRecognizedAsStoppedRunningStopped)
{
    EXPECT_CALL(pulseCounter, reset())
            .Times(3)
            .WillOnce(Return(0))
            .WillOnce(Return(3))
            .WillOnce(Return(0));


    // observer shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _)).Times(3);

    {
          InSequence s;

          EXPECT_CALL(rpmsg, post_info(INFO_CONVEYOR_RUNNING)).Times(1);
          EXPECT_CALL(rpmsg, post_info(INFO_CONVEYOR_STOPPED)).Times(1);
    }

    conveyorBeltObserver.execute();
    conveyorBeltObserver.execute();
    conveyorBeltObserver.execute();
}


