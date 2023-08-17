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

    // comentsを格納 日本語化
    char *comentStill = strtok(data, "&");
    comentStill = strchr(comentStill, '=') + 1;

    char *edit = malloc(strlen(comentStill) + 1);
    urlDecode(comentStill, edit);    

    char *coments_id = strtok(NULL, "&");
    char *post_id = strtok(NULL, "&");

    coments_id = strchr(coments_id, '=') + 1;
    post_id = strchr(post_id, '=') + 1;

    // データベースのコメントの訂正
    char *query = malloc(1000);

    sprintf(query,  "UPDATE coments SET contents=\"%s\", update_date=now() WHERE id=%s;", edit, coments_id);
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

    mysql_free_result(result);
    mysql_close(conn);
    return 0;
}