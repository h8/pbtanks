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

#include "main.h"
#include "gamethread.h"
#include "gutils.h"
#include "glang.h"

extern const ibitmap stat_background;

/* internal vars */
GManager *m;

/* internal functions */
int round_handler(int type, int key, int rtime);
void game_handler_do_turns();
void game_handler_draw_stats();
void game_over_handler_draw();
void round_dialog_handler(int button);
void game_dialog_handler(int button);

int game_over_handler(int type, int key, int rtime)
{
    if (type == EVT_SHOW)
        game_over_handler_draw();

    if (type == EVT_KEYPRESS)
    {
        gmanager_free(m);
        SetEventHandler(game_handler);
    }

    return 0;
}

int game_handler(int type, int key, int rtime)
{
    if (type == EVT_SHOW)
        game_handler_draw_stats();

    if (type == EVT_KEYPRESS)
    {
        if (key == glob_back_key && glob_idata->rounds > glob_idata->cround)
            Dialog(ICON_QUESTION, (char *)_("Confirmation"),
                   (char *)_("\nAre you shure, you want to exit (game will be saved)?"),
                   (char *)_("Yes"), (char *)_("No"), game_dialog_handler);
        else if (key == KEY_OK)
        {
            if (glob_idata->rounds > glob_idata->cround)
                SetEventHandler(round_handler);
            else
                main_handler_ret();
        }
    }

    return 0;
}

int round_handler(int type, int key, int rtime)
{
    if (type == EVT_SHOW)
    {
    	// once at start to clean bakground
        ClearScreen();
        FullUpdate();
        FineUpdate();

        m = gmanager_new();
        gmanager_set_ginit_data(m, glob_idata);
        gmanager_prepare_game(m);
        game_handler_do_turns();
    }

    if (type == EVT_KEYPRESS)
    {
        if (key == glob_back_key)
            Dialog(ICON_QUESTION, (char *)_("Confirmation"),
                   (char *)_("\nAre you shure, you want to exit?"),
                   (char *)_("Yes"), (char *)_("No"), round_dialog_handler);

        if (key == glob_clean_key)
        {
            gmanager_clear_traj(m);
            gmanager_draw(m);
        }

        switch (key)
        {
        case KEY_OK:
            gmanager_do_turn(m);
            game_handler_do_turns();
            break;
        case KEY_UP:
            gmanager_change_tank_power(m, 1);
            break;
        case KEY_DOWN:
            gmanager_change_tank_power(m, -1);
            break;
        case KEY_LEFT:
            gmanager_change_tank_angle(m, 1);
            break;
        case KEY_RIGHT:
            gmanager_change_tank_angle(m, -1);
            break;
        }
    }

    if (type == EVT_KEYREPEAT)
        switch (key)
        {
        case KEY_UP:
            gmanager_change_tank_power(m, 10);
            break;
        case KEY_DOWN:
            gmanager_change_tank_power(m, -10);
            break;
        case KEY_LEFT:
            gmanager_change_tank_angle(m, 10);
            break;
        case KEY_RIGHT:
            gmanager_change_tank_angle(m, -10);
            break;
        }

    return 0;
}

void game_handler_do_turns()
{
    if (gmanager_do_turns(m) < 0)
        SetEventHandler(game_over_handler);
    else
        gmanager_draw(m);
}

void round_dialog_handler(int button)
{
    if (button == 1)
    {
        gmanager_free(m);
        SetEventHandler(game_handler);
    }
}

void game_dialog_handler(int button)
{
    if (button == 1)
    {
        // save state
        int index;
        FILE *f = NULL;
        TankDesc *td = NULL;
        int path_len = strlen(CONFIGPATH) + strlen(SAVENAME) + strlen("/") + 1;
        char *save_path = calloc(path_len, sizeof(char));

        snprintf(save_path, path_len, "%s/%s", CONFIGPATH, SAVENAME);

        f = fopen(save_path, "wb");
        if (f != NULL)
        {
            fwrite(glob_idata, sizeof(GInitData), 1, f);

            for (index = 0; index < glob_idata->tcount; index++)
            {
                td = &glob_idata->tdesc[index];
                fwrite(td, sizeof(TankDesc), 1, f);
            }

            fclose(f);
        }

        free(save_path);

        main_handler_ret();
    }
}

