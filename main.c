#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include "stmt_prepare.h"

#define DB_HOST  ""
#define DB_USER  ""
#define DB_PASS  ""
#define DB_NAME  ""

int main() {
    MYSQL *mysql;
    MYSQL_STMT *stmt;
    const char *query;
    char field_data[2][STRING_SIZE];
    unsigned long *field_length, *data_length; 
    char **result_data;

    int num_rows = 0;

    strncpy(field_data[0], "adrek", STRING_SIZE);
    strncpy(field_data[1], "2", STRING_SIZE);

    if (mysql_library_init(0, NULL, NULL)) {
        fprintf(stderr, "could not initialize MySQL library\n");
        return -1;
    } 

    mysql = mysql_init(NULL);
    
    if (mysql_real_connect(mysql, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        fprintf(stderr, "No Connection could be made to the database");
        exit(EXIT_FAILURE);
    } else {
        query =  "SELECT `username`, `password`, `email`, `user_id`  FROM Users WHERE username = ? OR  user_id > ? ";
        if ((stmt = mysql_stmt_init(mysql)) == 0) {
            return -1;
        }

        // Set up prepared statements, memory is dynmically allocated for the parmeter and results buffers
        // These will need to be freed using stmt_free
        if ((stmt_prepare(stmt, query, field_data, &field_length, &result_data, &data_length)) != 0) {
            return -1;
        }
        mysql_stmt_execute(stmt);
        
        // Print results, 1 row at a time
        while (mysql_stmt_fetch(stmt) == 0) {
            for (int i=0; i<mysql_stmt_field_count(stmt); i++) {
                printf("%s\n", result_data[i]); 
            }
            num_rows++;
        }

        // Work around until i can read the mysql_num_rows docs for prepared statements
        if (num_rows == 0) {
            printf("No results found\n");
        }
        
        // Free memory
        mysql_stmt_free_result(stmt);
        stmt_free(&result_data, mysql_stmt_field_count(stmt), &field_length, &data_length);
        mysql_stmt_close(stmt);
       
        mysql_close(mysql);
        mysql_library_end();
    }
}

