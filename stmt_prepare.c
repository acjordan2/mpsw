#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>

#define STRING_SIZE 256

int stmt_prepare(MYSQL *mysql, MYSQL_STMT *stmt, MYSQL_BIND *param, MYSQL_BIND *results, const char *query, char field_data[][STRING_SIZE], char *result_data) {
    int fields;
    int field_count = mysql_stmt_field_count(stmt);
    unsigned long *field_length;
    unsigned long *data_length;
    param = 0;
    if ((stmt = mysql_stmt_init(mysql)) == 0) {
        return -1;
    }
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        return -1;
    }

    fields = mysql_stmt_param_count(stmt);
    if (fields > 0) {
        param = (MYSQL_BIND*)malloc(sizeof(MYSQL_BIND) * fields); // @TODO free
        results = (MYSQL_BIND*)malloc(sizeof(MYSQL_BIND) * field_count); // @TODO free
        field_length = (unsigned long *)malloc(sizeof(unsigned long) * fields); //@TODO free
        data_length = (unsigned long *)malloc(sizeof(unsigned long) * field_count); //@TODO free 
        memset(param, 0, sizeof(MYSQL_BIND) * fields);
        memset(results, 0, sizeof(MYSQL_BIND) * fields);
        memset(field_length, 0, sizeof(unsigned long) * fields);
        memset(data_length, 0, sizeof(unsigned long) * field_count);
    
        for (int i=0; i<fields; i++) {
            param[i].buffer_type = MYSQL_TYPE_STRING;
            param[i].buffer = (char *) field_data[i];
            param[i].buffer_length = STRING_SIZE;
            param[i].is_null = 0;
            param[i].length = &field_length[i];
        
            field_length[i] = strlen(field_data[i]); 
        }

        printf("%lu", sizeof(data_length));
    
        for (int i=0; i<field_count; i++) {
            results[i].buffer_type = MYSQL_TYPE_STRING;
            results[i].buffer = result_data;
            results[i].buffer_length = STRING_SIZE;
            results[i].is_null = 0;
            results[i].length = &data_length[i];
        }

        printf("results");

        if (mysql_stmt_bind_param(stmt, param)) {
            fprintf(stderr, "mysql_stmt_bind_param() failed\n");
            fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
            exit(EXIT_FAILURE);
        }

        if (mysql_stmt_bind_result(stmt, results) != 0) {
            fprintf(stderr, "mysql_stmt_bind_result() failed\n");
            fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
            exit(EXIT_FAILURE);
        }

        if (mysql_stmt_execute(stmt)) {
            fprintf(stderr, " mysql_stmt_execute(), failed\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(EXIT_FAILURE);
        }
    
        printf("%d\n", field_count); 
        while (mysql_stmt_fetch(stmt) == 0) {
            printf("%s\n", result_data); 
        }

        if (mysql_stmt_close(stmt)) {
            fprintf(stderr, " failed while closing the statement\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(EXIT_FAILURE); 
        }
    }
    
    return 0;
}

int main() {

    MYSQL *mysql;
    MYSQL_STMT *stmt;
    MYSQL_BIND *param, *results;
    const char *sql;
    char field_data[2][STRING_SIZE];
    char result_data[STRING_SIZE];

    strncpy(field_data[0], "andrew", STRING_SIZE);
    strncpy(field_data[1], "2", STRING_SIZE);

    mysql = mysql_init(NULL);

    if (mysql_real_connect(mysql, "localhost", "", "", "", 0, NULL, 0) == NULL) {
        fprintf(stderr, "No Connection could be made to the database");
        exit(EXIT_FAILURE);
    } else {
        printf("Connection Established...\n");
        sql = "SELECT `username`, `password`  FROM Users WHERE username = ? or user_id = ?";
        stmt_prepare(mysql, stmt, param, results, sql, field_data, result_data);
    }

    return 0;
}
