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


#include <math.h>

#include "ballistics.h"

#define GC2 4.9
#define DRAWINC 0.01
#define BMAX 10

void ballistics_get_point_at_t(Ballistics *b, Point *p, float t)
{
    p->x = b->ps.x + (b->v * cos(b->a)) * t + (b->wind * t);
	p->y = b->ps.y + (b->v * sin(b->a)) * t - GC2 * pow(t, 2);
}

void ballistics_draw(Ballistics *b, int color)
{
    float index;
    Point p;
    int screen_height = ScreenHeight();
    for (index = 0; index < b->time; index += DRAWINC)
    {
        ballistics_get_point_at_t(b, &p, index);
        DrawPixel(p.x, screen_height - p.y, color);
    }
}

/* internal functions */
void popl(BQueue *q);

BQueue * bqueue_new()
{
    BQueue *q = malloc(sizeof(BQueue));

    q->count = 0;
    q->max_count = BMAX;

    q->head = NULL;
    q->tail = NULL;
    q->iter = NULL;

    return q;
}

void bqueue_free(BQueue *q)
{
    bqueue_clear(q);

    free(q);
}

void bqueue_clear(BQueue *q)
{
    while (q->count > 0)
        popl(q);

    q->head = NULL;
    q->tail = NULL;
    q->iter = NULL;

    q->count = 0;
}

void popl(BQueue *q)
{
    BQItem *qi = q->tail;

    if (qi != NULL)
    {
        free(qi->bdata);

        q->tail = qi->prev;
        if (q->tail != NULL)
            q->tail->next = NULL;

        q->count--;
        free(qi);
    }
}

void bqueue_add(BQueue *q, Ballistics *b)
{
    if (b == NULL)
        return;

    BQItem *qi = malloc(sizeof(BQItem));

    qi->bdata = b;
    qi->prev = NULL;
    qi->next = q->head;

    if (q->head != NULL)
        q->head->prev = qi;
    else
        q->tail = qi;

    q->head = qi;
    q->count++;

    if (q->count > q->max_count)
        popl(q);
}

void bqueue_iter_to_head(BQueue *q)
{
    q->iter = q->head;
}

Ballistics * bqueue_iter_next(BQueue *q)
{
    Ballistics *b = NULL;

    if (q->iter != NULL)
    {
        b = q->iter->bdata;
        q->iter = q->iter->next;
    }

    return b;
}
