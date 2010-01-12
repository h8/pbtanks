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

#ifndef TANK_H_INCLUDED
#define TANK_H_INCLUDED

#include "ballistics.h"

#define T_WIDTH 20
#define T_WIDTH2 10
#define T_HEIGHT 20
#define T_HEIGHT2 10
#define T_PAD_MIN_PEAKS 16
#define T_NAME_LEN 21
#define T_MAX_POWER 150

#define T_AI_NAMES {_("Human"), _("Moron"), _("Shooter"), _("Chooser"), _("Cyborg")}

typedef enum {TTMARKV, TTFORTR, TTTOWER} TankType;
typedef enum {TAIHUMAN, TAIMORON, TAISHOOTER, TAICHOOSER, TAICYBORG} TankAI;

typedef struct tankdesc
{
    TankType type;
    TankAI ai;
    char name[T_NAME_LEN];
    short hits;
    ushort wins;
} TankDesc;

typedef struct tank
{
    TankType type;
    TankAI ai;
    char name[T_NAME_LEN];
    Point pos;
    char id;
    ushort angle;  // in deg
    ushort power;
} Tank;

typedef struct tqitem
{
    Tank *tdata;
    struct tqitem *next;
    struct tqitem *prev;
} TQItem;

typedef struct tqueue
{
    ushort count;
    TQItem *head;
    TQItem *iter;
} TQueue;

int tank_is_in(Tank *t, Point *p);

Tank * tank_new();
void tank_get_barrel_end(Tank *t, Point *p);
void tank_fill_ballistics_data(Tank *t, Ballistics *b);

void tank_set_power(Tank *t, ushort value);
void tank_change_power(Tank *t, int inc);
void tank_set_angle(Tank *t, ushort value);
void tank_change_angle(Tank *t, int inc);

void tank_draw(Tank *t, ushort landscape_height);
void tank_redraw(Tank *t, ushort landscape_height);

TQueue * tqueue_new();
void tqueue_free(TQueue *q);

void tqueue_add(TQueue *q, Tank *t);
void tqueue_del(TQueue *q, char id);
void tqueue_clear(TQueue *q);
Tank * tqueue_get(TQueue *q, char id);
void tqueue_iter_to_head(TQueue *q);
Tank * tqueue_iter_next(TQueue *q);

#endif // TANK_H_INCLUDED
