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

#include "gutils.h"

void draw_text_rect(irect *rect, ifont *font, const char *buf, int flags)
{
    FillArea(rect->x + 2, rect->y + 2, rect->w, rect->h, BLACK);
    FillArea(rect->x, rect->y, rect->w, rect->h, LGRAY);
    SetFont(font, BLACK);
    DrawTextRect(rect->x + 20, rect->y, rect->w - 20, rect->h,
                    (char *)buf, flags);
}

void draw_menu_rect(irect *rect, ifont *font,
                        const char *buf, int flags, int active)
{
    FillArea(rect->x + 2, rect->y + 2, rect->w, rect->h, BLACK);
    FillArea(rect->x, rect->y, rect->w, rect->h, LGRAY);
    SetFont(font, BLACK);
    DrawTextRect(rect->x + 20, rect->y, rect->w - 20, rect->h,
                    (char *)buf, flags);

    if (active == rect->flags)
    {
        DrawRect(rect->x - 2, rect->y - 2, rect->w + 6, rect->h + 6, BLACK);
        DrawRect(rect->x - 3, rect->y - 3, rect->w + 8, rect->h + 8, BLACK);
    }
}
