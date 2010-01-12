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

#include "glang.h"
#include "inkview.h"

/* languages */
#define MSG_COUNT 52

static const char *msg_en[MSG_COUNT] =
{
    /* main screen */
    "Players: %i",
    "Rounds: %i",
    "Max wind speed: %i",
    "Changing wind: Yes",
    "Changing wind: No",
    "Tank marker: Yes",
    "Tank marker: No",
    "Rocks height: %s",
    "Min",
    "Normal",
    "High",
    "Max",
    "Cliffness: %s",
    "Language: English",
    "Help",
    "Start!",
    "Up/Down - select, Left/Right - change,\n"
        "Ok - accept, Back to exit.",
    "Up/Down - select, Left/Right - change,\n"
        "Ok - accept, Return to exit.",
    /* help */
    "Main goal is to hit all other tanks on the land by "
        "adjusting barrel angle and missile power "
        "(initial velocity). Wind speed and direction will "
        "affect missile flight.",
    "Exit or return",
    "Fire, select and change",
    "Clear game screen from tracing paths",
    "Select, change, adjust power (up and down) "
        "and angle (left and right) in the game.",
    "Quick help",
    "Press any key to return ...",
    /* prep screen */
    "Player\'s name",
    "Player setup: player %i of %i",
    "Name: %s",
    "Name: << predefined >>",
    "Type: %s",
    "Tank: ",
    "Continue >>",
    /* game screen */
    "Confirmation",
    "\nAre you shure, you want to exit?",
    "Yes",
    "No",
    "Name",
    "Type",
    "Hits",
    "Wins",
    "Prepare for round %i of %i",
    "Press Ok to start, Back to exit.",
    "Press Ok to start, Return to exit.",
    "Game over! %i of %i rounds",
    "Press Ok to continue.",
    "Statistics",
    "Press any key to continue ...",
    /* ai names */
    "Human",
    "Moron",
    "Shooter",
    "Chooser",
    "Cyborg"
};

static const char *msg_ru[MSG_COUNT] =
{
    /* main screen */
    "Игроки: %i",
    "Раунды: %i",
    "Макс. скорость ветра: %i",
    "Переменный ветер: Да",
    "Переменный ветер: Нет",
    "Маркер танка: Да",
    "Маркер танка: Нет",
    "Высота скал: %s",
    "Минимум",
    "Норма",
    "Высоко",
    "Максимум",
    "Скалистость: %s",
    "Язык: Русский",
    "Помощь",
    "Старт!",
    "Вверх/Вниз - выбор, Вправо/Влево - изменить,\n"
        "Ok - принять, Назад - выход.",
    "Вверх/Вниз - выбор, Вправо/Влево - изменить,\n"
        "Ok - принять, Возврат - выход.",
    /* help */
    "Основная задача - уничтожить все танки,"
        "расположенные на ландшафте, задавая угол наклона орудийного ствола "
        "и силу (начальную скорость) снаряда. Скорость и направление ветра "
        "будут оказывать влияние на полет снаряда.",
    "Выход или возврат",
    "Огонь, выбор и/или изменение",
    "Очистка экрана игры от траекторий полета снарядов",
    "Выбор, изменение, регулировка силы (вверх и вниз) и угла наклона"
        " (вправо и влево) в игре.",
    "Быстрая справка",
    "Нажмите любую клавишу для возврата ...",
    /* prep screen */
    "Имя игрока",
    "Настройка игроков: игрок %i из %i",
    "Имя: %s",
    "Имя: автоматически",
    "Тип: %s",
    "Танк: ",
    "Продолжить >>",
    /* game screen */
    "Подтверждение",
    "\nВы уверены, что хотите выйти?",
    "Да",
    "Нет",
    "Имя",
    "Тип",
    "Попадания",
    "Победы",
    "Приготовтесь к раунду %i из %i",
    "Нажмите Ok для начала, Назад для выхода.",
    "Нажмите Ok для начала, Возврат для выхода.",
    "Игра окончена! Раунд %i из %i.",
    "Нажмите Ok для продолжения.",
    "Статистика",
    "Нажмите любую клавишу для продолжения ...",
    /* ai names */
    "Человек",
    "Полоумный",
    "Последовательный",
    "Избирательный",
    "Киборг"
};


/* internal functions */
const char * get_msg(const char *lang, int index);

const char * _(const char *s)
{
    return (const char *)GetLangText((char *)s);
}

void load_language(const char *lang)
{
    int index;

    LoadLanguage((char *)lang);

    for (index = 0; index < MSG_COUNT; index++)
        AddTranslation((char *)msg_en[index], (char *)get_msg(lang, index));
}

const char * get_msg(const char *lang, int index)
{
    if (index < MSG_COUNT)
    {
        if (strcmp(lang, "ru") == 0)
            return msg_ru[index];
    }

    return msg_en[index];
}
