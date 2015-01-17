#ifndef STMT_PREPARE_H_INCLUDED
#define STMT_PREPARE_H_INCLUDED

#define STRING_SIZE 256

int mpsw_stmt_prepare(MYSQL_STMT *stmt, const char *query, char field_data[][STRING_SIZE], unsigned long **field_length, char ***result_data, unsigned long **data_length);
void mpsw_stmt_free(char ***result_data, int result_data_size,  unsigned long **field_length, unsigned long **data_length); 
#endif
