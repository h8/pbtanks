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

#ifndef LANDSCAPE_H_INCLUDED
#define LANDSCAPE_H_INCLUDED

#include "utils.h"

#define L_IN 1
#define L_OUT -1
#define L_NA 0

typedef struct landscape
{
    ushort *points;
    Size *size;
} Landscape;

typedef struct landprops
{
    float height_diff; // from 0.2 to 0.8
    float cliffness;  // from 0.2 to 0.8
    float max_peak_height; // 0.7 by def
    float max_min_diff;  // 0.3 by def (max * 0.3 = min)
    ushort dither_height; // 4 by def
} LandProps;

static const LandProps DEF_LANDPROPS = {0.6, 0.2, 0.7, 0.3, 4};  // default

Landscape * landscape_new(Size *s);
void landscape_free(Landscape *l);

void landscape_set_point(Landscape *l, ushort x, int height);
int landscape_get_point(Landscape *l, ushort x);

int landscape_get_max_height_in(Landscape *l, ushort xs, ushort xe);
int landscape_get_max_height(Landscape *l);
int landscape_get_min_height_in(Landscape *l, ushort xs, ushort xe);
int landscape_get_min_height(Landscape *l);

void landscape_generate(Landscape *l, LandProps *props);
void landscape_dig_tank_pad(Landscape *l, Point *p);
void landscape_dig_explosion(Landscape *l, Point *p, int r);

int landscape_is_in(Landscape *l, Point *p);

void landscape_draw(Landscape *l, int color);

#endif // LANDSCAPE_H_INCLUDED
