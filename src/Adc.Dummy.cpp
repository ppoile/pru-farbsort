/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "Adc.Dummy.h"

AdcDummy::AdcDummy() :
  stoppedSamples(),
  startedSamples(),
  running(false)
{
}

bool AdcDummy::isIdle() const
{
  return true;
}

void AdcDummy::start()
{
  running = true;
}

void AdcDummy::stop()
{
  running = false;
}

uint32_t AdcDummy::sampleCount() const
{
  return activeSamples().size();
}

uint32_t AdcDummy::popSample()
{
  const auto value = activeSamples().front();
  activeSamples().pop();
  return value;
}

const std::queue<uint32_t> &AdcDummy::activeSamples() const
{
  if (running) {
    return startedSamples;
  } else {
    return stoppedSamples;
  }
}

std::queue<uint32_t> &AdcDummy::activeSamples()
{
  return const_cast<std::queue<uint32_t>&>(static_cast<const AdcDummy*>(this)->activeSamples());
}
