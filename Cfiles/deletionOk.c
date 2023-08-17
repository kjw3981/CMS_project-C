#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

int main(void)
{
    //データ取得

    char *data;
    long data_len;

    data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    data = malloc(data_len + 1);
    fgets(data, data_len + 1, stdin);
/*
    printf("%s<br>", data);
    char *post_id;
    char *user_id;
    
    user_id = strtok(data, "&");
    post_id = strtok(NULL, "&");
    user_id = strchr(user_id, '=') + 1;
    post_id = strchr(post_id, '=') + 1;
    
    printf("%s<br>", user_id);    
    printf("%s<br>", post_id);
*/
    char *post_id;
    post_id = strtok(data, "&");
    post_id = strchr(post_id, '=') + 1;

    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    
    connect_sql(&conn);

    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    Session session;   
    getSession(conn, &result, &row, session_id, &session);

    // 記事の削除
    char *query = malloc(1000);
    sprintf(query, "DELETE FROM posts WHERE id=%s", post_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));

    }
/*
    printf("content-Type: text/html\n\n");    

    printf("<!DOCTYPE html>");
    printf("<html>");

    printf("<title>削除完了</title>");
    printf("<meta charset=\"UTF-8\">");
    printf("<link href=\"deletionOk.css\" rel=\"stylesheet\" type=\"text/css\">");

    printf("</head>");
    printf("<body>");
    printf("<h1>削除完了</h1>");
    printf("<p>マイページにお戻りいただき、引き続きお楽しみください。</p>");
    printf("<a href=\"http://localhost/cgi-bin/mypage.exe\" class=\"btn-square-shadow\">マイページ</a><br>");

    printf("<a href=\"http://localhost/cgi-bin/top.exe\">ホーム</a><br>");
    printf("<a href=\"http://localhost/cgi-bin/privacy.exe\">プライバシーポリシー</a><br>");
    printf("<a href=\"http://localhost/cgi-bin/comment.exe\">お問い合わせ</a><br>");
    printf("<a href=\"http://localhost/cgi-bin/promise.exe\">利用規約</a><br>");
    printf("</body>");
    printf("</html>");
*/

    printf("Content-type: text/html\r\n");
    printf("Location: mypage.exe\r\n\r\n");

    free(data);
    free(query);

    mysql_free_result(result);
    mysql_close(conn);
    return 0;

}