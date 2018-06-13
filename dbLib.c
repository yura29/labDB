#include"dbLib.h"

//Чтение записи БД
//В случае успеха возвращает 0, иначе 1
int readRow(database *db, row *dst)
{
    char *line = NULL;
    char *tmp;
    int del_pos = -1;
    size_t len = 0;
    //Читаем строку
    int status = getline(&line, &len, db->content);

    //Проверяем пустая она или нет
    if(status == -1) return 1;
    if(strlen(line) == 0) return 1;

    //Считаем количество столбцов
    char *p;
    int count = 0;
    for (p = line; *p; ++p)
    {
        if (*p == '|')
        {
            ++count;
        }
    }
    ++count;
    //Определяем параметры записи и выделяем память под количество аргументов
    dst->fields_num = count;
    dst->fields = malloc(sizeof(char*)*count);

    //Парсим строку и записываем данные в структуру row
    int i=0;
    size_t field_length;
    count = 0;
    for (p = line; *p; ++p)
    {
        if (*p == '|' || *p == '\n')
        {
            field_length = sizeof(char) * (i - del_pos - 1);
            dst->fields[count] = malloc(field_length * sizeof(char));
            memcpy(dst->fields[count], &line[del_pos+1], field_length);
            dst->fields[count][field_length] = '\0';
            del_pos = i;
            ++count;
        }
        ++i;
    }
    
    return 0;
}

//Запись данных в конец БД
void dbWrite(database *db, row r)
{
    //Устанавливаем курсор в конец фаила данных
    fseek(db->content, 0, SEEK_END);
    //Записываем поля row в фаил данных
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        fprintf(db->content, r.fields[i]);
        if(i != db->desc.fields_num -1) fprintf(db->content, "|");
    }
    fprintf(db->content, "\n");

    //Сохраняем изменения
    fclose(db->content);
    db->content = fopen(db->path, "r+");
}

//Устанвливаем курсор на первую запись БД
void dbSetToBegin(database *db)
{
    row tmp;
    //Устанавлиеваем курсор в начало фаила данных
    fseek(db->content, 0, SEEK_SET);
    //Читаем запись с описанием полей
    readRow(db, &tmp);
}

//Проверка подключения к БД
int checkConnection(database *db){
    //Если путь фаила данных неопределен, то соединение не установлено
    if(strlen(db->path) != 0) return 0;
    return 1;
}

//Вывод записи бд на экран
void dbPrintRow(row r, int mode)
{
    for(int i = 0; i < r.fields_num; ++i)
    {
        //Режим 0: выравнивание по левому краю
        if(mode == 0) printf("%-26s |", r.fields[i]);
        //Режим 1: варавнивание по центру
        if(mode == 1) printf("%*s%*s |",13+strlen(r.fields[i])/2,r.fields[i],13-strlen(r.fields[i])/2,"");
    }
    printf("\n");
}

//Вывод на экран описания полей
void dbPrintHead(row r)
{
    int head_length = 0;
    for(int i = 0; i < r.fields_num; ++i)
    {
        printf("%*s%*s |",13+strlen(r.fields[i])/2,r.fields[i],13-strlen(r.fields[i])/2,"");
        head_length += 13 + strlen(r.fields[i])/2 + 13-strlen(r.fields[i])/2 + 2;
    }
    printf("\n");
    //Вывод разделительной черты
    for(int i = 0; i < head_length; ++i) printf("-");
    printf("\n");
}

//Поиск записей по аргументу
int dbFindRowByArg(database *db, int target_field, char *request, row *dst)
{
    row tmp;
    int row_count = 0;
    dbSetToBegin(db);
    //Считаем количество совпадений
    while(readRow(db, &tmp) == 0)
    {
        if(strcmp(request, tmp.fields[target_field]) == 0)
        {
            ++row_count;
        }
    }

    //Выделяем память под найденное количество записей
    dst = malloc(sizeof(row) * row_count);
    dbSetToBegin(db);
    row_count = 0;
    //Читаем все записий и копируем в выходной массив структуры row
    while(readRow(db, &tmp) == 0)
    {
        if(strcmp(request, tmp.fields[target_field]) == 0)
        {
            memcpy(&dst[row_count], &tmp, sizeof(tmp));
            ++row_count;
        }
    }

    //Возвращаем количество найденных записей
    return row_count;
}

