// user_id, user_name(to my page), posts_cnt (to posts page), comments_cnt(to comments page)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define MAX_USERS 1000
#define MAX_USERS_ID 11
#define MAX_USERS_NAME 200
#define MAX_DATE 20
#define MAX_CONTENTS 1000
#define MAX_TITLE 100

typedef struct{
    char date[MAX_DATE];
    char user_id[MAX_USERS_ID];
    char contents[MAX_CONTENTS];
    char title[MAX_TITLE];
} Inquiry;

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
    check_admin(conn, &result, &row, session.user_id);


    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html><html lang=\"ja\">");
    printf("<head><meta charset=\"UTF-8\"><title>いしたな</title>");
    printf("<link href=\"view_inquiry.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head><body><div class=\"box21\"><h2><a href=\"administrator.html\">いしたな　管理者マイページ</a></h2>");
    printf("</div><br><h3>ユーザー情報</h3><div class=\"sortable-table\"><table id=\"order_aiueo\">");
    printf("<thead><tr><th date-id=\"id\" data-header>");
    printf("<div>#</div></th>");
    printf("<th data-id=\"user_name\" sortable>user_name</th></tr></thead></table></div>");
    printf("<div class=\"centered\">");
    printf("<table><tr><th>日付</th><th>ユーザID</th><th>タイトル</th>");
    printf("</tr>");

    char *query = malloc(1000);

    sprintf(query, "SELECT COUNT(*) from inquiry;");
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

    Inquiry *inquiry = calloc(id_count, sizeof(Inquiry));

    sprintf(query, "SELECT upload_date, user_id, contents, title FROM inquiry;");
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

    for (int i = 0; i < id_count; i++) {
        if ((row = mysql_fetch_row(result)) != NULL) {
            strcpy(inquiry[i].date, row[0]);
            strcpy(inquiry[i].user_id, row[1]);
            strcpy(inquiry[i].contents, row[2]);
            strcpy(inquiry[i].title, row[3]);
        }
    }

    
    printf("<tbody>");
    
    for (int i=0; i < id_count; i++){
        printf("<tr>");
        printf("<td>%s</td>", inquiry[i].date);
        printf("<td>%s</td>", inquiry[i].user_id);
        printf("<td><a href=\"#\" class=\"main-text\" id=\"showPopup%d\">%s</a></td>", i, inquiry[i].title);
        printf("</tr>");        
    }

    printf("</tbody>"); 

    printf("</table></div></div>");

    printf("<script>");
    for (int i = 0; i < id_count; i++) {
        printf("document.getElementById(\"showPopup%d\").addEventListener(\"click\", function(event) {", i);
        printf("event.preventDefault();");
        printf("var popupWindow = window.open(\"\", \"_blank\", \"width=500,height=300\");");
        printf("popupWindow.document.write(\"<p>\" + '%s' + \"</p>\");", inquiry[i].contents);
        printf("});");
    }
    printf("</script>");

    printf("</body></html>"); // 2個目の/dibの謎
    
    free(inquiry);
    free(query);
    mysql_free_result(result);
    mysql_close(conn);
    return 0;
}
