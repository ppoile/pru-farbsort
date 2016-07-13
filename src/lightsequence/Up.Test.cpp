/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "Up.h"
#include "Led.Dummy.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace lightsequence
{

class LightSequence_Up_Test :
    public testing::Test
{
public:
  LedDummy led{0};
  Up testee{led};
};

TEST_F(LightSequence_Up_Test, enables_first_led_after_first_tick)
{
  testee.tick();

  ASSERT_EQ(1 << 0, led.leds);
}

TEST_F(LightSequence_Up_Test, enables_second_led_after_second_tick)
{
  testee.tick();
  testee.tick();

  ASSERT_EQ(1 << 1, led.leds);
}

TEST_F(LightSequence_Up_Test, enables_third_led_after_third_tick)
{
  testee.tick();
  testee.tick();
  testee.tick();

  ASSERT_EQ(1 << 2, led.leds);
}

TEST_F(LightSequence_Up_Test, enables_fourth_led_after_fourth_tick)
{
  testee.tick();
  testee.tick();
  testee.tick();
  testee.tick();

  ASSERT_EQ(1 << 3, led.leds);
}

TEST_F(LightSequence_Up_Test, enables_first_led_after_fifth_tick)
{
  testee.tick();
  testee.tick();
  testee.tick();
  testee.tick();
  testee.tick();

  ASSERT_EQ(1 << 0, led.leds);
}

}
