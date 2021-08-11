#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

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

	while(true) {
        printf("\nWelcome back %s!\nTHESE ARE THE AVAILABLE COMMANDS FOR EMPLOYERS: \n\n", var_nome);
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
                //available_films(conn);
                break;
            case '2':
                //rent_film(conn);
                break;
            case '3':
                //return_film(conn);
                break;
            case '4':
                //expired_rentals(conn);
                break;
            case '5':
                //client_rentals(conn);
                break;
            case '6':
                //client_info(conn);
                break;
            case '7':
                //add_client(conn);
                break;
            case '8':
                //modify_client(conn);
                break;
            case '0':
                return;
            default:
                fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
                abort();
        }

		getchar();
	}
}
