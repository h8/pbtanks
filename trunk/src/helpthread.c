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

#include "helpthread.h"
#include "gutils.h"
#include "main.h"
#include "glang.h"

extern const ibitmap help_pb301_background, help_pb360_background;

/* internal functions */
void draw_help();

int help_handler(int type, int key, int rtime)
{
    if (type == EVT_SHOW)
        draw_help();

    if (type == EVT_KEYPRESS)
        SetEventHandler(main_handler);

    return 0;
}

void draw_help()
{
    ClearScreen();
    FullUpdate();

    irect head_rect = {100, 50, ScreenWidth() - 200, 50, 0};

    ifont *f_head = OpenFont(GFONTB, 18, 1);
    ifont *f_body = OpenFont(GFONTB, 14, 1);

    const char *info_buf = _("Main goal is to hit all other tanks on the land"
                        " by adjusting barrel angle and missile power "
                        "(initial velocity). Wind speed and direction will "
                        "affect missile flight.");
    const char *back_key_buf = _("Exit or return");
    const char *ok_key_buf = _("Fire, select and change");
    const char *clean_key_buf = _("Clear game screen from tracing paths");
    const char *arrows_key_buf = _("Select, change, adjust power (up and "
                                "down) and angle (left and right) "
                                "in the game.");

    SetFont(f_body, BLACK);

    switch (glob_book_model)
    {
        case 301:
            DrawBitmap(0, 0, &help_pb301_background);
            DrawTextRect(36, 388, 160, 50, (char *)back_key_buf,
                            ALIGN_FIT | VALIGN_BOTTOM);
            DrawTextRect(615, 455, 147, 50, (char *)ok_key_buf,
                            ALIGN_FIT | VALIGN_BOTTOM);
            DrawTextRect(36, 475, 160, 50, (char *)clean_key_buf,
                            ALIGN_FIT | VALIGN_BOTTOM);
            DrawTextRect(595, 140, 167, 200, (char *)arrows_key_buf,
                            ALIGN_FIT | VALIGN_BOTTOM);
            break;
        case 360:
            DrawBitmap(0, 0, &help_pb360_background);
            DrawTextRect(30, 385, 220, 50, (char *)back_key_buf,
                            ALIGN_FIT | VALIGN_BOTTOM);
            DrawTextRect(500, 195, 275, 50, (char *)ok_key_buf,
                            ALIGN_FIT | VALIGN_BOTTOM);
            DrawTextRect(560, 262, 215, 50, (char *)clean_key_buf,
                            ALIGN_FIT | VALIGN_BOTTOM);
            DrawTextRect(567, 395, 200, 100, (char *)arrows_key_buf,
                            ALIGN_FIT | VALIGN_BOTTOM);
            break;
    }

    draw_text_rect(&head_rect, f_head, _("Quick help"),
                    ALIGN_CENTER | VALIGN_MIDDLE);

    DrawTextRect(110, 125, ScreenWidth() - 220, 100, (char *)info_buf,
                    ALIGN_FIT | VALIGN_TOP);

    SetFont(f_body, BLACK);
    DrawTextRect(0, ScreenHeight() - 50, ScreenWidth(), 50,
                 (char *)_("Press any key to return ..."),
                 ALIGN_CENTER | VALIGN_MIDDLE);

    CloseFont(f_head);
    CloseFont(f_body);

    FullUpdate();
    FineUpdate();
}
