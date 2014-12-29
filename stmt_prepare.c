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

int stmt_prepare(MYSQL_STMT *stmt, const char *query, char field_data[][STRING_SIZE], unsigned long **field_length, char ***result_data, unsigned long **data_length) {
    long unsigned int param_count,
                      field_count;

    MYSQL_BIND *param, 
               *results;

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        return -1;
    }

    param_count = mysql_stmt_param_count(stmt);
    field_count = mysql_stmt_field_count(stmt);

    if (field_count > 0) {
        param = malloc(sizeof(MYSQL_BIND) * param_count);
        results= malloc(sizeof(MYSQL_BIND) * field_count);
        (*field_length) = malloc(sizeof(unsigned long) * param_count);
        (*data_length) = malloc(sizeof(unsigned long) * field_count);

        (*result_data) = malloc(sizeof(char*) * field_count);

        for (int i=0; i<field_count; i++) {
            (*result_data)[i] = (char *)malloc((STRING_SIZE+1) *  sizeof(char));
        }
       

        if (param != NULL) {
            memset(param, 0, sizeof(MYSQL_BIND) * param_count);
        }
        if (results != NULL) {
            memset(results, 0, sizeof(MYSQL_BIND) * field_count);
        }
        if (field_length != NULL) {
            memset((*field_length), 0, sizeof(unsigned long) * param_count);
        }
        if (data_length != NULL) {
            memset((*data_length), 0, sizeof(unsigned long) * field_count);
        }

        for (int i=0; i<param_count; i++) {
            param[i].buffer_type = MYSQL_TYPE_STRING;
            param[i].buffer = (char *) field_data[i];
            param[i].buffer_length = STRING_SIZE;
            param[i].is_null = 0;
            param[i].length = &(*field_length)[i];
            (*field_length)[i] = strlen(field_data[i]); 
        }

        for (int i=0; i<field_count; i++) {
            results[i].buffer_type = MYSQL_TYPE_STRING;
            results[i].buffer = (*result_data)[i];
            results[i].buffer_length = STRING_SIZE;
            results[i].is_null = 0;
            results[i].length = &(*data_length)[i];
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

        free(param);
        free(results);
        return 0;
    }
}

void stmt_free(char ***result_data, int result_data_size,  unsigned long **field_length, unsigned long **data_length) {
    for (int i=0; i<result_data_size; i++) {
        free((*result_data)[i]);
    }
    free(*result_data);
    (*result_data) = NULL;
    free((*field_length));
    (*field_length) = NULL;
    free((*data_length));
    (*data_length) = NULL;
}

int main() {
    MYSQL *mysql;
    MYSQL_STMT *stmt;
    const char *query;
    char field_data[2][STRING_SIZE];
    unsigned long *field_length, *data_length; 
    char **result_data;

    strncpy(field_data[0], "adrek", STRING_SIZE);
    strncpy(field_data[1], "2", STRING_SIZE);

    if (mysql_library_init(0, NULL, NULL)) {
        fprintf(stderr, "could not initialize MySQL library\n");
        exit(EXIT_FAILURE);
    } 

    mysql = mysql_init(NULL);
    
    if (mysql_real_connect(mysql, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        fprintf(stderr, "No Connection could be made to the database");
        exit(EXIT_FAILURE);
    } else {
        query =  "SELECT `username`, `password`, `email`  FROM Users WHERE username = ? OR user_id = ?";
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
        }
        
        // Free memory
        mysql_stmt_free_result(stmt);
        stmt_free(&result_data, mysql_stmt_field_count(stmt), &field_length, &data_length);
        mysql_stmt_close(stmt);
       
        mysql_close(mysql);
        mysql_library_end();
    }
}
