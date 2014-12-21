#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>

#define STRING_SIZE 256
#define DB_HOST  ""
#define DB_USER  ""
#define DB_PASS  ""
#define DB_NAME  ""

int stmt_prepare(MYSQL *mysql, MYSQL_STMT *stmt, MYSQL_BIND *param, MYSQL_BIND *results, const char *query, char field_data[][STRING_SIZE], char **result_data) {
    int fields;
    int field_count;
    int row_count;
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
    field_count = mysql_stmt_field_count(stmt);

    if (fields > 0) {
        param = (MYSQL_BIND*)malloc(sizeof(MYSQL_BIND) * fields); // @TODO free
        results = (MYSQL_BIND*)malloc(sizeof(MYSQL_BIND) * field_count); // @TODO free
        field_length = (unsigned long *)malloc(sizeof(unsigned long) * fields); //@TODO free
        data_length = (unsigned long *)malloc(sizeof(unsigned long) * field_count); //@TODO free 
        result_data = malloc(sizeof(char*) * field_count);

        for (int i=0; i<field_count; i++) {
            result_data[i] = (char *) malloc((STRING_SIZE+1) *  sizeof(char));
        }

        if (param != NULL) {
            memset(param, 0, sizeof(MYSQL_BIND) * fields);
        }
        if (results != NULL) {
            memset(results, 0, sizeof(MYSQL_BIND) * field_count);
        }
        if (field_length != NULL) {
            memset(field_length, 0, sizeof(unsigned long) * fields);
        }
        if (data_length != NULL) {
            memset(data_length, 0, sizeof(unsigned long) * field_count);
        }
    
        for (int i=0; i<fields; i++) {
            param[i].buffer_type = MYSQL_TYPE_STRING;
            param[i].buffer = (char *) field_data[i];
            param[i].buffer_length = STRING_SIZE;
            param[i].is_null = 0;
            param[i].length = &field_length[i];
        
            field_length[i] = strlen(field_data[i]); 
        }
    
        for (int i=0; i<field_count; i++) {
            results[i].buffer_type = MYSQL_TYPE_STRING;
            results[i].buffer = result_data[i];
            results[i].buffer_length = STRING_SIZE;
            results[i].is_null = 0;
            results[i].length = &data_length[i];
        }

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

        if (mysql_stmt_store_result(stmt)) {
            fprintf(stderr, " mysql_stmt_store_result() failed\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(EXIT_FAILURE);
        }
   
        row_count = 0;

        while (mysql_stmt_fetch(stmt) == 0) {
            for (int i=0; i<field_count; i++) {
                printf("%s\n", result_data[i]); 
            }
        }

        if (mysql_stmt_close(stmt)) {
            fprintf(stderr, " failed while closing the statement\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(EXIT_FAILURE); 
        }
    }

    free(param);
    free(results);
    free(field_length);
    free(data_length);
    
    return 0;
}

int main() {

    MYSQL *mysql;
    MYSQL_STMT *stmt;
    MYSQL_BIND *param, *results;
    const char *sql;
    char field_data[2][STRING_SIZE];
    char **result_data;

    strncpy(field_data[0], "adrek", STRING_SIZE);
    strncpy(field_data[1], "2", STRING_SIZE);

    mysql = mysql_init(NULL);

    if (mysql_real_connect(mysql, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        fprintf(stderr, "No Connection could be made to the database");
        exit(EXIT_FAILURE);
    } else {
        printf("Connection Established...\n");
        sql = "SELECT `username`, `password`, `email`  FROM Users WHERE username = ? or user_id = ?";
        stmt_prepare(mysql, stmt, param, results, sql, field_data, result_data);
    }

    return 0;
}
