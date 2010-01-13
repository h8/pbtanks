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

#include <math.h>

#include "gmanager.h"

extern const ibitmap battery, angle,
                        stat_c_left, stat_c_right,
                        flug_left, flug_right,
                        arrow_left, arrow_right;

#define LAND_COLOR 5 * 0x111111
#define EXPL_R 20
#define STAT_PANEL_OFFSET 10

/* internal functions */
int get_next_tank(GManager *m);
int check_human_players(GManager *m);
void draw_stat_panel(GManager *m);
void draw_fluger(GManager *m);
void draw_tank_marker(GManager *m);
void generate_wind(GManager *m);
void ai(GManager *m, Tank *t);

GManager * gmanager_new()
{
    GManager *m = malloc(sizeof(GManager));

    m->landscape = landscape_new(size_new(ScreenWidth(), ScreenHeight()));

    m->bqueue = bqueue_new();
    m->tqueue = tqueue_new();

    m->ginitdata = NULL;

    m->f_stat_bar = OpenFont("LiberationSans-Bold", 14, 1);

    return m;
}

void gmanager_free(GManager *m)
{
    landscape_free(m->landscape);

    bqueue_free(m->bqueue);
    tqueue_free(m->tqueue);

    CloseFont(m->f_stat_bar);

    free(m);
}

GInitData * gmanager_set_ginit_data(GManager *m, GInitData *idata)
{
    GInitData *data = m->ginitdata;
    m->ginitdata = idata;
    return data;
}

void gmanager_prepare_game(GManager *m)
{
    landscape_generate(m->landscape, &m->ginitdata->lprops);
    generate_wind(m);

    Tank *t;
    int pos_x;
    int index;
    int side_offset = m->landscape->size->width * 0.1;
    int tdist = (m->landscape->size->width - side_offset * 2) /
                    (m->ginitdata->tcount - 1);
    int tdist_delta = tdist * 0.25;

    tqueue_clear(m->tqueue);
    bqueue_clear(m->bqueue);

    for (index = 0; index < m->ginitdata->tcount; index++)
    {
        t = tank_new();
        t->id = index;
        t->ai = (m->ginitdata->tdesc[index]).ai;
        t->type = (m->ginitdata->tdesc[index]).type;
        strncpy(t->name, (m->ginitdata->tdesc[index]).name, T_NAME_LEN);
        pos_x = side_offset + index * tdist;
        t->pos.x = rnd_range(pos_x - tdist_delta, pos_x + tdist_delta);
        landscape_dig_tank_pad(m->landscape, &t->pos);
        tqueue_add(m->tqueue, t);
    }

    m->ctid = -1;
    m->ginitdata->cround++;
}

int get_next_tank(GManager *m)
{
    int id;
    Tank *t = NULL;

    if (m->tqueue->count > 1)
    {
        id = m->ctid;

        while (t == NULL)
        {
            id++;
            if (id >= m->ginitdata->tcount)
                id = 0;
            t = tqueue_get(m->tqueue, id);
        }

        m->ctid = id;
        return 1; // continue round
    }
    else
        return -1;  // round is over
}

int check_human_players(GManager *m)
{
    Tank *t;

    tqueue_iter_to_head(m->tqueue);
    t = tqueue_iter_next(m->tqueue);
    while (t != NULL)
    {
        if (t->ai == TAIHUMAN)
            return 1;
        t = tqueue_iter_next(m->tqueue);
    }

    return -1;
}

Tank * gmanager_prepare_turn(GManager *m)
{
    Tank *t = NULL;

    if (get_next_tank(m) > 0 && check_human_players(m) > 0)
    {
        t = tqueue_get(m->tqueue, m->ctid);
        if (m->ginitdata->chanwind > 0)
            generate_wind(m);
    }
    else
        /* statistics */
        if (check_human_players(m) > 0)  // last man standing
        {
            tqueue_iter_to_head(m->tqueue);
            t = tqueue_iter_next(m->tqueue);

            (m->ginitdata->tdesc[(int)t->id]).wins++;

            t = NULL;
        }

    return t;
}

