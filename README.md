MySQL-Prepared-Statements-Wrapper
=================================

Simple wrapper for using prepared statements using the MySQL C API. 

Requires the MySQL developement libriares

To compile

~~~
gcc stmt_prepare.c $(mysql_config --libs) -std=c99
~~~
