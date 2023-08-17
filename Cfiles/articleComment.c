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
char *urlDecode(char* encoded, char *decoded) {
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
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    connect_sql(&conn);

    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    Session session;   
    getSession(conn, &result, &row, session_id, &session);
    

    // 入力フォームからデータ受け取り 開始
    char *content_length_str = getenv("CONTENT_LENGTH");
    int content_length = atoi(content_length_str);

    char *data = malloc(content_length + 1);
    fgets(data, content_length + 1, stdin);
//    printf("%s<br>", data);



    char *comentStill = strtok(data, "&");
    comentStill = strchr(comentStill, '=') + 1;

    char *coment = malloc(strlen(comentStill) + 1);
    urlDecode(comentStill, coment);

    

    char *post_id = strtok(NULL, "&");
    post_id = strchr(post_id, '=') + 1;

    
    printf("%s<br>", session.user_id);    
    printf("%s<br>", post_id);
    printf("%s<br>", coment);
    // コメントの追加

    
    char *query = malloc(1000);
    sprintf(query, "INSERT INTO coments (contents, upload_date, update_date, user_id, post_id) VALUES ('%s', now(), now(), %s, %s);", coment, session.user_id, post_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }
   
    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
    }
    
    redirect(post_id);
 
    free(data);
    free(query);
    free(coment);

    mysql_free_result(result);
    mysql_close(conn);
    return 0;
}