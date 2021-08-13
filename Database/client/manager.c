#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

static void add_workshift(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[5];
    char options[3] = {'1','2','3'};
    
    MYSQL_TIME start;
    MYSQL_TIME end;
    int dd,mm,yy;
    char carica[64];
    char centro_c[4];
    int centro;
    char cf[64];
    
    printf("\nEmployee Tax Code: ");
    getInput(63, cf, false);
    
    char op = multiChoice("Cashier, Clerk or Warehouseman? [1/2/3]: ", options, 3);
    switch(op) {
        case '1':
            strcpy(carica,"Cashier");
            break;
        case '2':
            strcpy(carica,"Clerk");
            break;
        case '3':
            strcpy(carica,"Warehouseman");
            break;
        default:
            fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
            abort();
    }
in_date:
    printf("Start Date (dd-mm-yyyy): ");
    scanf("%2d-%2d-%4d",&dd,&mm,&yy);
    fflush(stdin);
    if (date_check(dd,mm,yy) == false){
        printf("\nInvalid date, try again!\n\n");
        goto in_date;
    }
    start.day = dd;
    start.month = mm;
    start.year = yy;
    
fin_date:
    printf("End Date   (dd-mm-yyyy): ");
    scanf("%2d-%2d-%4d",&dd,&mm,&yy);
    fflush(stdin);
    if (date_check(dd,mm,yy) == false){
        printf("\nInvalid date, try again!\n\n");
        goto fin_date;
    }
    end.day = dd;
    end.month = mm;
    end.year = yy;
    
    printf("Center ID: ");
    getInput(4, centro_c, false);
    centro = atoi(centro_c);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_turno(?, ?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize workshift insertion statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_DATE;
    param[0].buffer = &start;
    param[0].buffer_length = sizeof(start);

    param[1].buffer_type = MYSQL_TYPE_DATE;
    param[1].buffer = &end;
    param[1].buffer_length = sizeof(end);

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = carica;
    param[2].buffer_length = strlen(carica);
    
    param[3].buffer_type = MYSQL_TYPE_LONG;
    param[3].buffer = &centro;
    param[3].buffer_length = sizeof(centro);
    
    param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[4].buffer = cf;
    param[4].buffer_length = strlen(cf);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters for add workshift\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while adding the workshift.");
    } else {
        printf("\nWorkshift correctly added!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void show_workshifts(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[2];
    
    MYSQL_TIME var_day;
    int dd,mm,yy;
    char centro_c[4];
    int centro;
    
l_date:
    printf("\nDate (dd-mm-yyyy): ");
    scanf("%2d-%2d-%4d",&dd,&mm,&yy);
    fflush(stdin);
    if (date_check(dd,mm,yy) == false){
        printf("\nInvalid date, try again!\n\n");
        goto l_date;
    }
    var_day.day = dd;
    var_day.month = mm;
    var_day.year = yy;
    
    printf("Center ID: ");
    getInput(4, centro_c, false);
    centro = atoi(centro_c);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call turni_del_giorno(?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize show workshifts statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_DATE;
    param[0].buffer = &var_day;
    param[0].buffer_length = sizeof(var_day);
    
    param[1].buffer_type = MYSQL_TYPE_LONG;
    param[1].buffer = &centro;
    param[1].buffer_length = sizeof(centro);
    
    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters for show workshifts\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred showing the workshifts.");
    }
    
    dump_result_set(conn, prepared_stmt, "\nHere are the employees on duty:");
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void montly_report(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[4];
    
    char cf[64];
    char mm_c[64];
    char yy_c[64];
    int mm;
    int yy;
    int ore;
    
    printf("\nEmployee Tax Code: ");
    getInput(63, cf, false);
    
l_month:
    printf("Month [1-12]:      ");
    getInput(2, mm_c, false);
    if (isNumber(mm_c) == 0){
        printf("Invalid digit!");
        goto l_month;
    }
    mm = atoi(mm_c);
    if (mm < 1 || mm > 12) {
        printf("\nInvalid month, try again!\n\n");
        goto l_month;
    }
    
l_year:
    printf("Year:              ");
    getInput(4, yy_c, false);
    if (isNumber(yy_c) == 0){
        printf("Invalid digit!");
        goto l_year;
    }
    yy = atoi(yy_c);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call report_mensile(?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize monlty report statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = cf;
    param[0].buffer_length = strlen(cf);
    
    param[1].buffer_type = MYSQL_TYPE_LONG;
    param[1].buffer = &mm;
    param[1].buffer_length = sizeof(mm);
    
    param[2].buffer_type = MYSQL_TYPE_LONG;
    param[2].buffer = &yy;
    param[2].buffer_length = sizeof(yy);
    
    param[3].buffer_type = MYSQL_TYPE_LONG;
    param[3].buffer = &ore;
    param[3].buffer_length = sizeof(ore);
    
    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters for montly report\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred showing montly report.");
    }
    
    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &ore;
    param[0].buffer_length = sizeof(ore);
    /*
    if(mysql_stmt_bind_result(prepared_stmt, param)) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve output parameter", true);
    }
    
    if(mysql_stmt_fetch(prepared_stmt)) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not buffer results", true);
    }*/
    
    // mettere if ore lavorate = 0 non stamapre il report
    
    dump_result_set(conn, prepared_stmt, "\nHere is the report:");
    printf("\nThis employee collected %d hour/s of work!\n", ore);
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void annual_report(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[3];
    
    char cf[64];
    int year;
    int var_ore;
    
    printf("\nEmployee Tax Code: ");
    getInput(64, cf, false);
    
    printf("Year:              ");
    scanf("%4d",&year);
    fflush(stdin);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call report_annuale(?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize annual report statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = cf;
    param[0].buffer_length = strlen(cf);
    
    param[1].buffer_type = MYSQL_TYPE_LONG;;
    param[1].buffer = &year;
    param[1].buffer_length = sizeof(year);
    
    param[2].buffer_type = MYSQL_TYPE_LONG;
    param[2].buffer = &var_ore;
    param[2].buffer_length = sizeof(var_ore);
    
    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters for annual report\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred showing annual report.");
    }
    
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &var_ore;
    param[0].buffer_length = sizeof(var_ore);
    /*
    if(mysql_stmt_bind_result(prepared_stmt, param)) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve output parameter", true);
    }
    
    if(mysql_stmt_fetch(prepared_stmt)) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not buffer results", true);
    }*/
    
    // mettere if ore lavorate = 0 non stamapre il report
    
    dump_result_set(conn, prepared_stmt, "\nHere is the report:");
    printf("\nThis employee collected %d hour/s of work!\n", var_ore);
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void add_employee(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[6];
    
    char options[2] = {'1','2'};
    
    char cf[64];
    char nome[64];
    char titolo_studio[64];
    char ruolo[64];
    char telefono_c[10];
    int telefono;
    char pssw[64];
    
    printf("\nEmployee Tax Code: ");
    getInput(64, cf, false);
    printf("Employee Name: ");
    getInput(64, nome, false);
    printf("Password for login: ");
    getInput(64, pssw, false);
    
    char op = multiChoice("Manager or Employee? [1/2]: ", options, 2);
    switch(op) {
        case '1':
            sprintf(ruolo,"Manager");
            break;
        case '2':
            sprintf(ruolo,"Impiegato");
            break;
        default:
            fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
            abort();
    }
    
    printf("Qualification: ");
    getInput(64, titolo_studio, false);
    printf("Telephone number: ");
    getInput(10, telefono_c, false);
    telefono = atoi(telefono_c);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_impiegato(?, ?, ?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize employee insertion statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = cf;
    param[0].buffer_length = strlen(cf);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = nome;
    param[1].buffer_length = strlen(nome);

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = titolo_studio;
    param[2].buffer_length = strlen(titolo_studio);
    
    param[3].buffer_type = MYSQL_TYPE_LONG;
    param[3].buffer = &telefono;
    param[3].buffer_length = sizeof(telefono);
    
    param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[4].buffer = ruolo;
    param[4].buffer_length = strlen(ruolo);
    
    param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[5].buffer = pssw;
    param[5].buffer_length = strlen(pssw);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters for add employee\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while adding the employee.");
    } else {
        printf("\nEmployee correctly added!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void change_person_charge(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[2];
    
    char nome[64];
    char centro_c[4];
    int centro;
    
    printf("Center ID: ");
    getInput(4, centro_c, false);
    centro = atoi(centro_c);
    
    printf("Employee Name: ");
    getInput(64, nome, false);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call cambia_responsabile(?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize change person in charge statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &centro;
    param[0].buffer_length = sizeof(centro);
    
    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = nome;
    param[1].buffer_length = strlen(nome);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to change person in charge\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while changing the person in charge.");
    } else {
        printf("\nThe person in charge has been changed successfully!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void add_film(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[4];
    
    char options[2] = {'1','2'};
    
    char titolo[64];
    char regista[64];
    int anno;
    bool nuovo;
    
    printf("\nFilm Title: ");
    getInput(64, titolo, false);
    printf("Movie Director: ");
    getInput(64, regista, false);
    printf("Production Year: ");
    scanf("%4d",&anno);
    fflush(stdin);
    
    char op = multiChoice("Is a New Film or a Classic one? [1/2]: ", options, 2);
    switch(op) {
        case '1':
            nuovo = true;
            break;
        case '2':
            nuovo = false;
            break;
        default:
            fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
            abort();
    }
    
    if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_film(?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize add film statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = titolo;
    param[0].buffer_length = strlen(titolo);
    
    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = regista;
    param[1].buffer_length = strlen(regista);
    
    param[2].buffer_type = MYSQL_TYPE_LONG;
    param[2].buffer = &anno;
    param[2].buffer_length = sizeof(anno);
    
    param[3].buffer_type = MYSQL_TYPE_TINY;
    param[3].buffer = &nuovo;
    param[3].buffer_length = sizeof(nuovo);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to add the film\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while adding the film.");
    } else {
        printf("\nThe Film has been added successfully!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void add_film_copy(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[5];
    
    char titolo[64];
    char regista[64];
    char centro_c[4];
    int centro;
    char settore_c[4];
    int settore;
    char posizione_c[4];
    int posizione;
    
    printf("\nFilm Title: ");
    getInput(64, titolo, false);
    printf("Movie Director: ");
    getInput(64, regista, false);
    
    printf("Center ID: ");
    getInput(4, centro_c, false);
    centro = atoi(centro_c);
    
    printf("Sector ID: ");
    getInput(4, settore_c, false);
    settore = atoi(settore_c);
    
    printf("Position:  ");
    getInput(4, posizione_c, false);
    posizione = atoi(posizione_c);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_copia(?, ?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize add copy statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = titolo;
    param[0].buffer_length = strlen(titolo);
    
    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = regista;
    param[1].buffer_length = strlen(regista);
    
    param[2].buffer_type = MYSQL_TYPE_LONG;
    param[2].buffer = &centro;
    param[2].buffer_length = sizeof(centro);
    
    param[3].buffer_type = MYSQL_TYPE_LONG;
    param[3].buffer = &settore;
    param[3].buffer_length = sizeof(settore);
    
    param[4].buffer_type = MYSQL_TYPE_LONG;
    param[4].buffer = &posizione;
    param[4].buffer_length = sizeof(posizione);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to add the copy\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while adding the copy.");
    } else {
        printf("\nThe copy has been added successfully!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void qualifies_film(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[2];
    
    char titolo[64];
    char regista[64];
    
    printf("\nFilm Title: ");
    getInput(64, titolo, false);
    printf("Movie Director: ");
    getInput(64, regista, false);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call dequalifica_film(?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize qualifies film statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = titolo;
    param[0].buffer_length = strlen(titolo);
    
    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = regista;
    param[1].buffer_length = strlen(regista);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to qualifies the film\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while qualifying the film.");
    } else {
        printf("\nThe Film has been qualified to Classic successfully!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

void run_as_manager(MYSQL *conn, char* var_nome){
	char options[10] = {'1','2','3','4','5','6','7','8','9','0'};
	char op;

	if(!parse_config("users/manager.json", &conf)) {
		fprintf(stderr, "\nUnable to load manager configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "\nmysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}
    
    printf("\nWelcome back %s!", var_nome);

	while(true) {
        printf("\n\nAVAILABLE COMMANDS FOR MANAGERS:\n");
        printf("-------------------------------------------\n");
        printf("1) Add workshift\n");
		printf("2) Show workshifts of day 'X' of center 'Y'\n");
		printf("3) Monthly report\n");
		printf("4) Annual report\n");
		printf("5) Add new Employee\n");
        printf("6) Change person in charge of the center\n");
        printf("7) Add film\n");
        printf("8) Add a film copy\n");
        printf("9) Qualifies film to classic\n");
        printf("0) QUIT\n");

		op = multiChoice("\nSelect an option", options, 10);

		switch(op) {
			case '1':
				add_workshift(conn);
				break;
			case '2':
				show_workshifts(conn);
				break;
			case '3':
				montly_report(conn);    // ###
				break;
			case '4':
				annual_report(conn);    // ###
				break;
            case '5':
                add_employee(conn);
                break;
            case '6':
                change_person_charge(conn);
                break;
            case '7':
                add_film(conn);
                break;
            case '8':
                add_film_copy(conn);
                break;
            case '9':
                qualifies_film(conn);
                break;
			case '0':
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
	}
}
