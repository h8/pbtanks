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

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "gmanager.h"

/* global used fonts */
#define GFONT "LiberationSans"
#define GFONTB "LiberationSans-Bold"

/* global vars */
ushort glob_book_model;
char glob_back_key;
char glob_clean_key;  // clean traj. key
//char glob_marker_flag;  // tank marker flag
GInitData *glob_idata;

int main_handler(int type, int par1, int par2);
void main_handler_ret();

#endif // MAIN_H_INCLUDED
