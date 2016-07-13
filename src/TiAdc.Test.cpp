/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "TiAdc.h"

#include <MemoryMap.Mock.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(TiAdc_Test, register_offsets_are_available)
{
  ASSERT_EQ(0x40, TiAdc::CTRL);
  ASSERT_EQ(0x4c, TiAdc::ADC_CLKDIV);
  ASSERT_EQ(0x64, TiAdc::STEPCONFIG1);
  ASSERT_EQ(0x68, TiAdc::STEPDELAY1);
  ASSERT_EQ(0x84, TiAdc::STEPCONFIG5);
  ASSERT_EQ(0x88, TiAdc::STEPDELAY5);
  ASSERT_EQ(0xf4, TiAdc::FIFO1THRESHOLD);
  ASSERT_EQ(0xf0, TiAdc::FIFO1COUNT);
  ASSERT_EQ(0x200, TiAdc::FIFO1DATA);
  ASSERT_EQ(0x44, TiAdc::ADCSTAT);
  ASSERT_EQ(0x54, TiAdc::STEPENABLE);
}

TEST(TiAdc_Test, initializes_adc_with_channel_5)
{
  testing::StrictMock<MemoryMapMock> memory;

  testing::Sequence sequence;
  EXPECT_CALL(memory, write(TiAdc::ADC_CLKDIV,  0x7)).InSequence(sequence);
  EXPECT_CALL(memory, write(TiAdc::CTRL,        0x6)).InSequence(sequence);
  EXPECT_CALL(memory, write(TiAdc::CTRL,        0x7)).InSequence(sequence);

  EXPECT_CALL(memory, write(TiAdc::STEPCONFIG1, 0x04280008)).InSequence(sequence);
  EXPECT_CALL(memory, write(TiAdc::STEPDELAY1,  0x0f000098)).InSequence(sequence);

  EXPECT_CALL(memory, write(TiAdc::FIFO1THRESHOLD,  19)).InSequence(sequence);

  TiAdc testee{memory};
}

TEST(TiAdc_Test, can_start_the_adc)
{
  testing::NiceMock<MemoryMapMock> memory;
  TiAdc testee{memory};

  EXPECT_CALL(memory, write(TiAdc::STEPENABLE, 0x2));

  testee.start();
}

TEST(TiAdc_Test, can_stop_the_adc)
{
  testing::NiceMock<MemoryMapMock> memory;
  TiAdc testee{memory};

  EXPECT_CALL(memory, write(TiAdc::STEPENABLE, 0x0));

  testee.stop();
}

TEST(TiAdc_Test, can_check_for_idle)
{
  testing::NiceMock<MemoryMapMock> memory;
  TiAdc testee{memory};

  EXPECT_CALL(memory, read(TiAdc::ADCSTAT))
      .WillOnce(testing::Return(0));

  ASSERT_TRUE(testee.isIdle());
}

TEST(TiAdc_Test, can_check_for_not_idle)
{
  testing::NiceMock<MemoryMapMock> memory;
  TiAdc testee{memory};

  EXPECT_CALL(memory, read(TiAdc::ADCSTAT))
      .WillOnce(testing::Return(1<<5));

  ASSERT_FALSE(testee.isIdle());
}
