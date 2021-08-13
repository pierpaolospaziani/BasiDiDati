#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "defines.h"

static void available_films(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[1];
    
    char centro_c[4];
    int centro;
    
    printf("\nCenter ID: ");
    getInput(4, centro_c, false);
    centro = atoi(centro_c);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call film_disponibili_noleggio(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize show available films statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &centro;
    param[0].buffer_length = sizeof(centro);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to show available films\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while showing available films.");
    }
    
    dump_result_set(conn, prepared_stmt, "\nHere are the available films:");
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void rent_film(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[8];
    
    char cliente[64];
    MYSQL_TIME restituzione;
    int dd,mm,yy;
    char centro_c[4];
    int centro;
    char film[64];
    char regista[64];
    float costo_noleggio;
    float costo_giornaliero;
    int num_giorni;
    
    printf("\nClient Tax Code: ");
    getInput(64, cliente, false);
    printf("Film: ");
    getInput(64, film, false);
    printf("Movie Director: ");
    getInput(64, regista, false);
    printf("Center ID: ");
    getInput(4, centro_c, false);
    centro = atoi(centro_c);
    
l_date:
    printf("Return date (dd-mm-yyyy): ");
    scanf("%2d-%2d-%4d",&dd,&mm,&yy);
    fflush(stdin);
    if (date_check(dd,mm,yy) == false){
        printf("\nInvalid date, try again!\n\n");
        goto l_date;
    }
    restituzione.day = dd;
    restituzione.month = mm;
    restituzione.year = yy;
    
    if(!setup_prepared_stmt(&prepared_stmt, "call noleggia(?, ?, ?, ?, ?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize rent statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = cliente;
    param[0].buffer_length = strlen(cliente);
    
    param[1].buffer_type = MYSQL_TYPE_DATE;
    param[1].buffer = &restituzione;
    param[1].buffer_length = sizeof(restituzione);
    
    param[2].buffer_type = MYSQL_TYPE_LONG;
    param[2].buffer = &centro;
    param[2].buffer_length = sizeof(centro);
    
    param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[3].buffer = film;
    param[3].buffer_length = strlen(film);
    
    param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[4].buffer = regista;
    param[4].buffer_length = strlen(regista);
    
    param[5].buffer_type = MYSQL_TYPE_LONG;
    param[5].buffer = &num_giorni;
    param[5].buffer_length = sizeof(num_giorni);
    
    param[6].buffer_type = MYSQL_TYPE_FLOAT;
    param[6].buffer = &costo_noleggio;
    param[6].buffer_length = sizeof(costo_noleggio);
    
    param[7].buffer_type = MYSQL_TYPE_FLOAT;
    param[7].buffer = &costo_giornaliero;
    param[7].buffer_length = sizeof(costo_giornaliero);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to rent the film\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while renting the film.");
    }
    
    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &num_giorni;
    param[0].buffer_length = sizeof(num_giorni);
    
    param[1].buffer_type = MYSQL_TYPE_FLOAT;
    param[1].buffer = &costo_noleggio;
    param[1].buffer_length = sizeof(costo_noleggio);
    
    param[2].buffer_type = MYSQL_TYPE_FLOAT;
    param[2].buffer = &costo_giornaliero;
    param[2].buffer_length = sizeof(costo_giornaliero);
    
    if(mysql_stmt_bind_result(prepared_stmt, param)) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve output parameter", true);
    }
    
    if(mysql_stmt_fetch(prepared_stmt)) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not buffer results", true);
    }
    
    printf("\nFilm successfully rented!\n\nThe price is €%.2f for rental and €%.2f per day.\n\nTotal Cost: €%.2f.\n", costo_noleggio, costo_giornaliero, costo_noleggio + (costo_giornaliero * num_giorni));
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void return_film(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[2];
    
    char noleggio_c[4];
    int noleggio;
    int prezzo;
    
    printf("\nRent ID: ");
    getInput(4, noleggio_c, false);
    noleggio = atoi(noleggio_c);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call restituisci_film(?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize return film statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &noleggio;
    param[0].buffer_length = sizeof(noleggio);
    
    param[1].buffer_type = MYSQL_TYPE_LONG;
    param[1].buffer = &prezzo;
    param[1].buffer_length = sizeof(prezzo);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to return film\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while return film.");
    }
    
    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &prezzo;
    param[0].buffer_length = sizeof(prezzo);
    
    if(mysql_stmt_bind_result(prepared_stmt, param)) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve output parameter", true);
    }
    
    if(mysql_stmt_fetch(prepared_stmt)) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not buffer results", true);
    }
    
    if (prezzo != 0) {
        printf("\nRental returned successfully, but the film was supposed to be returned %d day/s ago!\n\nThe fee is €1 for each extra day.\n\nThe client has to pay an additional €%d.\n", prezzo, prezzo);
    } else {
        printf("\nRental returned successfully!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void expired_rentals(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[1];
    
    char centro_c[4];
    int centro;
    
    printf("\nCenter ID: ");
    getInput(4, centro_c, false);
    centro = atoi(centro_c);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call noleggi_scaduti(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize show expired rentals statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &centro;
    param[0].buffer_length = sizeof(centro);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to show expired rentals\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while showing expired rentals.");
    }
    
    dump_result_set(conn, prepared_stmt, "\nHere are the expired rentals:");
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void client_rentals(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[1];
    
    char cliente[64];
    
    printf("\nClient Tax Code: ");
    getInput(64, cliente, false);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call noleggi_cliente(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize show client rentals statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = &cliente;
    param[0].buffer_length = strlen(cliente);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to show client rentals\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while showing client rentals.");
    }
    
    dump_result_set(conn, prepared_stmt, "\nHere are the client rentals:");
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void client_info(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[1];
    int status;
    int i = 0;
    
    char cliente[64];
    
    printf("\nClient Tax Code: ");
    getInput(64, cliente, false);
    
    if(!setup_prepared_stmt(&prepared_stmt, "call info_cliente(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize show client infos statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = &cliente;
    param[0].buffer_length = strlen(cliente);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to show client infos\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while showing client infos.");
    }
    
    do {
        if(conn->server_status & SERVER_PS_OUT_PARAMS) {
            goto next;
        }
        
        if (i == 0){
            dump_result_set(conn, prepared_stmt, "\nHere are the client infos:");
        } else if (i == 1){
            dump_result_set(conn, prepared_stmt, "");
        } else if (i == 2){
            dump_result_set(conn, prepared_stmt, "");
        } else if (i == 3){
            dump_result_set(conn, prepared_stmt, "");
        } else if (i == 4){
            dump_result_set(conn, prepared_stmt, "");
        }
        
        i++;
        
    next:
        status = mysql_stmt_next_result(prepared_stmt);
        if (status > 0){
            finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
        }
    } while (status == 0);
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void add_client(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[4];
    
    char cliente[64];
    char nome[64];
    char cognome[64];
    MYSQL_TIME data_nascita;
    int dd,mm,yy;
    
    printf("\nClient Tax Code: ");
    getInput(64, cliente, false);
    printf("Client Name: ");
    getInput(64, nome, false);
    printf("Client Surname: ");
    getInput(64, cognome, false);
    
l_date:
    printf("Birth Date (dd-mm-yyyy): ");
    scanf("%2d-%2d-%4d",&dd,&mm,&yy);
    fflush(stdin);
    if (date_check(dd,mm,2222) == false){
        printf("\nInvalid date, try again!\n\n");
        goto l_date;
    }
    data_nascita.day = dd;
    data_nascita.month = mm;
    data_nascita.year = yy;
    
    if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_cliente(?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize add client statement\n", false);
    }

    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = &cliente;
    param[0].buffer_length = strlen(cliente);
    
    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = &nome;
    param[1].buffer_length = strlen(nome);
    
    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = &cognome;
    param[2].buffer_length = strlen(cognome);
    
    param[3].buffer_type = MYSQL_TYPE_DATE;
    param[3].buffer = &data_nascita;
    param[3].buffer_length = sizeof(data_nascita);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to add client\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while adding client.");
    } else {
        printf("\nClient correctly added!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

static void modify_client(MYSQL *conn){
    MYSQL_STMT *prepared_stmt;
    MYSQL_BIND param[4];
    char options[8] = {'1','2','3','4','5','6','7','8'};
    
    char cliente[64];
    char numero_c[11];
    int numero_i;
    int centro = 0;
    char via[64];
    char citta[64];
    bool tipo = false;
    
    printf("\nClient Tax Code: ");
    getInput(64, cliente, false);
    
    char op = multiChoice("\n1) Add Cellular Number\n2) Add Telephone Number\n3) Add Address\n4) Add Email\n5) Remove Cellular Number\n6) Remove Telephone Number\n7) Remove Address\n8) Remove Email\n\nWhich info you wanna modify?: ", options, 8);
    switch(op) {
        case '1':
    l_c1:
            printf("\nCellular Number: ");
            getInput(10, numero_c, false);
            if (isNumber(numero_c) == 0){
                printf("Invalid digit!");
                goto l_c1;
            }
            numero_i = atoi(numero_c);
            
            if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_cellulare(?, ?)", conn)) {
                finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize modify client statement\n", false);
            }
            
            memset(param, 0, sizeof(param));
            
            param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[0].buffer = numero_c;
            param[0].buffer_length = strlen(numero_c);
            
            param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[1].buffer = cliente;
            param[1].buffer_length = strlen(cliente);
            
            break;
        case '2':
    l_t1:
            printf("\nTelephone Number: ");
            getInput(10, numero_c, false);
            if (isNumber(numero_c) == 0){
                printf("Invalid digit!");
                goto l_t1;
            }
            numero_i = atoi(numero_c);
            
            if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_telefono(?, ?, ?, ?)", conn)) {
                finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize modify client statement\n", false);
            }
            
            memset(param, 0, sizeof(param));
            
            param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[0].buffer = numero_c;
            param[0].buffer_length = strlen(numero_c);
            
            param[1].buffer_type = MYSQL_TYPE_LONG;
            param[1].buffer = &centro;
            param[1].buffer_length = sizeof(centro);
            
            param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[2].buffer = cliente;
            param[2].buffer_length = strlen(cliente);
            
            param[3].buffer_type = MYSQL_TYPE_TINY;
            param[3].buffer = &tipo;
            param[3].buffer_length = sizeof(tipo);
            
            break;
        case '3':
            printf("\nCity: ");
            getInput(64, citta, false);
            printf("Street: ");
            getInput(64, via, false);
            
            printf("House Number: ");
            getInput(2, numero_c, false);
            numero_i = atoi(numero_c);
            
            if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_indirizzo(?, ?, ?, ?)", conn)) {
                finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize modify client statement\n", false);
            }
            
            memset(param, 0, sizeof(param));
            
            param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[0].buffer = via;
            param[0].buffer_length = strlen(via);
            
            param[1].buffer_type = MYSQL_TYPE_LONG;
            param[1].buffer = &numero_i;
            param[1].buffer_length = sizeof(numero_i);
            
            param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[2].buffer = citta;
            param[2].buffer_length = strlen(citta);
            
            param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[3].buffer = cliente;
            param[3].buffer_length = strlen(cliente);
            
            break;
        case '4':
            printf("\nEmail: ");
            getInput(64, via, false);
            
            if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_email(?, ?, ?, ?)", conn)) {
                finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize modify client statement\n", false);
            }
            
            memset(param, 0, sizeof(param));
            
            param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[0].buffer = via;
            param[0].buffer_length = strlen(via);
            
            param[1].buffer_type = MYSQL_TYPE_LONG;
            param[1].buffer = &centro;
            param[1].buffer_length = sizeof(centro);
            
            param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[2].buffer = cliente;
            param[2].buffer_length = strlen(cliente);
            
            param[3].buffer_type = MYSQL_TYPE_TINY;
            param[3].buffer = &tipo;
            param[3].buffer_length = sizeof(tipo);
            
            break;
        case '5':
    l_c2:
            printf("\nCellular Number: ");
            getInput(10, numero_c, false);
            if (isNumber(numero_c) == 0){
                printf("Invalid digit!");
                goto l_c2;
            }
            numero_i = atoi(numero_c);
            
            if(!setup_prepared_stmt(&prepared_stmt, "call elimina_cellulare(?)", conn)) {
                finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize modify client statement\n", false);
            }
            
            memset(param, 0, sizeof(param));
            
            param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[0].buffer = numero_c;
            param[0].buffer_length = strlen(numero_c);
            
            break;
        case '6':
    l_t2:
            printf("\nTelephone Number: ");
            getInput(10, numero_c, false);
            if (isNumber(numero_c) == 0){
                printf("Invalid digit!");
                goto l_t2;
            }
            numero_i = atoi(numero_c);
            
            if(!setup_prepared_stmt(&prepared_stmt, "call elimina_telefono(?, ?, ?, ?)", conn)) {
                finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize modify client statement\n", false);
            }
            
            memset(param, 0, sizeof(param));
            
            param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[0].buffer = cliente;
            param[0].buffer_length = strlen(cliente);
            
            param[1].buffer_type = MYSQL_TYPE_LONG;
            param[1].buffer = &centro;
            param[1].buffer_length = sizeof(centro);
            
            param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[2].buffer = numero_c;
            param[2].buffer_length = strlen(numero_c);
            
            param[3].buffer_type = MYSQL_TYPE_TINY;
            param[3].buffer = &tipo;
            param[3].buffer_length = sizeof(tipo);
            
            break;
        case '7':
            printf("\nCity: ");
            getInput(64, citta, false);
            printf("\nStreet: ");
            getInput(64, via, false);
            
            printf("\nHouse Number: ");
            getInput(2, numero_c, false);
            numero_i = atoi(numero_c);
            
            
            if(!setup_prepared_stmt(&prepared_stmt, "call elimina_indirizzo(?, ?, ?, ?)", conn)) {
                finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize modify client statement\n", false);
            }
            
            memset(param, 0, sizeof(param));
            
            param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[0].buffer = cliente;
            param[0].buffer_length = strlen(cliente);
            
            param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[1].buffer = via;
            param[1].buffer_length = strlen(via);
            
            param[2].buffer_type = MYSQL_TYPE_LONG;
            param[2].buffer = &numero_i;
            param[2].buffer_length = sizeof(numero_i);
            
            param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[3].buffer = citta;
            param[3].buffer_length = strlen(citta);
            
            break;
        case '8':
            printf("\nEmail: ");
            getInput(64, via, false);
            
            if(!setup_prepared_stmt(&prepared_stmt, "call elimina_email(?, ?, ?, ?)", conn)) {
                finish_with_stmt_error(conn, prepared_stmt, "\nUnable to initialize modify client statement\n", false);
            }
            
            memset(param, 0, sizeof(param));
            
            param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[0].buffer = cliente;
            param[0].buffer_length = strlen(cliente);
            
            param[1].buffer_type = MYSQL_TYPE_LONG;
            param[1].buffer = &centro;
            param[1].buffer_length = sizeof(centro);
            
            param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
            param[2].buffer = via;
            param[2].buffer_length = strlen(via);
            
            param[3].buffer_type = MYSQL_TYPE_TINY;
            param[3].buffer = &tipo;
            param[3].buffer_length = sizeof(tipo);
            
            break;
        default:
            fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
            abort();
    }

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "\nCould not bind parameters to modify client\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error (prepared_stmt, "\nAn error occurred while modifying client.");
    } else {
        printf("\nClient correctly motified!\n");
    }
    
    mysql_stmt_free_result(prepared_stmt);
    for(; mysql_next_result(conn) == 0;)
    mysql_stmt_close(prepared_stmt);
}

void run_as_impiegato(MYSQL *conn, char* var_nome){
    char options[9] = {'1','2','3','4','5','6','7','8','0'};
	char op;
    
    if(!parse_config("users/impiegato.json", &conf)) {
        fprintf(stderr, "Unable to load impiegato configuration\n");
        exit(EXIT_FAILURE);
    }

    if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
        fprintf(stderr, "mysql_change_user() failed\n");
        exit(EXIT_FAILURE);
    }
    
    printf("\nWelcome back %s!", var_nome);

    while(true) {
        printf("\n\nAVAILABLE COMMANDS FOR EMPLOYERS:\n");
        printf("---------------------------------\n");
        printf("1) Show available films\n");
        printf("2) Rent a film\n");
        printf("3) Return film\n");
        printf("4) Show expired rentals\n");
        printf("5) Show client rentals\n");
        printf("6) Client info\n");
        printf("7) Add new client\n");
        printf("8) Modify client info\n");
        printf("0) QUIT\n");

        op = multiChoice("\nSelect an option", options, 9);

        switch(op) {
            case '1':
                available_films(conn);
                break;
            case '2':
                rent_film(conn);
                break;
            case '3':
                return_film(conn);
                break;
            case '4':
                expired_rentals(conn);
                break;
            case '5':
                client_rentals(conn);
                break;
            case '6':
                client_info(conn);
                break;
            case '7':
                add_client(conn);
                break;
            case '8':
                modify_client(conn);
                break;
            case '0':
                return;
            default:
                fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
                abort();
        }
	}
}
