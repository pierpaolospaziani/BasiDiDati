#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <mysql_time.h>

#include "defines.h"
#include "manager.c"
#include "impiegato.c"
#include "inout.c"
#include "utils.c"
#include "parse.c"

typedef enum {
    IMPIEGATO = 1,
    MANAGER,
    FAILED_LOGIN
} role_t;


struct configuration conf;

static MYSQL *conn;

char var_nome[64];

static role_t attempt_login(MYSQL *conn, char *username, char *password) {
    MYSQL_STMT *login_procedure;
    
    MYSQL_BIND param[4];
    int role = 0;

    if(!setup_prepared_stmt(&login_procedure, "call login(?, ?, ?, ?)", conn)) {
        print_stmt_error(login_procedure, "Unable to initialize login statement\n");
        goto err2;
    }
    
    // Prepare parameters
    memset(param, 0, sizeof(param));
    
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING; // IN
    param[0].buffer = username;
    param[0].buffer_length = strlen(username);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING; // IN
    param[1].buffer = password;
    param[1].buffer_length = strlen(password);

    param[2].buffer_type = MYSQL_TYPE_LONG; // OUT
    param[2].buffer = &role;
    param[2].buffer_length = sizeof(role);
    
    param[3].buffer_type = MYSQL_TYPE_VAR_STRING; // OUT
    param[3].buffer = var_nome;
    param[3].buffer_length = strlen(var_nome);

    if (mysql_stmt_bind_param(login_procedure, param) != 0) { // Note _param
        print_stmt_error(login_procedure, "Could not bind parameters for login");
        goto err;
    }

    // Run procedure
    if (mysql_stmt_execute(login_procedure) != 0) {
        print_stmt_error(login_procedure, "Could not execute login procedure");
        goto err;
    }

    // Prepare output parameters
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_LONG; // OUT
    param[0].buffer = &role;
    param[0].buffer_length = sizeof(role);
    
    param[1].buffer_type = MYSQL_TYPE_VAR_STRING; // OUT
    param[1].buffer = var_nome;
    param[1].buffer_length = 64;
    
    if(mysql_stmt_bind_result(login_procedure, param)) {
        print_stmt_error(login_procedure, "Could not retrieve output parameter");
        goto err;
    }
    
    // Retrieve output parameter
    if(mysql_stmt_fetch(login_procedure)) {
        print_stmt_error(login_procedure, "Could not buffer results");
        goto err;
    }

    mysql_stmt_close(login_procedure);
    return role;

    err:
    mysql_stmt_close(login_procedure);
    err2:
    return FAILED_LOGIN;
}

int main(void) {
    role_t role;

    if(!parse_config("users/login.json", &conf)) {
        fprintf(stderr, "Unable to load login configuration\n");
        exit(EXIT_FAILURE);
    }

    conn = mysql_init (NULL);
    if (conn == NULL) {
        fprintf (stderr, "mysql_init() failed (probably out of memory)\n");
        exit(EXIT_FAILURE);
    }

    if (mysql_real_connect(conn, conf.host, conf.db_username, conf.db_password, conf.database, conf.port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL) {
        fprintf (stderr, "mysql_real_connect() failed\n");
        mysql_close (conn);
        exit(EXIT_FAILURE);
    }
    
    printf("\033[2J\033[H");
    
    printf("██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗\n");
    printf("██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝\n");
    printf("██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗\n");
    printf("██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝\n");
    printf("╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗\n");
    printf(" ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝\n\n");
    printf("████████╗ ██████╗     ██████╗ ██╗   ██╗██████╗\n");
    printf("╚══██╔══╝██╔═══██╗    ██╔══██╗██║   ██║██╔══██╗\n");
    printf("   ██║   ██║   ██║    ██║  ██║██║   ██║██║  ██║\n");
    printf("   ██║   ██║   ██║    ██║  ██║╚██╗ ██╔╝██║  ██║\n");
    printf("   ██║   ╚██████╔╝    ██████╔╝ ╚████╔╝ ██████╔╝\n");
    printf("   ╚═╝    ╚═════╝     ╚═════╝   ╚═══╝  ╚═════╝\n\n");
    printf(" ██████╗███████╗███╗   ██╗████████╗███████╗██████╗\n");
    printf("██╔════╝██╔════╝████╗  ██║╚══██╔══╝██╔════╝██╔══██╗\n");
    printf("██║     █████╗  ██╔██╗ ██║   ██║   █████╗  ██████╔╝\n");
    printf("██║     ██╔══╝  ██║╚██╗██║   ██║   ██╔══╝  ██╔══██╗\n");
    printf("╚██████╗███████╗██║ ╚████║   ██║   ███████╗██║  ██║\n\n");
    
    printf("Tax Code: ");
    getInput(128, conf.username, false);
    printf("Password: ");
    getInput(128, conf.password, true);

    role = attempt_login(conn, conf.username, conf.password);

    switch(role) {
        case MANAGER:
            run_as_manager(conn,var_nome);
            break;
            
        case IMPIEGATO:
            run_as_impiegato(conn,var_nome);
            break;
            
        case FAILED_LOGIN:
            fprintf(stderr, "Invalid credentials\n");
            exit(EXIT_FAILURE);
            break;

        default:
            fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
            abort();
    }

    printf("Bye!\n");

    mysql_close(conn);
    return 0;
}
