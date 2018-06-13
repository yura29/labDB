#include"menuLib.h"

//Вывод меню на экран
void initMenu(database *db){

    char input[256];
    int selected = 0;
    int status = 0;
    int menu_items_size;

    if(checkConnection(db) != 0)
    {
        printf("Подключение к БД\n");
        dbConnect(db);
    }

    menu_item menu_items[] = {
                              {"Вывести все запимси", dbPrint},
                              {"Добавить запись", dbAdd},
                              {"Удалить запись по ID", dbRemove},
                              {"Поиск записи по полю", dbSearch},
                              {"Редактирование записи", dbEdit},
                              {"Сортрировка данных по полю", dbSort},
                              {"Переподключиться к бд", dbConnect}
                            };

    menu_items_size = sizeof(menu_items) / sizeof(menu_items[0]);

    for(int i = 0; i < menu_items_size; ++i)
    {
        printf("[%d] %s\n", i+1, menu_items[i].label);
    }

    while(1){
        printf("Введите номер пунка меню: ");
        status = scanf(" %s", &input);
        selected = atoi(input);
        if(status == 0 || selected > menu_items_size || selected < 1){
            printf("Неверное значение! Попробуйте ввести другое\n");
            continue;
        }

        (menu_items[selected-1].functor)(db);
        break;
    }
}

void dbConnect(database *db)
{
    char path[255];

    while(1)
    {
        printf("Укажите адрес файла БД: ");
        scanf("%s", path);
        if ((db->content = fopen(path, "r+")) == NULL) 
        {
            printf("Невозможно подключиться. Попробуйте другой путь\n");
            continue;
        }
        
        strcpy(db->path, path);

        break;
    }
    readRow(db, &db->desc);
    printf("\nПодключение выполненно успешно\n");
    initMenu(db);
}

void dbPrint(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Сначала необходимо подключиться к бд\n");
        initMenu(db);
    }

    dbSetToBegin(db);

    dbPrintHead(db->desc);
    row dst;
    while(readRow(db, &dst) == 0)
    {
        dbPrintRow(dst, 0);
    }

    initMenu(db);
}

void dbAdd(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Сначала необходимо подключиться к бд\n");
        initMenu(db);
    }

    printf("Добавление новой записи\n");

    char input[256];
    char *fields[db->desc.fields_num];

    getchar();
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        printf("Введите поле \"%s\": ", db->desc.fields[i]);
        fgets(input, 256, stdin);

        if ((strlen(input) > 0) && (input[strlen (input) - 1] == '\n'))
        input[strlen (input) - 1] = '\0';

        fields[i] = malloc(sizeof(char) * strlen(input));
        strcpy(fields[i], input);
    }

    row *r = malloc(sizeof(row));
    r->fields = malloc(sizeof(fields));
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        r->fields[i] = malloc(sizeof(fields[i]));
        strcpy(r->fields[i], fields[i]);
    }
    r->fields_num = db->desc.fields_num;

    dbWrite(db, *r);
    
    for(int i = 0; i < db->desc.fields_num; ++i) free(fields[i]);

    printf("Запись добавлена\n");
    initMenu(db);
}

void dbRemove(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Сначала необходимо подключиться к бд\n");
        initMenu(db);
    }

    char input[256];
    int line_number;
    printf("Укажите номер записи для удаления: ");
    scanf(" %s", &input);
    line_number = atoi(input) + 1;

    dbRemoveRow(db, line_number);

    initMenu(db);
}

void dbSearch(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Сначала необходимо подключиться к бд\n");
        initMenu(db);
    }

    char input[256];
    printf("Введите поле для поиска: ");
    scanf(" %s", &input);

    int target_field_num = -1;
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        if(strcmp(input, db->desc.fields[i]) == 0)
        {
            target_field_num = i;
            break;
        }
    }

    if(target_field_num == -1)
    {
        printf("Заданное поле не найдено. Попробуйте ввести другое\n");
        dbSearch(db);
    }

    printf("Введите фразу для поиска: ");
    scanf(" %s", &input);

    row tmp;
    int row_count = 0;
    dbSetToBegin(db);
    dbPrintHead(db->desc);
    while(readRow(db, &tmp) == 0)
    {
        if(strcmp(input, tmp.fields[target_field_num]) == 0)
        {
            dbPrintRow(tmp, 0);
        }
    }

    initMenu(db);
}

void dbEdit(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Сначала необходимо подключиться к бд\n");
        initMenu(db);
    }

    char input[256];
    int line_number = -1;
    printf("Введите номер строки для редактирования: ");
    scanf(" %s", &input);

    line_number = atoi(input) + 1;

    char *fields[db->desc.fields_num];

    getchar();
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        printf("Введите поле \"%s\": ", db->desc.fields[i]);
        fgets(input, 256, stdin);

        if ((strlen(input) > 0) && (input[strlen (input) - 1] == '\n'))
        input[strlen (input) - 1] = '\0';
        fields[i] = malloc(sizeof(char) * strlen(input));
        strcpy(fields[i], input);
    }

    row *r = malloc(sizeof(row));
    r->fields = malloc(sizeof(fields));
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        r->fields[i] = malloc(sizeof(fields[i]));
        strcpy(r->fields[i], fields[i]);
    }
    r->fields_num = db->desc.fields_num;

    dbRemoveRow(db, line_number);
    dbInsertRow(db, *r, line_number);

    initMenu(db);
}

void dbSort(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Сначала необходимо подключиться к бд\n");
        initMenu(db);
    }

    char input[256];
    printf("Введите поле для сортировки: ");
    getchar();
    fgets(input, 256, stdin);

    if ((strlen(input) > 0) && (input[strlen (input) - 1] == '\n'))
    input[strlen (input) - 1] = '\0';

    int target_field_num = -1;
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        if(strcmp(input, db->desc.fields[i]) == 0)
        {
            target_field_num = i;
            break;
        }
    }

    if(target_field_num == -1)
    {
        printf("Заданное поле не найдено. Попробуйте ввести другое\n");
        dbSort(db);
    }

    dbSortByArg(db, target_field_num);

    initMenu(db);
}
