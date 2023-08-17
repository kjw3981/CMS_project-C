#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

void urlDecode(const char* encoded, char* decoded) {
    //  decoded = malloc(strlen(encoded) + 1);
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
}

int main() 
{    
    char *data;
    long data_len;

    data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    data = malloc(data_len + 1);
    fgets(data, data_len + 1, stdin);

    char *title;
    char *contents;

    title = strtok(data, "&"); 
    contents= strtok(NULL, "&");

    title = strchr(title, '=') + 1;
    contents = strchr(contents, '=') +1;

    char *decoded_title = malloc(strlen(title) + 1);
    char *decoded_contents = malloc(strlen(contents) + 1);

    urlDecode(title, decoded_title);
    urlDecode(contents, decoded_contents);

    char *session_id = calloc(SESSION_MAX, sizeof(char));
    Session session;
    cookie(session_id);

    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    
    connect_sql(&conn);
    getSession(conn, &result, &row, session_id, &session);

    // データベースに新規作成
    char *query = malloc(10000);
    sprintf(query, "insert into inquiry (title, contents, upload_date, user_id) values ('%s', '%s', now(), %s);", decoded_title, decoded_contents, session.user_id);


    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
    }

    printf("<Content-Type: text/html>\r\n");
    
    printf("Location: mypage.exe\r\n\r\n");

/*
    printf("<html>");
    printf("<head>");
    printf("<title>投稿完了</title>");
    printf("<link href=\"write_process.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head>");
    printf("<body>\n");
    // ヘッダー
    header(session_id, session.user_name);
    
    printf("</div></div>\n");
    printf("</section>\n");
    printf("<h2>投稿完了<br>引き続き<font color=\"green\">いしたな</font>をお楽しみください★<br></h2>");
    printf("</body>");
    printf("</html>");
*/
    free(data);
    free(decoded_title);
    free(decoded_contents);
    free(query);

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}