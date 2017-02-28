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

class ColorDetectTest : public ::testing::Test{
protected:

    MockAdc adc;
    MockTimer timer;
    Queue<Color,COLOR_QUEUE_SIZE> queue;

    Hw hw;
    ColorDetect colorDetect;

    ColorDetectTest():
        hw(0, 0, 0, 0, 0, 0, 0, &adc),
        colorDetect(hw, &timer, queue){};

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

    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();

    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.pull(), RED);
}

TEST_F(ColorDetectTest, 3AdcValuesDefiningRed_ShallDetectColorWhite)
{
    EXPECT_CALL(adc, read())
            .Times(3)
            .WillOnce(Return(1228))
            .WillOnce(Return(780))
            .WillOnce(Return(1228));

    // color detect shall register itself to keep running
    EXPECT_CALL(timer, registerCommand(_, _))
            .Times(3);

    colorDetect.execute();
    colorDetect.execute();
    colorDetect.execute();

    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.pull(), WHITE);
}