//Удаление выбрнной записи
int dbRemoveRow(database *db, int line_number)
{
    char buffer[256];
    int count = 1;
    //Создаем временный фаил данных
    char *tmp_db_path = "dbRemoveRow.tmp";

    FILE *tmpFile = fopen(tmp_db_path, "w");

    fseek(db->content, 0, SEEK_SET);
    //Записываем во временную БД все записи, кроме заданной
    while ((fgets(buffer, 256, db->content)) != NULL)
    {
        if (line_number != count)
            fputs(buffer, tmpFile);

        count++;
    }

    //Удаляем старый фаил данных и заменяем новым
    fclose(db->content);
    fclose(tmpFile);

    remove(db->path);
    rename(tmp_db_path, db->path);
    db->content = fopen(db->path, "r+");

    return 0;
}

//Вставка записи по номеру строки (работает примерно как dbRemoveRow)
int dbInsertRow(database *db, row r, int line_num)
{
    char buffer[1024];
    int count = 1;
    database tdb;
    char *tmp_db_path = "dbInsertRow.tmp";

    if ((tdb.content = fopen(tmp_db_path, "w")) == NULL) 
    {
        printf("Невозможно создать временный фаил\n");
        return 1;
    }
    strcpy(tdb.path, tmp_db_path);
    memcpy(&tdb.desc, &db->desc, sizeof(db->desc)); 

    fseek(db->content, 0, SEEK_SET);
    row tmp;
    while(readRow(db, &tmp) == 0)
    {
        dbWrite(&tdb, tmp);

        if (line_num-1 == count)
            dbWrite(&tdb, r);
        count++;
    }

    fclose(db->content);
    fclose(tdb.content);

    remove(db->path);
    rename(tmp_db_path, db->path);
    db->content = fopen(db->path, "r+");

    return 0;
}

//Сортировка БД по полю
int dbSortByArg(database *db, int arg_num)
{
    //Cоздаем временную БД и подключаемся к ней
    database tdb;
    char *tmp_db_path = "dbSort.tmp";

    if ((tdb.content = fopen(tmp_db_path, "w")) == NULL) 
    {
        printf("Невозможно создать временный фаил\n");
        return 1;
    }
    strcpy(tdb.path, tmp_db_path);
    memcpy(&tdb.desc, &db->desc, sizeof(db->desc));
    dbWrite(&tdb, db->desc);

    //Методом пузырька сортируем данные, записывая их во временную БД и удаляя их из старой
    row tmp;
    int line_to_remove;  
    while(line_to_remove = dbFindMinMaxByArg(db, arg_num, 0, &tmp), line_to_remove != 0)
    {
        dbRemoveRow(db, line_to_remove+1);
        dbWrite(&tdb, tmp);
    }

    //Меняем фаил данных на новый
    fclose(db->content);
    fclose(tdb.content);

    remove(db->path);
    rename(tmp_db_path, db->path);
    db->content = fopen(db->path, "r+");

    return 0;
}

//Поиск минимальной(mode=0)/минимальной(mode!=0) записи по заданному полю
int dbFindMinMaxByArg(database *db, int arg_num, int mode, row *dst)
{
    int res = 0;
    row tmp;
    int cmp = (mode == 0) ? 1 : -1;

    //Устанавливаем курсор на первую запись
    dbSetToBegin(db);

    //Пытаемся прочитать начальный элемент
    if(readRow(db, dst) == 0)
    {
        res = 1;
    }
    else
    {
        //В слчае, если БД пуста, то возвращаем 0
        return res;
    }

    //Читаем все последующие элементы и сравниваем их с начальным
    int count = res + 1;
    while(readRow(db, &tmp) == 0)
    {
        if(strcmp(dst->fields[arg_num], tmp.fields[arg_num]) * cmp > 0)
        {
            dst->fields_num = db->desc.fields_num;

            for(int i = 0; i < db->desc.fields_num; ++i)
            {
                ++i;
                --i;
                dst->fields[i] = malloc(strlen(tmp.fields[i]) * sizeof(char));
                strcpy(dst->fields[i], tmp.fields[i]);
            }
            res = count;
        }
        
        ++count;
    }

    //Возвращаем номер строки
    return res;
}