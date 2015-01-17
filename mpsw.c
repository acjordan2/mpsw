#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include "mpsw.h"

int mpsw_stmt_prepare(MYSQL_STMT *stmt, const char *query, char field_data[][STRING_SIZE], unsigned long **field_length, char ***result_data, unsigned long **data_length) {
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
            return -1;
        }

        if (mysql_stmt_bind_result(stmt, results) != 0) {
            return -1;
        }

        free(param);
        free(results);
        return 0;
    }
}

void mpsw_stmt_free(char ***result_data, int result_data_size,  unsigned long **field_length, unsigned long **data_length) {
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

