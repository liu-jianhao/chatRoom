#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>
int main() {
    MYSQL *conn_ptr;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;
    /* MYSQL_FIELD *fd; */
    int res, i, j;

    conn_ptr = mysql_init(NULL);
    if (!conn_ptr) {
        return EXIT_FAILURE;
    }
    conn_ptr = mysql_real_connect(conn_ptr, "localhost", "root", "liujianhao", "test", 0, NULL, 0);
    if (conn_ptr) {
        char query[100];
        sprintf(query, "select password from userinfo where name = '%s'", "username");
        res = mysql_query(conn_ptr, query); //查询语句
        if (res) {
            printf("SELECT error:%s\n",mysql_error(conn_ptr));
        } else {
            res_ptr = mysql_store_result(conn_ptr);             //取出结果集
            if(res_ptr) {
                /* printf("%lu Rows\n",(unsigned long)mysql_num_rows(res_ptr)); */
                j = mysql_num_fields(res_ptr);
                printf("j = %d\n", j);
                /* if(j == 0) */
                /* { */
                /*     printf("username not found\n"); */
                /* } */
                while((sqlrow = mysql_fetch_row(res_ptr)))  {   //依次取出记录
                    for(i = 0; i < j; i++)
                    {
                        printf("%s\t", sqlrow[i]);              //输出
                    }
                    printf("\n");
                }
                if (mysql_errno(conn_ptr)) {
                    fprintf(stderr,"Retrive error%s\n",mysql_error(conn_ptr));
                }
            }
            mysql_free_result(res_ptr);
        }
    } else {
        printf("Connection failed\n");
    }
    mysql_close(conn_ptr);
    return EXIT_SUCCESS;
}
