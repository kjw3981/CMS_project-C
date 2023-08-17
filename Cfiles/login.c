#include <stdio.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define ID_MAX 100

int main() {
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    connect_sql(&conn);

    char *query = malloc(1000);
    sprintf(query, "SELECT COUNT(*) from users where is_published = 0");

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Query execution failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to store result set: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    row = mysql_fetch_row(result);
    if (row == NULL) {
        fprintf(stderr, "Failed to fetch row: %s\n", mysql_error(conn));
        mysql_free_result(result);
        mysql_close(conn);
        return 1;
    }

    int id_count = atoi(row[0]);

    char **id_str = malloc(sizeof(char*));

    for (int i = 0; i < id_count; i++) {
        id_str[i] = calloc(ID_MAX, sizeof(char));
    }

    sprintf(query, "SELECT user_id, password FROM users where is_published = 0");
    if (mysql_query(conn, query)) {
        printf("Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        printf("Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }

    for (int i = 0; i < id_count; i++) {
        if ((row = mysql_fetch_row(result)) != NULL) {
            strcpy(id_str[i], row[0]);
        }
    }

    printf("content-Type: text/html\n\n");  
    printf("<html>\n");
    printf("<head>\n");
    printf("<title>Login Form</title>\n");
    printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"login.css\">\n");
    printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("<h2>Login</h2>\n");
    printf("<form action=\"loginform.exe\" method=\"POST\">\n");
    printf("<label for=\"id\">IDを入力</label>\n");
    printf("<input class=\"input\" type=\"text\" id=\"id\" name=\"id\"><br><br>\n");
    printf("<h4 class=\"Error\" id=\"idError\"></h4>\n");
    printf("<label for=\"password\">Passwordを入力</label>\n");
    printf("<input class=\"input\" type=\"password\" id=\"password\" name=\"password\"><br><br>\n");
    printf("<input id=\"login-btn\" type=\"submit\" id=\"mySubmit\" onclick=\"idInput(event), banCheck(event);\" value=\"Login\">\n");
    printf("</form>\n");

    printf("<script>\n");
    printf("'use strict';\n");
    symbolErrorInit();
    symbolError("id");
    printf("function banCheck(event) {\n");
    printf("var idInput = document.getElementById('id');\n");
    printf("var submit = document.getElementById('mySubmit');\n");
    printf("var Flg = false;\n");
    for (int i = 0; i < id_count; i++) {
        printf("if (idInput.value === '%s') {\n", id_str[i]); // BANされてるかチェック
        printf("Flg = true;\n");
        printf("}\n");
    }
    printf("if (Flg) {\n");
    printf("event.preventDefault();\n");
    printf("window.alert('お前はだめだ BANしてやったぞ');\n");
    printf("}\n");
    printf("}\n");

    printf("</script>\n");
    printf("</body>\n");
    printf("</html>\n");

    free(query);
    free(id_str);

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}
