/************************************************************************/
/* PBTanks                                                              */
/* URLs: http://code.google.com/p/pbtanks                              */
/* Copyright (C) 2010, Dmitry Stropaloff <dmitry.stropaloff@gmail.com>  */
/*                                                                      */
/* This program is free software; you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with this program. If not, see <http://www.gnu.org/licenses/>. */
/************************************************************************/

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "inc.h"

typedef struct point
{
    int x;
    int y;
} Point;

typedef struct size
{
    int width;
    int height;
} Size;

Point * point_new(int x, int y);
Size * size_new(int width, int height);

void init_rnd();
int rnd(int limit);
int rnd_tw(int limit);
int rnd_range(int min, int max);

float deg_to_rad(float deg);

#endif // UTILS_H_INCLUDED
