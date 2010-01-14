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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <dlfcn.h>

#include "main.h"
#include "gamethread.h"
#include "prepthread.h"
#include "helpthread.h"
#include "gutils.h"
#include "glang.h"

extern const ibitmap main_background;

#define MENU_PLAYERS 0
#define MENU_ROUNDS 1
#define MENU_MAXWIND 2
#define MENU_CHANWIND 3
#define MENU_TMARKER 4
#define MENU_HDIFF 5
#define MENU_CLIFF 6
#define MENU_LANG 7
#define MENU_HELP 8
#define MENU_LOADGAME 9
#define MENU_START 10

#define MAX_PLAYERS 10
#define MAX_ROUNDS 20

#define LANG_C 3
#define LANG_M LANG_C - 1

#define HDIFF_C 4
#define HDIFF_M HDIFF_C - 1

#define CLIFF_C 4
#define CLIFF_M CLIFF_C - 1

#define CONFIGNAME "pbtanks.cfg"

/* internal vars */
int active_item;
int rounds;
int players;
int maxwind;
int chanwind;
int tmarker;
int hdiff;
int cliff;

int lang_index;
static const char *lang[LANG_C] = {"en", "ru", "ua"};
char *cfg_path;

static iconfigedit cfg[] =
{
    {CFG_INDEX, NULL, "2", NULL, "players", "2", NULL, NULL},
    {CFG_INDEX, NULL, "10", NULL, "rounds", "10", NULL, NULL},
    {CFG_INDEX, NULL, "2", NULL, "maxwind", "2", NULL, NULL},
    {CFG_INDEX, NULL, "1", NULL, "chanwind", "1", NULL, NULL},
    {CFG_INDEX, NULL, "1", NULL, "marker", "1", NULL, NULL},
    {CFG_INDEX, NULL, "2", NULL, "hdiff", "2", NULL, NULL},
    {CFG_INDEX, NULL, "0", NULL, "cliff", "0", NULL, NULL},
    {CFG_INDEX, NULL, "0", NULL, "lang", "0", NULL, NULL},
    {0, NULL, NULL, NULL, NULL, NULL, NULL}

};

/* internal functions */
void create_idata();
void draw_main_screen();
int get_book_model();
void load_config();
void save_config();
void set_hotkeys();
int load_saved_game();

int main_handler(int type, int key, int rtime)
{
    if (type == EVT_SHOW)
    {
        active_item = MENU_START;
        draw_main_screen();
    }

    if (type == EVT_INIT)
    {
        set_hotkeys();
        load_config();
        load_language(lang[lang_index]);
        SetOrientation(2);
    }

    if (type == EVT_KEYPRESS)
    {
        if (key == glob_back_key)
        {
            save_config();
            CloseApp();
        }

        switch (key)
        {
        case KEY_OK:
            switch (active_item)
            {
            case MENU_CHANWIND:
                chanwind = chanwind == 1? 0 : 1;
                break;
            case MENU_LOADGAME:
                if (load_saved_game())
                    SetEventHandler(game_handler);
                break;
            case MENU_START:
                create_idata();
                SetEventHandler(prep_handler);
                break;
            case MENU_HELP:
                SetEventHandler(help_handler);
                break;
            case MENU_TMARKER:
                tmarker = tmarker == 1? 0 : 1;
                break;
            }
            break;

        case KEY_DOWN:
            active_item = active_item + 1 > MENU_START? 0 : active_item + 1;
            break;
        case KEY_UP:
            active_item = active_item - 1 >= 0? active_item - 1 : MENU_START;
            break;

        case KEY_LEFT:
            switch (active_item)
            {
            case MENU_PLAYERS:
                players = players > 2? players - 1 : MAX_PLAYERS;
                break;
            case MENU_ROUNDS:
                rounds = rounds > 2? rounds - 1 : MAX_ROUNDS;
                break;
            case MENU_MAXWIND:
                maxwind = maxwind > 0? maxwind - 1 : 10;
                break;
            case MENU_CHANWIND:
                chanwind = chanwind == 1? 0 : 1;
                break;
            case MENU_TMARKER:
                tmarker = tmarker == 1? 0 : 1;
                break;
            case MENU_HDIFF:
            	hdiff = hdiff > 0? hdiff - 1 : HDIFF_M;
            	break;
            case MENU_CLIFF:
            	cliff = cliff > 0? cliff - 1 : CLIFF_M;
            	break;
            case MENU_LANG:
                lang_index = lang_index > 0? lang_index - 1 : LANG_M;
                load_language(lang[lang_index]);
                break;
            }
            break;
        case KEY_RIGHT:
            switch (active_item)
            {
            case MENU_PLAYERS:
                players = players < MAX_PLAYERS? players + 1 : 2;
                break;
            case MENU_ROUNDS:
                rounds = rounds < MAX_ROUNDS? rounds + 1 : 2;
                break;
            case MENU_MAXWIND:
                maxwind = maxwind < 10? maxwind + 1 : 0;
                break;
            case MENU_CHANWIND:
                chanwind = chanwind == 1? 0 : 1;
                break;
            case MENU_TMARKER:
                tmarker = tmarker == 1? 0 : 1;
                break;
            case MENU_HDIFF:
            	hdiff = hdiff < HDIFF_M? hdiff + 1 : 0;
            	break;
            case MENU_CLIFF:
            	cliff = cliff < CLIFF_M? cliff + 1 : 0;
            	break;
            case MENU_LANG:
                lang_index = lang_index < LANG_M? lang_index + 1 : 0;
                load_language(lang[lang_index]);
                break;
            }
            break;
        }

        draw_main_screen();
    }

    return 0;
}

