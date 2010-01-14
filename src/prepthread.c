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

#include "prepthread.h"
#include "gamethread.h"
#include "main.h"
#include "gutils.h"
#include "glang.h"

#define MENU_NAME 0
#define MENU_AI 1
#define MENU_TYPE 2
#define MENU_OK 3

extern const ibitmap prep_background;
extern const ibitmap tank_type_0, tank_type_1, tank_type_2;

static const char *tnames[] = {"Goggy", "Murko", "Sarah", "Arny", "Harry",
                               "Petro", "Jikko", "Ummi", "Neo", "Folks"
                              };

/* internal vars */
const char tinitname[] = "Player";
char tname[T_NAME_LEN];
int tank_index;
int active_item;
int ai_type;
int tank_type;
int human_players;

/* internal functions */
void draw_tank_dialog();
void keyboard_entry(char *s);
void prep_handler_init();

int prep_handler(int type, int key, int rtime)
{
    if (type == EVT_SHOW)
    {
        prep_handler_init();
        draw_tank_dialog();
    }

    if (type == EVT_KEYPRESS)
    {
        if (key == glob_back_key)
            main_handler_ret();

        switch (key)
        {
        case KEY_OK:
            switch (active_item)
            {
            case MENU_NAME:
                if (!ai_type)
                    OpenKeyboard((char *)_("Player\'s name"),
                                    tname, 20, 0, keyboard_entry);
                break;
            case MENU_OK:
                // fill tank data
                if (ai_type == TAIHUMAN)
                {
                    human_players++;
                    strncpy((glob_idata->tdesc[tank_index]).name,
                            tname, T_NAME_LEN);
                }
                else
                    strncpy((glob_idata->tdesc[tank_index]).name,
                            tnames[tank_index], T_NAME_LEN);
                (glob_idata->tdesc[tank_index]).ai = ai_type;
                (glob_idata->tdesc[tank_index]).type = tank_type;
                (glob_idata->tdesc[tank_index]).hits = 0;
                (glob_idata->tdesc[tank_index]).wins = 0;

                if (tank_index >= (glob_idata->tcount - 1))
                {
                    if (human_players > 0)
                        SetEventHandler(game_handler);
                    else
                    {
                        ai_type = TAIHUMAN;
                        draw_tank_dialog();
                    }
                }
                else
                {
                    tank_index++;
                    snprintf(tname, T_NAME_LEN, "%s%i",
                                tinitname, human_players + 1);
                    draw_tank_dialog();
                }
                break;
            }
            break;
        case KEY_DOWN:
            active_item = active_item + 1 > MENU_OK? 0 : active_item + 1;
            draw_tank_dialog();
            break;
        case KEY_UP:
            active_item = active_item - 1 >= 0? active_item - 1 : MENU_OK;
            draw_tank_dialog();
            break;

        case KEY_LEFT:
            switch (active_item)
            {
            case MENU_AI:
                ai_type = ai_type > 0? ai_type - 1 : TAICYBORG;
                draw_tank_dialog();
                break;
            case MENU_TYPE:
                tank_type = tank_type > 0? tank_type - 1 : TTTOWER;
                draw_tank_dialog();
                break;
            }
            break;
        case KEY_RIGHT:
            switch (active_item)
            {
            case MENU_AI:
                ai_type = ai_type < TAICYBORG? ai_type + 1 : 0;
                draw_tank_dialog();
                break;
            case MENU_TYPE:
                tank_type = tank_type < TTTOWER? tank_type + 1 : 0;
                draw_tank_dialog();
                break;
            }
            break;
        }
    }

    return 0;
}

void draw_tank_dialog()
{
    const int side_offset = 100;
    const Size screen = {ScreenWidth(), ScreenHeight()};
    const int name_top = 250;
    const int items_width = 220;
    const int menu_pos = screen.width - side_offset - items_width;
    const int bsize = 100;
    char *buf = calloc(bsize, sizeof(char));
    const char *ait[] = T_AI_NAMES;

    irect head_rect = {side_offset, 50, screen.width - side_offset * 2, 50, 0};
    irect name_rect = {menu_pos, name_top, items_width, 30, MENU_NAME};
    irect type_rect = {menu_pos, name_top + 50, items_width, 30, MENU_AI};
    irect tank_rect = {menu_pos, name_top + 100, items_width, 30, MENU_TYPE};
    irect cont_rect = {menu_pos, name_top + 200, items_width, 30, MENU_OK};

    ibitmap *tank_bm = NULL;

    ifont *f_head = OpenFont(GFONTB, 18, 1);
    ifont *f_menu = OpenFont(GFONTB, 14, 1);

    DrawBitmap(0, 0, &prep_background);
    DrawRect(0, 0, screen.width, ScreenHeight(), BLACK);
    DrawRect(2, 2, screen.width - 4, screen.height - 4, BLACK);

    snprintf(buf, bsize, _("Player setup: player %i of %i"),
             tank_index + 1, glob_idata->tcount);
    draw_text_rect(&head_rect, f_head, buf, ALIGN_CENTER | VALIGN_MIDDLE);

    /* menu */
    if (!ai_type)
        snprintf(buf, bsize, _("Name: %s"), tname);
    else
        snprintf(buf, bsize, _("Name: << predefined >>"));
    draw_menu_rect(&name_rect, f_menu, buf, VALIGN_MIDDLE, active_item);

    snprintf(buf, bsize, _("Type: %s"), ait[ai_type]);
    draw_menu_rect(&type_rect, f_menu, buf, VALIGN_MIDDLE, active_item);

    strncpy(buf, _("Tank: "), bsize);
    draw_menu_rect(&tank_rect, f_menu, buf, VALIGN_MIDDLE, active_item);
    switch (tank_type)
    {
    case TTMARKV:
        tank_bm = (ibitmap *)&tank_type_0;
        break;
    case TTFORTR:
        tank_bm = (ibitmap *)&tank_type_1;
        break;
    case TTTOWER:
        tank_bm = (ibitmap *)&tank_type_2;
        break;
    }
    DrawBitmap(tank_rect.x + StringWidth(buf) + 40, tank_rect.y + 5, tank_bm);

    draw_menu_rect(&cont_rect, f_menu, _("Continue >>"),
                   VALIGN_MIDDLE, active_item);

    /* footer */
    if (glob_back_key == KEY_PREV)
        DrawTextRect(0, screen.height - 50, screen.width, 50,
                     (char *)_("Up/Down - select, Left/Right - change,\n"
                                "Ok - accept, Back to exit."),
                     ALIGN_CENTER | VALIGN_MIDDLE);
    else
        DrawTextRect(0, screen.height - 50, screen.width, 50,
                     (char *)_("Up/Down - select, Left/Right - change,\n"
                                "Ok - accept, Return to exit."),
                     ALIGN_CENTER | VALIGN_MIDDLE);

    CloseFont(f_head);
    CloseFont(f_menu);

    free(buf);

    PartialUpdate(0, 0, screen.width, screen.height);
}

void prep_handler_init()
{
    active_item = 0;
    ai_type = 0;
    tank_type = 0;
    tank_index = 0;
    human_players = 0;
    snprintf(tname, T_NAME_LEN, "%s%i", tinitname, human_players + 1);
}

void keyboard_entry(char *s)
{
    if (s != NULL)
        strncpy(tname, s, T_NAME_LEN - 1);
    else
        strncpy(tname, tinitname, T_NAME_LEN - 1);
    tname[T_NAME_LEN] = '\0';
    draw_tank_dialog();
}
