#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

int main(void)
{
    
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    connect_sql(&conn);

    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);
    Session session;
    getSession(conn, &result, &row, session_id, &session);
    check_admin(conn, &result, &row, session.user_id);

     printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html>");
    printf("<html lang=\"ja\">");
    printf("<head><meta charset=\"UTF-8\">");
    printf("<title>いしたな</title>");
    printf("<link href=\"administrator.css\" rel=\"stylesheet\" type=\"text/css\"></head>");
    printf("<body><div class=\"box21\">");
    printf("<h2>いしたな　管理者マイページ</h2></div><br>");
    printf("<h3>メニュー</h3><div class=\"sub\">");
    printf("<a href=\"view_users.exe\">ユーザー情報</a><br><a href=\"view_inquiry.exe\">問い合わせ</a><br>");
    printf("<a href=\"top.exe\">トップページ</a><br>");
    printf("</html>");


    mysql_free_result(result);
    mysql_close(conn);
    return 0;
}