void create_idata()
{
	float hdiffs[HDIFF_C] = {0.2, 0.4, 0.6, 0.8};
	float cliffs[CLIFF_C] = {0.2, 0.4, 0.6, 0.8};

    glob_idata = malloc(sizeof(GInitData));
    glob_idata->maxwind = maxwind;
    glob_idata->chanwind = chanwind;
    glob_idata->tmarker = tmarker;
    glob_idata->rounds = rounds;
    glob_idata->cround = 0;
    glob_idata->tcount = players;
    glob_idata->tdesc = calloc(glob_idata->tcount, sizeof(TankDesc));

    glob_idata->lprops = DEF_LANDPROPS;
    glob_idata->lprops.height_diff = hdiffs[hdiff];
    glob_idata->lprops.cliffness = cliffs[cliff];
}

void main_handler_ret()
{
    free(glob_idata->tdesc);
    free(glob_idata);
    SetEventHandler(main_handler);
}

void draw_main_screen()
{
    const int side_offset = 50;
    const int items_width = 220;
    const int bsize = 150;
    char *buf = calloc(bsize, sizeof(char));
    const Size screen = {ScreenWidth(), ScreenHeight()};

    irect players_rect = {side_offset, 50, items_width, 30, MENU_PLAYERS};
    irect rounds_rect = {side_offset, 95, items_width, 30, MENU_ROUNDS};
    irect windm_rect = {side_offset, 140, items_width, 30, MENU_MAXWIND};
    irect windc_rect = {side_offset, 185, items_width, 30, MENU_CHANWIND};
    irect tmark_rect = {side_offset, 230, items_width, 30, MENU_TMARKER};
    irect hdiff_rect = {side_offset, 275, items_width, 30, MENU_HDIFF};
    irect cliff_rect = {side_offset, 320, items_width, 30, MENU_CLIFF};
    irect lang_rect = {side_offset, 365, items_width, 30, MENU_LANG};
    irect help_rect = {side_offset, 410, items_width, 30, MENU_HELP};
    irect load_rect = {side_offset, 455, items_width, 30, MENU_LOADGAME};
    irect start_rect = {side_offset, 500, items_width, 30, MENU_START};

    const char *hdiff_sr[HDIFF_C] = {_("Min"), _("Normal"),
    									_("High"), _("Max")};
    const char *cliff_sr[HDIFF_C] = {_("Max"), _("High"),
    									_("Normal"), _("Min")};

    ifont *f = OpenFont(GFONTB, 14, 1);


    DrawBitmap(0, 0, &main_background);
    DrawRect(0, 0, screen.width, screen.height, BLACK);
    DrawRect(2, 2, screen.width - 4, screen.height - 4, BLACK);

    /* menu items */
    snprintf(buf, bsize, _("Players: %i"), players);
    draw_menu_rect(&players_rect, f, buf, VALIGN_MIDDLE, active_item);

    snprintf(buf, bsize, _("Rounds: %i"), rounds);
    draw_menu_rect(&rounds_rect, f, buf, VALIGN_MIDDLE, active_item);

    snprintf(buf, bsize, _("Max wind speed: %i"), maxwind);
    draw_menu_rect(&windm_rect, f, buf, VALIGN_MIDDLE, active_item);

    if (chanwind)
        strncpy(buf, _("Changing wind: Yes"), bsize);
    else
        strncpy(buf, _("Changing wind: No"), bsize);
    draw_menu_rect(&windc_rect, f, buf, VALIGN_MIDDLE, active_item);

    if (tmarker)
        strncpy(buf, _("Tank marker: Yes"), bsize);
    else
        strncpy(buf, _("Tank marker: No"), bsize);
    draw_menu_rect(&tmark_rect, f, buf, VALIGN_MIDDLE, active_item);

    snprintf(buf, bsize, _("Rocks height: %s"), hdiff_sr[hdiff]);
    draw_menu_rect(&hdiff_rect, f, buf, VALIGN_MIDDLE, active_item);

    snprintf(buf, bsize, _("Cliffness: %s"), cliff_sr[cliff]);
    draw_menu_rect(&cliff_rect, f, buf, VALIGN_MIDDLE, active_item);

    draw_menu_rect(&lang_rect, f, _("Language: English"),
                    VALIGN_MIDDLE, active_item);

    draw_menu_rect(&help_rect, f, _("Help"), VALIGN_MIDDLE, active_item);

    draw_menu_rect(&load_rect, f, _("Load game"), VALIGN_MIDDLE, active_item);

    draw_menu_rect(&start_rect, f, _("Start!"), VALIGN_MIDDLE, active_item);

    /* footer */
    if (glob_back_key == KEY_PREV)
        snprintf(buf, bsize, _("Up/Down - select, Left/Right - change,\n"
                                "Ok - accept, Back to exit."));
    else
        snprintf(buf, bsize, _("Up/Down - select, Left/Right - change,\n"
                                "Ok - accept, Return to exit."));
    DrawTextRect(0, screen.height - 50, screen.width, 50, buf,
                    ALIGN_CENTER | VALIGN_MIDDLE);

    CloseFont(f);
    free(buf);

    PartialUpdate(0, 0, screen.width, screen.height);
}

