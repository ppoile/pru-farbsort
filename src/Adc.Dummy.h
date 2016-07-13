/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef ADCDUMMY_H
#define ADCDUMMY_H

#include "Adc.h"

#include <queue>
#include <cstdint>

class AdcDummy :
    public Adc
{
public:
  AdcDummy();

  std::queue<uint32_t> stoppedSamples;
  std::queue<uint32_t> startedSamples;
  bool running;

  bool isIdle() const override;
  void start() override;
  void stop() override;

  uint32_t sampleCount() const override;
  uint32_t popSample() override;

private:
  const std::queue<uint32_t> &activeSamples() const;
  std::queue<uint32_t> &activeSamples();

};

#endif
