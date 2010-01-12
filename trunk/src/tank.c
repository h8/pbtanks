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

#include "tank.h"

extern const ibitmap tank_type_0, tank_type_1, tank_type_2;

/* internal functions */
void get_barrel_end(Tank *t, Point *p);

Tank * tank_new()
{
    Tank *t = malloc(sizeof(Tank));

    t->angle = rnd(170);
    t->power = rnd_range(30, 80);
    strncpy(t->name, "N/A", T_NAME_LEN);

    return t;
}

int tank_is_in(Tank *t, Point *p)
{
    int y;
    float a;

    if (abs(p->x - t->pos.x) <= T_WIDTH2)
    {
        a = acos((t->pos.x - p->x) / T_HEIGHT2);
        y = t->pos.y + T_HEIGHT2 * sin(a);

        if (p->y >= t->pos.y && p->y <= y)
            return 1;
    }

    return -1;
}

void get_barrel_end(Tank *t, Point *p)
{
    float a = deg_to_rad(t->angle);

    p->x = t->pos.x + T_HEIGHT2 * cos(a);
    p->y = (t->pos.y + T_HEIGHT2) + T_HEIGHT2 * sin(a);
}

void tank_fill_ballistics_data(Tank *t, Ballistics *b)
{
    get_barrel_end(t, &b->ps);
    b->a = deg_to_rad(t->angle);
    b->v = t->power;  // FIXME: !!! power scaling !!!
}

void tank_set_power(Tank *t, ushort value)
{
    if (value <= T_MAX_POWER)
        t->power = value;
}

void tank_change_power(Tank *t, int inc)
{
    tank_set_power(t, t->power + inc);
}

void tank_set_angle(Tank *t, ushort value)
{
    if (value <= 180)
        t->angle = value;
}

void tank_change_angle(Tank *t, int inc)
{
    tank_set_angle(t, t->angle + inc);
}

void tank_draw(Tank *t, ushort landscape_height)
{
    Point p;
    ibitmap *b;

    switch (t->type)
    {
        case TTFORTR:
            b = (ibitmap *)&tank_type_1;
            break;
        case TTTOWER:
            b = (ibitmap *)&tank_type_2;
            break;
        default:
            b = (ibitmap *)&tank_type_0;
    };

    get_barrel_end(t, &p);

    DrawLine(t->pos.x, landscape_height - (t->pos.y + T_HEIGHT2),
            p.x, landscape_height - p.y, BLACK);
    DrawBitmap((t->pos.x - T_WIDTH2),
                landscape_height - (t->pos.y + T_HEIGHT), b);
}

void tank_redraw(Tank *t, ushort landscape_height)
{
    FillArea(t->pos.x - T_WIDTH2, landscape_height - (t->pos.y + T_HEIGHT), T_WIDTH, T_HEIGHT2, WHITE);

    tank_draw(t, landscape_height);

    PartialUpdate(t->pos.x - T_WIDTH2 + 2, landscape_height - (t->pos.y + T_HEIGHT), T_WIDTH - 4, T_HEIGHT2);
}

TQueue * tqueue_new()
{
    TQueue *q = malloc(sizeof(TQueue));

    q->count = 0;
    q->head = NULL;
    q->iter = NULL;

    return q;
}

void tqueue_free(TQueue *q)
{
    tqueue_clear(q);

    free(q);
}

void tqueue_clear(TQueue *q)
{
    TQItem *qi, *ql;

    qi = q->head;
    while (qi != NULL)
    {
        ql = qi->next;
        free(qi->tdata);
        free(qi);
        qi = ql;
    }

    q->head = NULL;
    q->count = 0;
}

void tqueue_add(TQueue *q, Tank *t)
{
    TQItem *qi = malloc(sizeof(TQItem));

    qi->next = q->head;
    qi->tdata = t;
    t->id = q->count++;

    q->head = qi;
}

void tqueue_del(TQueue *q, char id)
{
    TQItem *qi = q->head;
    TQItem *qp = NULL;

    while (qi != NULL)
        if (qi->tdata->id == id)
        {
            q->count--;

            if (qp != NULL)
                qp->next = qi->next;
            else
                q->head = qi->next;

            free(qi->tdata);
            free(qi);

            qi = NULL;
        }
        else
        {
            qp = qi;
            qi = qi->next;
        }
}

Tank * tqueue_get(TQueue *q, char id)
{
    Tank *t = NULL;
    TQItem *qi = q->head;

    while (qi != NULL)
        if (qi->tdata->id == id)
        {
            t = qi->tdata;
            break;
        }
        else
            qi = qi->next;

    return t;
}

void tqueue_iter_to_head(TQueue *q)
{
    q->iter = q->head;
}

Tank * tqueue_iter_next(TQueue *q)
{
    Tank *t = NULL;

    if (q->iter != NULL)
    {
        t = q->iter->tdata;
        q->iter = q->iter->next;
    }

    return t;
}

