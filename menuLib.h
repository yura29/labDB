//Библиотека для работы пользователя с БД
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"dbLib.h"

//Структура для пунктов меню. Содержит текст записи и указатель на соответствующую функцию
typedef struct menu_item {
        char label[256];
        void (*functor)(database *db);
} menu_item;

//Вывод меню на экран
void initMenu(database *db);
//Подключение к БД
void dbConnect(database *db);
//Вывод БД на экран в табличном виде
void dbPrint(database *db);
//Добавление записи в БД
void dbAdd(database *db);
//Удаление записи в БД
void dbRemove(database *db);
//Поиск записей в БД
void dbSearch(database *db);
//Редактировнаие записи в БД
void dbEdit(database *db);
//Сотрировка записей БД
void dbSort(database *db);