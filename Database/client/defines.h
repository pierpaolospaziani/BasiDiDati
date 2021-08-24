#pragma once

#include <stdbool.h>
#include <mysql.h>
#include <ctype.h>

static bool date_check(int dd, int mm, int yy){
    if(yy>=2000 && yy<=9999){
        if(mm>=1 && mm<=12){
            if((dd>=1 && dd<=31) && (mm==1 || mm==3 || mm==5 || mm==7 || mm==8 || mm==10 || mm==12)){
                return true;
            } else if((dd>=1 && dd<=30) && (mm==4 || mm==6 || mm==9 || mm==11)){
                    return true;
            } else if((dd>=1 && dd<=28) && (mm==2)){
                    return true;
            } else if(dd==29 && mm==2 && (yy%400==0 ||(yy%4==0 && yy%100!=0))){
                return true;
            } else{
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

int isNumber(char s[]){
    for (int i = 0; s[i]!= '\0'; i++)    {
        if (isdigit(s[i]) == 0)
              return 0;
    }
    return 1;
}

struct configuration {
	char *host;
	char *db_username;
	char *db_password;
	unsigned int port;
	char *database;

	char username[128];
	char password[128];
};

extern struct configuration conf;

extern int parse_config(char *path, struct configuration *conf);

extern char *getInput(unsigned int lung, char *stringa, bool hide);
extern bool yesOrNo(char *domanda, char yes, char no, bool predef, bool insensitive);
extern char multiChoice(char *domanda, char choices[], int num);

extern void print_error (MYSQL *conn, char *message);
extern void print_stmt_error (MYSQL_STMT *stmt, char *message);
extern void finish_with_error(MYSQL *conn, char *message);
extern void finish_with_stmt_error(MYSQL *conn, MYSQL_STMT *stmt, char *message, bool close_stmt);
extern bool setup_prepared_stmt(MYSQL_STMT **stmt, char *statement, MYSQL *conn);
extern void dump_result_set(MYSQL *conn, MYSQL_STMT *stmt, char *title);

extern void run_as_manager(MYSQL *conn, char* var_nome);
extern void run_as_impiegato(MYSQL *conn, char* var_nome);