int get_book_model()
{
    const char *s = GetDeviceModel();
    int mod = 301;

    if (strstr(s, "301"))
        mod = 301;
    if (strstr(s, "101"))
        mod = 360;

    return mod;
}

void set_hotkeys()
{
    glob_book_model = get_book_model();
    glob_back_key = glob_book_model == 301? KEY_BACK : KEY_PREV;
    glob_clean_key = glob_book_model == 301? KEY_DELETE : KEY_NEXT;
}

void load_config()
{
    int path_len = strlen(CONFIGPATH) + strlen(CONFIGNAME) + strlen("/") + 1;
    cfg_path = calloc(path_len, sizeof(char));  // free in save_config

    snprintf(cfg_path, path_len, "%s/%s", CONFIGPATH, CONFIGNAME);
    iconfig *c = OpenConfig(cfg_path, cfg);

    players = ReadInt(c, "players", 2);
    rounds = ReadInt(c, "rounds", 10);
    maxwind = ReadInt(c, "maxwind", 2);
    chanwind = ReadInt(c, "chanwind", 1);
    tmarker = ReadInt(c, "marker", 1);
    lang_index = ReadInt(c, "lang", 0);
    hdiff = ReadInt(c, "hdiff", 2);
    cliff = ReadInt(c, "cliff", 0);

    CloseConfig(c);
}

void save_config()
{
    iconfig *c = OpenConfig(cfg_path, cfg);

    WriteInt(c, "players", players);
    WriteInt(c, "rounds", rounds);
    WriteInt(c, "maxwind", maxwind);
    WriteInt(c, "chanwind", chanwind);
    WriteInt(c, "marker", tmarker);
    WriteInt(c, "hdiff", hdiff);
    WriteInt(c, "cliff", cliff);
    WriteInt(c, "lang", lang_index);

    SaveConfig(c);
    CloseConfig(c);

    free(cfg_path);
}

int load_saved_game()
{
    int index;
    size_t dr;
    FILE *f = NULL;
    GInitData *gid = NULL;
    int path_len = strlen(CONFIGPATH) + strlen(SAVENAME) + strlen("/") + 1;
    char *save_path = calloc(path_len, sizeof(char));

    snprintf(save_path, path_len, "%s/%s", CONFIGPATH, SAVENAME);

    f = fopen(save_path, "rb");
    if (f != NULL)
    {
        gid = malloc(sizeof(GInitData));
        dr = fread(gid, sizeof(GInitData), 1, f);

        if (dr)
        {
            gid->tdesc = calloc(gid->tcount, sizeof(TankDesc));

            for (index = 0; index < gid->tcount; index++)
            {
                dr = fread(&gid->tdesc[index], sizeof(TankDesc), 1, f);
                if (!dr)  // if error reading file
                {
                    free(gid->tdesc);
                    free(gid);
                    gid = NULL;
                    break;
                }
            }
        }

        fclose(f);
    }

    free(save_path);

    glob_idata = gid;

    return gid != NULL? 1 : 0;
}

int main(int argc, char **argv)
{
    InkViewMain(main_handler);

    return 0;
}

