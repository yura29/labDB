#include"menuLib.h"

//Вывод меню на экран
void initMenu(database *db){

    char input[256];
    int selected = 0;
    int menu_items_size;

    if(checkConnection(db) != 0)
    {
        printf("Connect to DB\n");
        dbConnect(db);
    }

    menu_item menu_items[] = {
                              {"Show all rows", dbPrint},
                              {"Add row", dbAdd},
                              {"Remove row by number", dbRemove},
                              {"Search row", dbSearch},
                              {"Edit row", dbEdit},
                              {"Sort rows", dbSort},
                              {"Reconnect", dbConnect}
                            };

    menu_items_size = sizeof(menu_items) / sizeof(menu_items[0]);

    for(int i = 0; i < menu_items_size; ++i)
    {
        printf("[%d] %s\n", i+1, menu_items[i].label);
    }

    while(1){
        printf("Enter menu item number: ");
        char *status = fgets(input, 256, stdin);

		if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n'))
			input[strlen(input) - 1] = '\0';

        selected = atoi(input);
        if(status == NULL || selected > menu_items_size || selected < 1){
            printf("Item not found! Try another\n");
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
        printf("Enter path to DB file: ");
		fgets(path, 256, stdin);

		if ((strlen(path) > 0) && (path[strlen(path) - 1] == '\n'))
			path[strlen(path) - 1] = '\0';

        if ((db->content = fopen(path, "r+")) == NULL) 
        {
            printf("Unnable to connect. Try another path\n");
            continue;
        }
        
        strcpy(db->path, path);

        break;
    }
    readRow(db, &db->desc);
    printf("\nConnection successful\n");
    initMenu(db);
}

void dbPrint(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Connect to DB first\n");
        initMenu(db);
    }

    dbSetToBegin(db);

    dbPrintHead(db->desc);
    row dst;
    while(readRow(db, &dst) == 0)
    {
        dbPrintRow(dst, 0);
    }
	printf("\n\n");

    initMenu(db);
}

void dbAdd(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Connect to DB first\n");
        initMenu(db);
    }

    printf("Add new row\n");

    char input[256];
    char **fields = malloc(db->desc.fields_num * sizeof(char));

    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        printf("Enter field \"%s\": ", db->desc.fields[i]);
        fgets(input, 256, stdin);

        if ((strlen(input) > 0) && (input[strlen (input) - 1] == '\n'))
        input[strlen (input) - 1] = '\0';

        fields[i] = malloc(sizeof(char) * (strlen(input) + 256));
        strcpy(fields[i], input);
    }

    row *r = malloc(sizeof(row));
    r->fields = malloc(sizeof(char*) * db->desc.fields_num);
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        r->fields[i] = malloc(sizeof(char) * (strlen(fields[i] + 1)));
        strcpy(r->fields[i], fields[i]);
    }
    r->fields_num = db->desc.fields_num;

    dbWrite(db, *r);

    printf("Row added\n");
    initMenu(db);
}

void dbRemove(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Connect to DB first\n");
        initMenu(db);
    }

    char input[256];
    int line_number;
    printf("Enter row number to delete: ");
	fgets(input, 256, stdin);

	if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n'))
		input[strlen(input) - 1] = '\0';

    line_number = atoi(input) + 1;

    dbRemoveRow(db, line_number);

    initMenu(db);
}

void dbSearch(database *db)
{
    if(checkConnection(db) != 0)
    {
        printf("Connect to DB first\n");
        initMenu(db);
    }

    char input[256];
    printf("Enter search field: ");
	fgets(input, 256, stdin);

	if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n'))
		input[strlen(input) - 1] = '\0';

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
        printf("Field not found. Try another field\n");
        dbSearch(db);
    }

    printf("Enter search phrase: ");
	fgets(input, 256, stdin);

	if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n'))
		input[strlen(input) - 1] = '\0';

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
        printf("Connect to DB first\n");
        initMenu(db);
    }

    char input[256];
    int line_number = -1;
    printf("Enter line number for edit: ");
	fgets(input, 256, stdin);
	if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n'))
		input[strlen(input) - 1] = '\0';

    line_number = atoi(input) + 1;

    char **fields = malloc(db->desc.fields_num * sizeof(char));

    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        printf("Enter field \"%s\": ", db->desc.fields[i]);
        fgets(input, 256, stdin);

        if ((strlen(input) > 0) && (input[strlen (input) - 1] == '\n'))
        input[strlen (input) - 1] = '\0';
        fields[i] = malloc(sizeof(char) * strlen(input));
        strcpy(fields[i], input);
    }

    row *r = malloc(sizeof(row));
    r->fields = malloc(sizeof(char*) * db->desc.fields_num);
    for(int i = 0; i < db->desc.fields_num; ++i)
    {
        r->fields[i] = malloc(sizeof(char) * (strlen(fields[i] + 1)));
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
        printf("Connect to DB first\n");
        initMenu(db);
    }

    char input[256];
    printf("Enter sort field: ");
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
        printf("Field not found. Try another\n");
        dbSort(db);
    }

    dbSortByArg(db, target_field_num);

    initMenu(db);
}
