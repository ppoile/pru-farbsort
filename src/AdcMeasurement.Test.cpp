/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "AdcMeasurement.h"
#include "Adc.Mock.h"
#include "Adc.Dummy.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(AdcMeasurement_Test, reading_a_sample_waits_for_one)
{
  testing::StrictMock<AdcMock> adc;
  AdcMeasurement testee{adc};
  testing::Sequence sequence;
  EXPECT_CALL(adc, sampleCount())
      .InSequence(sequence)
      .WillOnce(testing::Return(0));
  EXPECT_CALL(adc, isIdle())
      .InSequence(sequence)
      .WillOnce(testing::Return(true));
  EXPECT_CALL(adc, start())
      .InSequence(sequence);

  EXPECT_CALL(adc, sampleCount())
      .InSequence(sequence)
      .WillOnce(testing::Return(0));
  EXPECT_CALL(adc, sampleCount())
      .InSequence(sequence)
      .WillOnce(testing::Return(0));
  EXPECT_CALL(adc, sampleCount())
      .InSequence(sequence)
      .WillOnce(testing::Return(1));
  EXPECT_CALL(adc, popSample())
      .InSequence(sequence)
      .WillOnce(testing::Return(0));
  EXPECT_CALL(adc, stop())
      .InSequence(sequence);

  testee.read();
}

TEST(AdcMeasurement_Test, waits_for_idle_bevore_start)
{
  testing::StrictMock<AdcMock> adc;
  AdcMeasurement testee{adc};
  EXPECT_CALL(adc, sampleCount())
      .WillOnce(testing::Return(0))
      .WillRepeatedly(testing::Return(1));
  EXPECT_CALL(adc, popSample())
      .WillRepeatedly(testing::Return(0));
  EXPECT_CALL(adc, stop());

  testing::Sequence sequence;
  EXPECT_CALL(adc, isIdle())
      .InSequence(sequence)
      .WillOnce(testing::Return(false));
  EXPECT_CALL(adc, isIdle())
      .InSequence(sequence)
      .WillOnce(testing::Return(true));
  EXPECT_CALL(adc, start())
      .InSequence(sequence);

  testee.read();
}

TEST(AdcMeasurement_Test, clears_the_fifo_bevore_start)
{
  AdcDummy adc;
  AdcMeasurement testee{adc};

  adc.stoppedSamples.push({});
  adc.stoppedSamples.push({});
  adc.startedSamples.push({});

  testee.read();

  ASSERT_EQ(0, adc.stoppedSamples.size());
}

TEST(AdcMeasurement_Test, stops_the_adc_after_the_measurement)
{
  AdcDummy adc;
  AdcMeasurement testee{adc};
  adc.startedSamples.push({});

  testee.read();

  ASSERT_FALSE(adc.running);
}

TEST(AdcMeasurement_Test, reading_a_sample_returns_the_first_fifo_value)
{
  AdcDummy adc;
  AdcMeasurement testee{adc};

  adc.startedSamples.push(100);
  adc.startedSamples.push(200);

  ASSERT_EQ(100, testee.read());
}

TEST(AdcMeasurement_Test, reading_a_sample_removes_the_channel_info)
{
  AdcDummy adc;
  AdcMeasurement testee{adc};

  adc.startedSamples.push(0x50100);

  ASSERT_EQ(0x0100, testee.read());
}
