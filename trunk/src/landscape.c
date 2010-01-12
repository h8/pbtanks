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

#include <math.h>

#include "tank.h"
#include "landscape.h"

/* internal functions */
int line_calc_y(Point *ps, Point *pe, int x);
int ellipse_calc_y(Point *ps, Point *pe, int x, int type);

Landscape * landscape_new(Size *s)
{
    Landscape *l = malloc(sizeof(Landscape));
    l->points = (ushort *)calloc(s->width, sizeof(ushort));

    l->size = s;

    return l;
}

void landscape_free(Landscape *l)
{
    free(l->points);
    free(l->size);
    free(l);
}

void landscape_set_point(Landscape *l, ushort x, int height)
{
    if (l != NULL)
        if (x < l->size->width)
            l->points[x] = height > 0? height : 0;
}

int landscape_get_point(Landscape *l, ushort x)
{
    if (l != NULL)
        if (x < l->size->width)
            return l->points[x];

    return -1;
}

int landscape_get_max_height_in(Landscape *l, ushort xs, ushort xe)
{
    int index;
    int max = -1;
    int ch;

    for (index = xs; index <= xe; index++)
    {
        ch = landscape_get_point(l, index);
        if (max < ch)
            max = ch;
    }

    return max;
}

int landscape_get_max_height(Landscape *l)
{
    return landscape_get_max_height_in(l, 0, (l->size->width - 1));
}

int landscape_get_min_height_in(Landscape *l, ushort xs, ushort xe)
{
    int index;
    int min = landscape_get_point(l, 0);
    int ch;

    for (index = xs; index <= xe; index++)
    {
        ch = landscape_get_point(l, index);
        if (min > ch)
            min = ch;
    }

    return min;
}

int landscape_get_min_height(Landscape *l)
{
    return landscape_get_min_height_in(l, 0, (l->size->width - 1));
}

void landscape_generate(Landscape *l, LandProps *props)
{
    ushort height_range_max = l->size->height * props->height_diff;
    ushort height_range_min = height_range_max * props->max_min_diff;

    int width_range_max = l->size->width * props->cliffness;
    int width_range_min = width_range_max * props->max_min_diff;

    int index;

    /* init land with random peaks height */
    init_rnd();
    for (index = 0; index < l->size->width; index++)
        landscape_set_point(l, index,
                            rnd_range(height_range_min, height_range_max));

    /* connect peaks */
    index = 0;
    int l_index = 0;
    int w_delta;
    int x, y;
    int line_type, el_type;
    Point p1, p2;
    while (index < (l->size->width - 1))
    {
        w_delta = rnd_range(width_range_min, width_range_max);
        index = l_index + w_delta;
        if (index >= l->size->width)
            index = l->size->width - 1;

        p1.x = l_index;
        p1.y = landscape_get_point(l, l_index);
        p2.x = index;
        p2.y = landscape_get_point(l, index);


        line_type = rnd(3);
        el_type = (p2.x - p1.x) - abs(p1.y - p2.y);

        for (x = l_index; x < index; x++)
        {
            switch (line_type)
            {
            case 1:
                y = line_calc_y(&p1, &p2, x);
                break;
            case 2:
                y = ellipse_calc_y(&p1, &p2, x, el_type);
                break;
            default:
                y = line_calc_y(&p1, &p2, x);
                break;
            }
            landscape_set_point(l, x, y);
        }

        l_index = index;
    }

    /* lift up */
    int max_h = landscape_get_max_height(l);
    if (max_h < (l->size->height * props->max_peak_height)) // if can lift up land
        if (rnd(150) > 100) // lift up
        {
            int up_limit = (l->size->height * props->max_peak_height) - max_h;
            int up_delta = rnd_range(0, up_limit);
            int h;
            for (index = 0; index < l->size->width; index++)
            {
                h = landscape_get_point(l, index) + up_delta;
                landscape_set_point(l, index, h);
            }
        }

    /* dither */
    if (rnd(200) > 150 && (landscape_get_min_height(l) > props->dither_height * 2))
        for (index = 0; index < l->size->width; index++)
            landscape_set_point(l, index,
                                landscape_get_point(l, index) -
                                rnd(props->dither_height));
}

int line_calc_y(Point *ps, Point *pe, int x)
{
    int dx = pe->x - ps->x;
    int dy = pe->y - ps->y;

    return (x - ps->x) * dy / dx + ps->y;
}

int ellipse_calc_y(Point *ps, Point *pe, int x, int type)
{
    int y;
    int a = pe->x - ps->x;
    int b = abs(pe->y - ps->y);

    if (type > 0)
    {
        y = sqrt(abs(pow(b, 2) - pow(a - (x - ps->x), 2)/pow(a, 2)*pow(b, 2)));
        y = b - y;
    }
    else
        y = sqrt(abs(pow(b, 2) - pow(x - ps->x, 2)/pow(a, 2)*pow(b, 2)));

    if (ps->y > pe->y)
        y = pe->y + y;
    else
        y = pe->y - y;

    return y;
}

void landscape_dig_tank_pad(Landscape *l, Point *p)
{
    if (p->x < T_WIDTH)
        p->x = T_WIDTH;

    if (p->x > (l->size->width - T_WIDTH))
        p->x = l->size->width - T_WIDTH;

    int h = landscape_get_max_height_in(l, p->x - T_WIDTH2, p->x + T_WIDTH2);
    int index;
    int peaks = 1;

    while (peaks <= T_PAD_MIN_PEAKS && h > 0)
    {
        for (index = (p->x - T_WIDTH2); index < (p->x + T_WIDTH2); index++)
            if (landscape_get_point(l, index) >= h)
            {
                landscape_set_point(l, index, h);
                peaks++;
            }

        if (peaks < T_PAD_MIN_PEAKS)
        {
            h--;
            peaks = 1;
        }
    }

    p->y = h;
}

void landscape_dig_explosion(Landscape *l, Point *p, int r)
{
    int x, y, yc;
    int delta;
    float a;

    int l_lim, r_lim;

    l_lim = (p->x - r);
    if (l_lim < 0)
        l_lim = 0;

    r_lim = (p->x + r);
    if (r_lim >= l->size->width)
        r_lim = l->size->width - 1;

    for (x = l_lim; x <= r_lim; x++)
    {
        a = acos((x - p->x) / (float)r);
        y = p->y + r * sin(a);
        delta = y - p->y;

        yc = landscape_get_point(l, x);
        if (y <= yc)
                landscape_set_point(l, x, yc - delta * 2);
        else
            if (yc >= (p->y - delta))
                landscape_set_point(l, x, (p->y - delta));
    }
}

int landscape_is_in(Landscape *l, Point *p)
{
    if ((p->x < l->size->width) && (p->x >= 0))
    {
        if (p->y <= landscape_get_point(l, p->x))
            return L_IN;
        else
            return L_NA;
    }

    return L_OUT;
}

void landscape_draw(Landscape *l, int color)
{
    int index;

    if (l != NULL)
        for (index = 0; index < l->size->width; index++)
            DrawLine(index, l->size->height,
                     index, (l->size->height - landscape_get_point(l, index)),
                     color);
}
