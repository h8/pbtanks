/************************************************************************/
/* PBTanks                                                              */
/* URLs: http://code.google.com/p/pbtanks                               */
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

#ifndef GMANAGER_H_INCLUDED
#define GMANAGER_H_INCLUDED

#include "tank.h"
#include "landscape.h"

typedef struct ginitdata
{
    ushort rounds;
    ushort cround;
    char tcount;
    signed char maxwind;
    char chanwind;
    char tmarker;
    TankDesc *tdesc;
    LandProps lprops;
} GInitData;

typedef struct gmanager
{
    GInitData *ginitdata;
    Landscape *landscape;
    TQueue *tqueue;
    BQueue *bqueue;

    signed char wind;
    ushort ctid; // current tank id

    ifont *f_stat_bar;  // status bar/panel font
} GManager;

GManager * gmanager_new();
void gmanager_free(GManager *m);

GInitData * gmanager_set_ginit_data(GManager *m, GInitData *idata);
void gmanager_prepare_game(GManager *m);
Tank * gmanager_prepare_turn(GManager *m);
void gmanager_do_turn(GManager *m);
int gmanager_do_turns(GManager *m);
void gmanager_clear_traj(GManager *m);

void gmanager_change_tank_angle(GManager *m, int dir);
void gmanager_change_tank_power(GManager *m, int dir);

void gmanager_draw(GManager *m);

#endif // GMANAGER_H_INCLUDED
