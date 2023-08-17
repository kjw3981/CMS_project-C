#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define COMMENTS_MAX 1000
#define CONTENTS_MAX 1000
#define COMMENTS_ID_MAX 11

// 管理者がユーザー情報取得
int select_username_id(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *user_name,char *id)
{
    char *query = malloc(1000);
    sprintf(query, "select user_name from users where id = '%s';", id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }

    if ((*row = mysql_fetch_row(*result)) != NULL){
        strcpy(user_name, (*row)[0]);
    }

    free(query);
    return 0;
}

// getenvで住所持ってきて必要情報をとる
void get_user_id(char *user_id)
{
  char *query = getenv("QUERY_STRING");
  if (query != NULL) {
    char *param = strtok(query, "&");
    while (param != NULL) {
        char *equalsPos = strchr(param, '=');
        if (equalsPos != NULL) {
            *equalsPos = '\0';  // Null-terminate the parameter name
            char *name = param;
            char *value = equalsPos + 1;  // Move to the parameter value

            // Compare the parameter name with 'post_id'
            if (strcmp(name, "user_id") == 0) {
                // Parameter found, store the value
                strcpy(user_id, value);
                // post_id = value;
                break;
            }
        }

        param = strtok(NULL, "&");
        }
    }
}

// すべてのコメントを表示させる
int view_user_all_comments(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *id){

    char *query = (char *)malloc(1000);
    sprintf(query, "select id, contents, post_id, is_published from coments where user_id = %s;", id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }
    
    while ((*row = mysql_fetch_row(*result)) != NULL){
        printf("<tr>");
        printf("<td>%s</td>", (*row)[0]);
        printf("<td>%s</td>",(*row)[1]);
        printf("<td><a href=\"look.exe?post_id=%s\"></a>%s</td>",(*row)[2],(*row)[2]);
        printf("<td>");
        
        printf("<form id=\"\" action=\"change_published.exe\" method=\"post\">");
        printf("<input type=\"hidden\" name=\"type\" value=\"%s\">","coments");
        printf("<input type=\"hidden\" name=\"id\" value=\"%s\">",(*row)[0]);
        printf("<input type=\"hidden\" name=\"now\" value=\"%s\">",(*row)[3]);
        printf("<input type=\"hidden\" name=\"user_id\" value=\"%s\">",id);
        printf("<input type=\"submit\" value=\"%s\">", (*row)[3]);
        printf("</form>");
        
        printf("</td>");
        printf("</tr>");
    }

    free(query);
    return 0;
}

int main(void)
{
    // getでユーザーのIDを貰う
    char *user_id = (char *)malloc(100);
    get_user_id(user_id);

    // そのユーザーが作成した全ての記事を見る

    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    connect_sql(&conn);

    char *user_name = (char *)malloc(200);
    select_username_id(conn, &result, &row, user_name, user_id);
    printf("Content-Type: text/html\n\n");

    // html コード
    printf("<!DOCTYPE html><html lang=\"ja\">");
    printf("<head><meta charset=\"UTF-8\"><title>いしたな</title>");
    printf("<link href=\"user_traffic.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head><body><div class=\"box21\"><h2><a href=\"administrator.exe\" class=\"abc\">いしたな　管理者権限</a></h2>");
    printf("</div><br><h3>%sさんのcomments</h3><div class=\"sortable-table\"><table id=\"order_aiueo\">",user_name);
    printf("<thead><tr><th date-id=\"id\" data-header><div></div></th>");
    printf("<th data-id=\"user_name\" sortable></th></tr></thead></table></div>");
    printf("<div class=\"centered\"><table> <tr><th>COMMENTS_ID</th><th>Contents</th><th>Post_Id</th><th>Is Published</th>");
    printf("</tr>");
    
    view_user_all_comments(conn, &result, &row, user_id);

    printf("<tr>");

    printf("</table></div></div></body></html>");
    
    mysql_free_result(result);
    mysql_close(conn);
    
    free(user_id);
    free(user_name);

    return 0;
}