void gmanager_do_turn(GManager *m)
{
    float i;
    Point p;
    int shooter_id;
    Tank *t = tqueue_get(m->tqueue, m->ctid);

    if (t != NULL)
    {
        shooter_id = t->id;

        Ballistics *b = malloc(sizeof(Ballistics));
        tank_fill_ballistics_data(t, b);
        b->wind = m->wind;

        for (i = 0; 1; i += 0.01)
        {
            ballistics_get_point_at_t(b, &p, i);

            /* check ground hit */
            if (landscape_is_in(m->landscape, &p) != L_NA)
            {
                b->time = i;
                landscape_dig_explosion(m->landscape, &p, EXPL_R);
                break;
            }

            /* check tank hit */
            tqueue_iter_to_head(m->tqueue);
            t = tqueue_iter_next(m->tqueue);
            while (t != NULL)
            {
                if (tank_is_in(t, &p) > 0)
                {
                    /* statistics */
                    if (t->id != shooter_id)
                        (m->ginitdata->tdesc[shooter_id]).hits++;
                    else
                        (m->ginitdata->tdesc[shooter_id]).hits--; // self-hit

                    tqueue_del(m->tqueue, t->id);
                    break;
                }
                t = tqueue_iter_next(m->tqueue);
            }
        }

        bqueue_add(m->bqueue, b);

        tqueue_iter_to_head(m->tqueue);
        t = tqueue_iter_next(m->tqueue);
        while (t != NULL)
        {
            landscape_dig_tank_pad(m->landscape, &t->pos);
            t = tqueue_iter_next(m->tqueue);
        }
    }
}

int gmanager_do_turns(GManager *m)
{
    Tank *t = gmanager_prepare_turn(m);

    if (t == NULL)  // the end of the round
        return -1;
    else
    {
        while (t->ai != TAIHUMAN)
        {
            ai(m, t);
            gmanager_do_turn(m);
            t = gmanager_prepare_turn(m);
            if (t == NULL)  // if last human tank was hit (during loop)
                return -1;  // or human is last man standing
        }

        return 1;  // tank count > 1 and human player is in the game
    }
}

void gmanager_draw(GManager *m)
{
    int index;
    Tank *t = NULL;
    Ballistics *b = NULL;

    // FIXME: fullupdate here after stat screen
    ClearScreen();

    landscape_draw(m->landscape, LAND_COLOR);

    tqueue_iter_to_head(m->tqueue);
    t = tqueue_iter_next(m->tqueue);
    while (t != NULL)
    {
        tank_draw(t, m->landscape->size->height);
        t = tqueue_iter_next(m->tqueue);
    }

    bqueue_iter_to_head(m->bqueue);
    b = bqueue_iter_next(m->bqueue);
    index = 3; // FIXME: ballistics colors
    while (b != NULL)
    {
        ballistics_draw(b, ++index * 0x111111);
        b = bqueue_iter_next(m->bqueue);
    }

    draw_stat_panel(m);
    draw_fluger(m);
    draw_tank_marker(m);
    DrawRect(0, 0,
                m->landscape->size->width,
                m->landscape->size->height,
                BLACK);

    FullUpdate();
    FineUpdate();
}

void gmanager_change_tank_angle(GManager *m, int dir)
{
    Tank *t = tqueue_get(m->tqueue, m->ctid);

    if (t != NULL)
    {
        tank_change_angle(t, dir);
        tank_redraw(t, m->landscape->size->height);
        draw_stat_panel(m);
    }
}

void gmanager_change_tank_power(GManager *m, int dir)
{
    Tank *t = tqueue_get(m->tqueue, m->ctid);

    if (t != NULL)
    {
        tank_change_power(t, dir);
        tank_redraw(t, m->landscape->size->height);
        draw_stat_panel(m);
    }
}

void draw_stat_panel(GManager *m)
{
    Tank *t = tqueue_get(m->tqueue, m->ctid);
    char buf[200];
    int swidth;
    int points[7];

    if (t != NULL)
    {
        SetFont(m->f_stat_bar, BLACK);
        swidth = StringWidth(t->name);

        points[0] = 15;
        points[1] = points[0] + stat_c_left.width;
        points[2] = points[1] + swidth + 5;
        points[3] = points[2] + angle.width;
        points[4] = points[3] + 30;
        points[5] = points[4] + battery.width;
        points[6] = points[5] + 30;

        /* panel */
        DrawBitmap(points[0], 0, &stat_c_left);
        FillArea(points[1], 1, swidth + 5, stat_c_left.height - 1, LGRAY);
        FillArea(points[2], 1, points[6] - points[2],
                    stat_c_left.height - 1, WHITE);
        DrawLine(points[1], stat_c_left.height - 2, points[6],
                    stat_c_left.height - 2, BLACK);
        DrawLine(points[1], stat_c_left.height - 1, points[6],
                    stat_c_left.height - 1, BLACK);
        DrawLine(points[2], 0, points[2], stat_c_left.height - 1, BLACK);
        DrawBitmap(points[6], 0, &stat_c_right);

        /* name */
        DrawString(points[1], 2, t->name);

        /* angle and power */
        DrawBitmap(points[2], 0, &angle);
        DrawBitmap(points[4], 0, &battery);

        SetFont(m->f_stat_bar, BLACK);

        if (t->angle <= 90)
            sprintf(buf, "%i", t->angle);
        else
            sprintf(buf, "%i", 180 - t->angle);
        DrawString(points[3], 2, buf);

        sprintf(buf, "%i", t->power);
        DrawString(points[5], 2, buf);

        PartialUpdate(points[3], 0, points[6] - points[3], stat_c_left.height);
    }
}

