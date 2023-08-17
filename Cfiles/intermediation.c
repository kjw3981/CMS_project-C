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

int main()
{
    char *session_id = calloc(SESSION_MAX, sizeof(char));
    Session session;
    cookie(session_id);

    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    // mysql初期化
    connect_sql(&conn);
    
    // session_id取得
    getSession(conn, &result, &row, session_id, &session);

    // データの取得/
    char *data;
    long data_len;

    data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    data = malloc(data_len + 1);
    fgets(data, data_len + 1, stdin);
    
    char *post_id;
//    strcpy(post_id , "2");

    // title contents を格納 日本語化
    post_id = strtok(data, "&");
    char *titleStill = strtok(NULL, "&");
    char *contentsStill = strtok(NULL, "&");

    post_id = strchr(post_id, '=') + 1;
    titleStill = strchr(titleStill, '=') + 1;
    contentsStill = strchr(contentsStill, '=') + 1;

    char *ptitle = malloc(strlen(titleStill) + 1);
    char *pcontents = malloc(strlen(contentsStill) + 1);
    urlDecode(titleStill, ptitle);
    urlDecode(contentsStill, pcontents);


    // 記事のタイトル、本文の修正。
    char *query = malloc(10000);
    sprintf(query, "update posts set title = \"%s\", contents = \"%s\", update_date = now() where id = %s;", ptitle, pcontents, post_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
    }



    redirect(post_id);
    mysql_free_result(result);
    mysql_close(conn);
    free(data);
    free(ptitle);
    free (pcontents);
    return 0;
}