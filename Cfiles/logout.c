#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

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
    // データの取得/
    char *data;
    long data_len;

    data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    data = malloc(data_len + 1);
    fgets(data, data_len + 1, stdin);

    char *session_id;

    session_id = strtok(data, "&");
    session_id = strchr(session_id, '=') + 1;

    connect_sql(&conn);
    delete_session_id(conn, &result, &row, session_id);
    
    mysql_free_result(result);
    mysql_close(conn);
    
    return 0;

}