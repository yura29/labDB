#include<stdio.h>
#include<string.h>

#include"menuLib.h"

int main(){
	setlocale(0, "cp866");
    //Инициализируем БД
	database db = {.path = ""};
    //Запускаем меню, передавая ему струтуру БД
    initMenu(&db);
    return 0;
}