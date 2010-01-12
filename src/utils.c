/************************************************************************/
/* PBTanks                                                              */
/* URLs: https://code.google.com/p/pbtanks                              */
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

#include <time.h>
#include <math.h>

#include "utils.h"

Point * point_new(int x, int y)
{
    Point *p = malloc(sizeof(Point));

    p->x = x;
    p->y = y;

    return p;
}

Size * size_new(int width, int height)
{
    Size *s = malloc(sizeof(Size));

    s->width = width;
    s->height = height;

    return s;
}

void init_rnd()
{
    time_t t;
    time(&t);
    srand(t);
}

int rnd(int limit)
{
    return rand() % limit + 1;
}

int rnd_tw(int limit)
{
    int r = rnd(100) > 50? 1 : -1;
    return rand() % limit * r;
}

int rnd_range(int min, int max)
{
    if (max > min && max - min != 0)
        return rnd(max - min) + min;
    else
        return min;
}

float deg_to_rad(float deg)
{
    return deg * M_PI / 180;
}
