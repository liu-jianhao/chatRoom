#include <stdlib.h>
#include <stdio.h>
#include <mysql.h>



int main() {
    MYSQL *conn_ptr;
    int res;

    conn_ptr = mysql_init(NULL);
    if (!conn_ptr) {
        printf("mysql_init failed\n");
        return EXIT_FAILURE;
    }
    conn_ptr = mysql_real_connect(conn_ptr, "localhost", "root", "liujianhao", "test", 0, NULL, 0);
    if (conn_ptr) {
        res = mysql_query(conn_ptr, "insert into userinfo values('username','mypassword')");   //可以把insert语句替换成delete或者update语句，都一样的
        if (!res) {     //输出受影响的行数
            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
        }  else {       //打印出错误代码及详细信息
            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
        }
    } else {
        printf("Connection failed\n");
    }
    mysql_close(conn_ptr);
    return EXIT_SUCCESS;
}