void draw_fluger(GManager *m)
{
    int i;
    int dir = m->wind > 0? 1 : -1;
    int center = m->landscape->size->width -
                    flug_left.width -
                    arrow_left.width * 15;
    ibitmap *b = (ibitmap *)(m->wind > 0? &flug_right : &flug_left);

    DrawBitmap(center, 5, b);

    b = (ibitmap *)(m->wind > 0? &arrow_right : &arrow_left);

    center += flug_left.width / 2 - arrow_left.width / 2;
    for (i = 0; i < abs(m->wind); i++)
    {
        DrawBitmap(center + dir * 20 + i * arrow_left.width * dir,
                    flug_left.height - 7, b);

        if (((i + 1) % 3) == 0 && i > 0)
            center += arrow_left.width * dir;
    }
}

void draw_tank_marker(GManager *m)
{
    Tank *t = tqueue_get(m->tqueue, m->ctid);

    if (t != NULL && m->ginitdata->tmarker)
        DrawLine(t->pos.x,
                    m->landscape->size->height - t->pos.y + 5,
                    t->pos.x,
                    m->landscape->size->height - 5,
                    BLACK);
}

void generate_wind(GManager *m)
{
    m->wind = m->ginitdata->maxwind == 0? 0:rnd_tw(m->ginitdata->maxwind + 1);
}

void gmanager_clear_traj(GManager *m)
{
    bqueue_clear(m->bqueue);
}

void ai(GManager *m, Tank *t)
{
    int x, y, a;
    int id = t->id;
    int i = 0;
    float angle;
    Tank *tt;
    int ids[m->tqueue->count - 1];

    switch (t->ai)
    {
        case TAIMORON:
            t->angle = rnd_range(1, 179);
            t->power = rnd_range(5, T_MAX_POWER);
            break;
        case TAISHOOTER:
            t->angle = t->angle - 5 >= 0? t->angle - 5 : rnd_range(170, 180);
            if (t->angle > 85 && t->angle < 95)
                t->angle = 85;
            t->power = rnd(150) > 120? rnd_range(40, 90) : t->power;
            break;
        case TAICHOOSER:
            x = rnd_range(1, m->landscape->size->width);
            y = landscape_get_point(m->landscape, x);
            a = x > t->pos.x? rnd_range(40, 80) : rnd_range(100, 140);
            angle = deg_to_rad(a);

            t->angle = a;
            t->power = sqrt((4.9 * pow(x - t->pos.x, 2)) /
                            ( pow(cos(angle), 2) *
                            ( t->pos.y -  y + tan(angle) *
                            (x - t->pos.x) ) ));
            break;
        case TAICYBORG:
            tqueue_iter_to_head(m->tqueue);
            tt = tqueue_iter_next(m->tqueue);
            while (tt != NULL)
            {
                if (id  != tt->id)
                    ids[i++] = tt->id;
                tt = tqueue_iter_next(m->tqueue);
            }

            tt = tqueue_get(m->tqueue, ids[rnd(m->tqueue->count - 1) - 1]);
            x = tt->pos.x;

            y = landscape_get_point(m->landscape, x);
            a = x > t->pos.x? rnd_range(40, 80) : rnd_range(100, 140);
            angle = deg_to_rad(a);

            t->angle = a;
            t->power = sqrt((4.9 * pow(x - t->pos.x, 2)) /
                            ( pow(cos(angle), 2) *
                            ( t->pos.y -  y + tan(angle) *
                            (x - t->pos.x) ) ));
            break;
        default:
            break;
    }
}
