#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>


/**
 * 日本語に直す
 * ※decoded はフリーしないといけない
 * @param encoded 日本語になる前の記号の文字列
 * @param decoded 日本語に変換した後の文字列を指すポインタ、この関数を呼び出す前に宣言する
 * 
*/
char *urlDecode(const char* encoded, char *decoded) {
    char temp[3] = {0};
    int j = 0;

    for (size_t i = 0; i < strlen(encoded); i++) {
        if (encoded[i] == '%') {
            temp[0] = encoded[i + 1];
            temp[1] = encoded[i + 2];
            decoded[j++] = strtol(temp, NULL, 16);
            i += 2;
        }
        else if (encoded[i] == '+') {
            decoded[j++] = ' ';
        }
        else {
            decoded[j++] = encoded[i];
        }
    }

    decoded[j] = '\0';

    return decoded;
}

int main(void)
{
    printf("content-Type: text/html\n\n");    
    printf("<meta charset=\"UTF-8\">");
 
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    connect_sql(&conn);

    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    Session session;   
    getSession(conn, &result, &row, session_id, &session);
    
    char *data;
    long data_len;

    data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    data = malloc(data_len + 1);
    fgets(data, data_len + 1, stdin);

    char *coments_id = strtok(data, "&");
    char *post_id = strtok(NULL, "&");

    coments_id = strchr(coments_id, '=') + 1;
    
    post_id = strchr(post_id, '=') + 1;
    
    // コメントの取得・表示
    char * query = malloc(1000);
    sprintf(query, "SELECT contents FROM coments WHERE id=%s", coments_id);
    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }
   
    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }
    
    char *edit = malloc(1000);    
    if ((row = mysql_fetch_row(result)) != NULL){        
        strcpy(edit, row[0]);
    }

    printf("<html>");
    printf("<head>");
    printf("<title>コメント追加</title>");
    printf(" <meta charset=\"UTF-8\">");
    printf("<link href=\"correctComment.css\" rel=\"stylesheet\" type=\"text/css\">");

    printf("</head>");
    printf("<body>");
    printf("<h1>コメント再投稿</h1>");
    printf("<form action=\"correctComment.exe\" method=\"post\">");
    printf("<label for=\"coments\">コメント</label>");
    printf("<input class=\"input\" type=\"text\" id=\"coment\" name=\"coment\" value=\"%s\">", edit);
    printf("<h2 class=\"Error\" id=\"comentError\"></h2>");
    printf("<input type=\"hidden\" id=\"id\" name=\"comment_id\" value=\"%s\">", coments_id);
    printf("<input type=\"hidden\" id=\"id\" name=\"post_id\" value=\"%s\">", post_id);

    printf("<input id=\"login-btn\" type=\"submit\" onclick=\"comentInput(event);\" value=\"投稿\">");
    printf("</form>");

    printf("<script>");
    printf("'use strict';");
    symbolErrorInit();
    symbolError("coment");
    printf("</script>");
    printf("</body>");
    printf("</html>");

    free(data);
    free(query);
    free(edit);

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}
