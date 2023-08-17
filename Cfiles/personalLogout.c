#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

// sessionテーブルのsesesion_idを削除
int delete_session_id(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *session_id)
{
    char *query = (char *)malloc(1000);
    sprintf(query, "delete from sessions where session_id = '%s';",session_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        // fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        // return 1;
    }

    free(query);
    return 0;
}

int main(void)
{
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    
    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    connect_sql(&conn);
    delete_session_id(conn, &result, &row, session_id);
    
    printf("Content-type: text/html\r\n");
    printf("Location: personalLogin.exe\r\n\r\n");

    mysql_free_result(result);
    mysql_close(conn);

    free(session_id);

    return 0;
}