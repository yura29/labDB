#include<stdio.h>
#include<string.h>

#include"menuLib.h"

int main(){
    //Инициализируем БД
    database db;
    //Запускаем меню, передавая ему струтуру БД
    initMenu(&db);
    return 0;
}