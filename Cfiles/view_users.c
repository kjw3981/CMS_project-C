// user_id, user_name(to my page), posts_cnt (to posts page), comments_cnt(to comments page)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define MAX_USERS 1000
#define MAX_USERS_ID 11
#define MAX_USERS_NAME 200

// 二次元配列の構造体
typedef struct{
    char user_id[MAX_USERS][MAX_USERS_ID];
    char user_name[MAX_USERS][MAX_USERS_NAME];
    char user_published[MAX_USERS][2];
} Users;

// 記事の個数カウント
int select_posts_cnt(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *id, char *posts_cnt)
{
    char *query = (char *)malloc(1000);
    sprintf(query, "select count(*) from posts where user_id = %s;", id);
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
        strcpy(posts_cnt, (*row)[0]);
    } else {
        strcpy(posts_cnt, "0");
    }

    free(query);
    return 0;
}

// コメントの個数カウント
int select_comments_cnt(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *id, char *comments_cnt)
{
    char *query = (char *)malloc(1000);
    sprintf(query, "select count(*) from coments where user_id = %s;", id);
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
        strcpy(comments_cnt, (*row)[0]);
    } else {
        strcpy(comments_cnt, "0");
    }

    free(query);
    return 0;
}

// すべてのユーザーの情報もってくる
int view_all_users(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row)
{
    char *query = (char *)malloc(1000);
    sprintf(query, "select id ,user_name, is_published from users;");
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }

    char *id = (char *)malloc(11);
    char *user_name = (char *)malloc(100);
    char *posts_cnt = (char *)malloc(20);
    char *comments_cnt = (char *)malloc(20);

    Users users;

    int user_cnt = 0;
    while ((*row = mysql_fetch_row(*result)) != NULL){
        // idを持ってきます
        strcpy(users.user_id[user_cnt], (*row)[0]);
        strcpy(users.user_name[user_cnt], (*row)[1]);
        strcpy(users.user_published[user_cnt], (*row)[2]);
        user_cnt ++;
    }

    for (int i=0; i<user_cnt; i++){
        select_posts_cnt(conn, result, row, users.user_id[i], posts_cnt);
        select_comments_cnt(conn, result, row, users.user_id[i], comments_cnt);
        printf("<tr>");
        printf("<td>%s</td>", users.user_id[i]);
        printf("<td>%s</td>", users.user_name[i]);
        printf("<td><a href=\"view_user_posts.exe?user_id=%s\">%s</a></td>", users.user_id[i],posts_cnt);
        printf("<td><a href=\"view_user_comments.exe?user_id=%s\">%s</a></td>", users.user_id[i],comments_cnt);
        printf("<td>");
        printf("<form id=\"\" action=\"change_published.exe\" method=\"post\">");
        printf("<input type=\"hidden\" name=\"type\" value=\"%s\">","users");
        printf("<input type=\"hidden\" name=\"id\" value=\"%s\">",users.user_id[i]);
        printf("<input type=\"hidden\" name=\"now\" value=\"%s\">",users.user_published[i]);
        printf("<input type=\"hidden\" name=\"user_id\" value=\"%s\">",users.user_id[i]);
        if (i != 0) {
            printf("<input type=\"submit\" value=\"%s\">", users.user_published[i]);
        }
        printf("</form>");
        printf("</td>");
        printf("</tr>");        
    }

    free(query);
    free(id);
    free(user_name);
    free(posts_cnt);
    free(comments_cnt);

    return 0;
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
    check_admin(conn, &result, &row, session.user_id);


    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html><html lang=\"ja\">");
    printf("<head><meta charset=\"UTF-8\"><title>いしたな</title>");
    printf("<link href=\"user_traffic.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head><body><div class=\"box21\"><h2><a href=\"administrator.exe\" class=\"change-none\">いしたな　管理者権限</a></h2>");
    printf("</div><br><h3>ユーザー情報</h3><div class=\"sortable-table\"><table id=\"order_aiueo\">");
    printf("<thead><tr><th date-id=\"id\" data-header>");
    printf("<div></div></th>");
    printf("<th data-id=\"user_name\" sortable></th></tr></thead></table></div>");
    printf("<div class=\"centered\">");
    printf("<table><tr><th>ID</th><th>User Name</th><th>Posts Count</th><th>Comments Count</th><th>is_published</th>");
    printf("</tr>");

    view_all_users(conn, &result, &row);
    
    printf("</table></div></div></body></html>");

    mysql_free_result(result);
    mysql_close(conn);
    
    free(session_id);
    return 0;
}