void game_handler_draw_stats()
{
    int index, tindex;
    const int bsize = 100;
    char *buf = calloc(bsize, sizeof(char));
    char *footer_buf = calloc(bsize, sizeof(char));
    char stat_buf[5][bsize];
    const int side_offset = 100;
    const Size screen = {ScreenWidth(), ScreenHeight()};
    const int table_top = 140;
    const int table_spacing = 18;

    const char *stat_head[] = {"N", _("Name"), _("Type"),
                                _("Hits"), _("Wins")};
    const char *ai_str[5] = T_AI_NAMES;

    const int table_points[] = {side_offset,
                                side_offset + 20,
                                screen.width - side_offset - 300,
                                screen.width - side_offset - 160,
                                screen.width - side_offset - 60,
                                screen.width - side_offset
                               };

    irect head_rect = {side_offset, 50,
                       screen.width - side_offset * 2, 50, 0
                      };

    ifont *f_head = OpenFont(GFONTB, 18, 1);
    ifont *f_foot = OpenFont(GFONTB, 14, 1);
    ifont *f_body = OpenFont(GFONTB, 12, 1);

    DrawBitmap(0, 0, &stat_background);
    DrawRect(0, 0, screen.width, screen.height, BLACK);

    if (glob_idata->rounds > glob_idata->cround)
    {
        snprintf(buf, bsize, _("Prepare for round %i of %i"),
                 (glob_idata->cround + 1), glob_idata->rounds);
        if (glob_back_key == KEY_PREV)
            snprintf(footer_buf, bsize,
                        _("Press Ok to start, Back to exit."));
        else
            snprintf(footer_buf, bsize,
                        _("Press Ok to start, Return to exit."));
    }
    else
    {
        snprintf(buf, bsize, _("Game over! %i of %i rounds"),
                 glob_idata->cround, glob_idata->rounds);
        snprintf(footer_buf, bsize, _("Press Ok to continue."));
    }


    draw_text_rect(&head_rect, f_head, buf, ALIGN_CENTER | VALIGN_MIDDLE);

    SetFont(f_foot, BLACK);
    DrawTextRect(0, 110, screen.width, 25, (char *)_("Statistics"),
                 ALIGN_CENTER | VALIGN_MIDDLE);

    /* table head */
    SetFont(f_body, BLACK);
    for (tindex = 0; tindex < 5; tindex++)
    {
        DrawRect(table_points[tindex], table_top,
                 table_points[tindex + 1] - table_points[tindex] + 1,
                 table_spacing + 1,
                 BLACK);
        DrawTextRect(table_points[tindex], table_top,
                     table_points[tindex + 1] - table_points[tindex],
                     table_spacing,
                     (char *)stat_head[tindex],
                     ALIGN_CENTER | VALIGN_MIDDLE);
    }

    /* table body */
    for (index = 0; index < glob_idata->tcount; index++)
    {
        snprintf(stat_buf[0], bsize, "%i", index + 1);
        strncpy(stat_buf[1], (glob_idata->tdesc[index]).name, bsize);

        strncpy(stat_buf[2], ai_str[(glob_idata->tdesc[index]).ai], bsize);
        snprintf(stat_buf[3], bsize, "%i", (glob_idata->tdesc[index]).hits);

        if ((glob_idata->tdesc[index]).ai == TAIHUMAN)
            snprintf(stat_buf[4], bsize, "%i", (glob_idata->tdesc[index]).wins);
        else
            strncpy(stat_buf[4], "-", bsize);

        for (tindex = 0; tindex < 5; tindex++)
        {
            DrawRect(table_points[tindex],
                     table_top + (index + 1) * table_spacing,
                     table_points[tindex + 1] - table_points[tindex] + 1,
                     table_spacing + 1,
                     BLACK);
            DrawTextRect(table_points[tindex],
                         table_top + (index + 1) * table_spacing,
                         table_points[tindex + 1] - table_points[tindex],
                         table_spacing,
                         stat_buf[tindex],
                         ALIGN_CENTER | VALIGN_MIDDLE);
        }
    }

    SetFont(f_foot, BLACK);
    DrawTextRect(0, screen.height - 50, screen.width, 50,
                 footer_buf, ALIGN_CENTER | VALIGN_MIDDLE);

    CloseFont(f_head);
    CloseFont(f_foot);
    CloseFont(f_body);

    free(buf);
    free(footer_buf);

    PartialUpdate(0, 0, screen.width, screen.height);
}

void game_over_handler_draw()
{
    gmanager_draw(m);
    ifont *f = OpenFont(GFONTB, 16, 1);

    SetFont(f, BLACK);
    DrawTextRect(0, 50, ScreenWidth(), 50,
                 (char *)_("Press any key to continue ..."),
                 ALIGN_CENTER | VALIGN_MIDDLE);

    CloseFont(f);

    FullUpdate();
    FineUpdate();
}
