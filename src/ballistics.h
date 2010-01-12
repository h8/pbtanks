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

#ifndef BALLISTICS_H_INCLUDED
#define BALLISTICS_H_INCLUDED

#include "utils.h"

typedef struct ballistics
{
    Point ps;  // start point
    float a; // angle, in radians
    float v;  // speed, m/s
    signed char wind;  // wind speed and dir.
    float time;  // end time in flight until explosion
} Ballistics;

typedef struct bqitem
{
    Ballistics *bdata;
    struct bqitem *next;
    struct bqitem *prev;
} BQItem;

typedef struct bqueue
{
    ushort count;
    ushort max_count;
    BQItem *head;
    BQItem *tail;
    BQItem *iter;
} BQueue;

void ballistics_get_point_at_t(Ballistics *b, Point *p, float t);
void ballistics_draw(Ballistics *b, int color);

BQueue * bqueue_new();
void bqueue_free(BQueue *q);

void bqueue_add(BQueue *q, Ballistics *b);
void bqueue_clear(BQueue *q);
void bqueue_iter_to_head(BQueue *q);
Ballistics * bqueue_iter_next(BQueue *q);

#endif // BALLISTICS_H_INCLUDED
