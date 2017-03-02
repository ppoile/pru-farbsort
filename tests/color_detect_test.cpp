#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "common.h"
#include "hw.h"
#include "queue.h"
#include "motor.h"
#include "piston.h"
#include "light_barrier.h"
#include "adc_interface.h"
#include "color_detect.h"
#include "timer_interface.h"
#include "command_interface.h"
#include "rpmsg_tx_interface.h"
#include "msg_definition.h"

using namespace testing;

class MockAdc: public AdcInterface {
public:
    MOCK_METHOD0(read, uint16_t());
};

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

class ColorDetectTest : public ::testing::Test{
protected:

    MockAdc adc;
    MockTimer timer;
    Queue<Color,COLOR_QUEUE_SIZE> queue;
    MockRpMsgTx rpmsg;

    Hw hw;
    ColorDetect colorDetect;

    ColorDetectTest():
        hw(0, 0, 0, 0, 0, 0, 0, &adc),
        colorDetect(hw, &timer, queue, &rpmsg){};

};


TEST_F(ColorDetectTest, NotEnoughAdcValues_NoColorDetected)
{
    EXPECT_CALL(adc, read())
            .Times(1)
            .WillOnce(Return(1228));

    // color detect shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(1);

    ASSERT_TRUE(queue.isEmpty());

    colorDetect.execute();

    // no color detected queue still empty
    ASSERT_TRUE(queue.isEmpty());

}

TEST_F(ColorDetectTest, 3AdcValuesDefiningBlue_ShallDetectColorBlue)
{
    EXPECT_CALL(adc, read())
            .Times(3)
            .WillOnce(Return(1228))
            .WillOnce(Return(1185))
            .WillOnce(Return(1228));

    // color detect shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(3);

    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_BLUE)).Times(1);

    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();

    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.pull(), BLUE);
}

TEST_F(ColorDetectTest, 3AdcValuesDefiningRed_ShallDetectColorRed)
{
    EXPECT_CALL(adc, read())
            .Times(3)
            .WillOnce(Return(1228))
            .WillOnce(Return(820))
            .WillOnce(Return(1228));

    // color detect shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(3);

    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_RED)).Times(1);

    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();

    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.pull(), RED);
}

TEST_F(ColorDetectTest, 3AdcValuesDefiningWhite_ShallDetectColorWhite)
{
    EXPECT_CALL(adc, read())
            .Times(3)
            .WillOnce(Return(1228))
            .WillOnce(Return(780))
            .WillOnce(Return(1228));

    // color detect shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(3);

    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_WHITE)).Times(1);

    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();

    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.pull(), WHITE);
}

TEST_F(ColorDetectTest, 3AdcValuesDefiningWhiteRedBlue_ShallDetectColorsWhiteRedBlue)
{
    EXPECT_CALL(adc, read())
            .Times(7)
            .WillOnce(Return(1228))
            .WillOnce(Return(780))
            .WillOnce(Return(1228))
            .WillOnce(Return(820))
            .WillOnce(Return(1228))
            .WillOnce(Return(1185))
            .WillOnce(Return(1228));

    // color detect shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(7);

    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_WHITE)).Times(1);
    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_RED)).Times(1);
    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_BLUE)).Times(1);

    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();

    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.pull(), WHITE);
    ASSERT_EQ(queue.pull(), RED);
    ASSERT_EQ(queue.pull(), BLUE);
}

TEST_F(ColorDetectTest, 3AdcValuesDefiningWhiteRedInShortDistance_ShallDetectColorsWhiteRed)
{
    // local maximum (1185) is not a blue, it's the transition betwen white and red
    EXPECT_CALL(adc, read())
            .Times(5)
            .WillOnce(Return(1228))
            .WillOnce(Return(780))
            .WillOnce(Return(1185))
            .WillOnce(Return(820))
            .WillOnce(Return(1228));

    // color detect shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(5);

    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_WHITE)).Times(1);
    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_RED)).Times(1);

    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();


    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.pull(), WHITE);
    ASSERT_EQ(queue.pull(), RED);
    ASSERT_TRUE(queue.isEmpty());
}

TEST_F(ColorDetectTest, 3AdcValuesDefiningBlueBouncing_ShallDetectOnlyOneBlue)
{
    EXPECT_CALL(adc, read())
            .Times(5)
            .WillOnce(Return(1228))
            .WillOnce(Return(1185))
            .WillOnce(Return(1200))
            .WillOnce(Return(1185))
            .WillOnce(Return(1228));

    // color detect shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(5);

    EXPECT_CALL(rpmsg, post_info(INFO_COLOR_BLUE)).Times(1);

    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();


    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.pull(), BLUE);
    ASSERT_TRUE(queue.isEmpty());
}


