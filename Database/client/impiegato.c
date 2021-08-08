#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#define MAX_DEGREES 128

struct average_grades {
	char corso[46];
	char iscritto[11];
	char concluso[11];
	double avg;
};

static size_t parse_avgs(MYSQL *conn, MYSQL_STMT *stmt, struct average_grades **ret)
{
	int status;
	size_t row = 0;
	MYSQL_BIND param[4];
	bool is_null;

	char corso[46];
	MYSQL_TIME date_iscritto;
	MYSQL_TIME date_concluso;
	double avg;

	if (mysql_stmt_store_result(stmt)) {
		fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(0);
	}

	*ret = malloc(mysql_stmt_num_rows(stmt) * sizeof(struct average_grades));

	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = corso;
	param[0].buffer_length = 46;

	param[1].buffer_type = MYSQL_TYPE_DATE;
	param[1].buffer = &date_iscritto;
	param[1].buffer_length = sizeof(date_iscritto);

	param[2].buffer_type = MYSQL_TYPE_DATE;
	param[2].buffer = &date_concluso;
	param[2].buffer_length = sizeof(date_concluso);
	param[2].is_null = &is_null;

	param[3].buffer_type = MYSQL_TYPE_DOUBLE;
	param[3].buffer = &avg;
	param[3].buffer_length = sizeof(avg);
	
	if(mysql_stmt_bind_result(stmt, param)) {
		finish_with_stmt_error(conn, stmt, "Unable to bind column parameters\n", true);
	}

	/* assemble course general information */
	while (true) {
		status = mysql_stmt_fetch(stmt);

		if (status == 1 || status == MYSQL_NO_DATA)
			break;

		strcpy((*ret)[row].corso, corso);
		sprintf((*ret)[row].iscritto, "%02d/%02d/%4d", date_iscritto.day, date_iscritto.month, date_iscritto.year);
		if(is_null) {
			sprintf((*ret)[row].concluso, "in corso");
		} else {
			sprintf((*ret)[row].concluso, "%02d/%02d/%4d", date_concluso.day, date_concluso.month, date_concluso.year);
		}
		(*ret)[row].avg = avg;

		row++;
	}
				
	return row;
}

static void career_report(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[3];
	int status;
	bool first = true;
	struct average_grades *avgs;
	size_t corsi = 0;
	char header[512];

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call report_carriera(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize career report statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for career report\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while retrieving the career report.");
		goto out;
	}

	// We have multiple result sets here!
	do {
		// Skip OUT variables (although they are not present in the procedure...)
		if(conn->server_status & SERVER_PS_OUT_PARAMS) {
			goto next;
		}

		if(first) {
			parse_avgs(conn, prepared_stmt, &avgs);
			first = false;
		} else {
			sprintf(header, "\nExam grades for the degree course: %s\nStarted on: %s\nCompleted on: %s\nAverage: %.02f\n", avgs[corsi].corso, avgs[corsi].iscritto, avgs[corsi].concluso, avgs[corsi].avg);
			dump_result_set(conn, prepared_stmt, header);
			corsi++;
		}

		// more results? -1 = no, >0 = error, 0 = yes (keep looking)
	    next:
		status = mysql_stmt_next_result(prepared_stmt);
		if (status > 0)
			finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
		
	} while (status == 0);

    out:
	mysql_stmt_close(prepared_stmt);
}

void run_as_impiegato(MYSQL *conn, char* var_nome)
{
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
