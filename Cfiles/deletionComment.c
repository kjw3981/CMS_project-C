#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

int main(void)
{
//    printf("content-Type: text/html\n\n"); 
//    printf("<meta charset=\"UTF-8\">");


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

/*   
    printf("%s<br>", session_id);    
    printf("%s<br>", coments_id);
    printf("%s<br>", post_id);
*/
    
    // コメントの削除
    char *query = malloc(1000);
    sprintf(query,  "DELETE FROM coments WHERE id=%s", coments_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }
   
    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
    }



    free(data);
    free(query);

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}