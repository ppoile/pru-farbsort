/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef LIGHTSEQUENCE_UP_H
#define LIGHTSEQUENCE_UP_H

class Led;

namespace lightsequence
{

class Up
{
public:
  Up(Led &led);

  void tick();

private:
  Led &led;
  unsigned count;

};

}

#endif
