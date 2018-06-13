//Вспомогательная библиотека для работы с БД
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Структура записи БД
typedef struct row {
        char **fields;  //Массив со значениями полей
        int fields_num; //Число полей в записи
} row;

//Структура БД
typedef struct database {
        char path[256]; //Путь до файла с БД
        FILE *content;  //Содержимое БД
        row desc;       //Описание полей БД
} database;

//Cчитывание следующй строки из БД
int readRow(database *db, row *dst);
//Запись в конец БД
void dbWrite(database *db, row r);
//Вывод записи БД в табличном формате
void dbPrintRow(row r, int mode);
//Вывод на экран описания полей
void dbPrintHead(row r);
//Установка курсора на первую запись БД
void dbSetToBegin(database *db);

//Проверка подключения к БД
int checkConnection(database *db);
//Поиск записей БД по заданному аргументу и фразе для поиска
int dbFindRowByArg(database *db, int target_field, char *request, row *dst);
//Удаление записи из БД по порядковому номеру
int dbRemoveRow(database *db, int line_number);
//Вставка записи в БД в опредленную строку
int dbInsertRow(database *db, row r, int line_num);
//Сортировка всех записей БД по заданному арументу
int dbSortByArg(database *db, int arg_num);
//Поиск максимальной/минимальной записи по аргументу
int dbFindMinMaxByArg(database *db, int arg_num, int mode, row *dst);