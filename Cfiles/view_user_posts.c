#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define POSTS_MAX 1000
#define TITLE_MAX 200
#define POSTS_ID_MAX 11

typedef struct {
    char posts_id[POSTS_MAX][POSTS_ID_MAX];
    char posts_title[POSTS_MAX][TITLE_MAX];
    char posts_published[POSTS_MAX][2];
} Posts;

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
// ポストのコメントの数とってくる
int select_comments_count(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *post_id, char *comments_cnt)
{
    char *query = (char *)malloc(1000);
    sprintf(query, "select count(*) from coments where post_id = %s;", post_id);
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

int view_user_all_post(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *id){

    char *query = (char *)malloc(1000);
    sprintf(query, "select id, title, is_published from posts where user_id = %s;", id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }

    Posts posts;
    int post_cnt = 0;
    
    while ((*row = mysql_fetch_row(*result)) != NULL){
        strcpy(posts.posts_id[post_cnt], (*row)[0]);
        strcpy(posts.posts_title[post_cnt], (*row)[1]);
        strcpy(posts.posts_published[post_cnt], (*row)[2]);
        post_cnt ++;
        
    }
    
    char *comments_cnt = malloc(100);
    for (int i=0; i<post_cnt; i++){
        select_comments_count(conn, result, row, posts.posts_id[i], comments_cnt);
        printf("<tr>");
        printf("<td>%s</td>", posts.posts_id[i]);
        printf("<td><a href=\"look.exe?post_id=%s\">%s</a></td>",posts.posts_id[i],posts.posts_title[i]);
        printf("<td><a href=\"#\">%s</td>",comments_cnt);
        printf("<td>");
        
        printf("<form id=\"\" action=\"change_published.exe\" method=\"post\">");
        printf("<input type=\"hidden\" name=\"type\" value=\"%s\">","posts");
        printf("<input type=\"hidden\" name=\"id\" value=\"%s\">",posts.posts_id[i]);
        printf("<input type=\"hidden\" name=\"now\" value=\"%s\">",posts.posts_published[i]);
        printf("<input type=\"hidden\" name=\"user_id\" value=\"%s\">",id);
        printf("<input type=\"submit\" value=\"%s\">", posts.posts_published[i]);
        printf("</form>");
        
        printf("</td>");
        printf("</tr>");
    }
    
    free(comments_cnt);
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
    printf("</div><br><h3>%sさんのPOSTS</h3><div class=\"sortable-table\"><table id=\"order_aiueo\">",user_name);
    printf("<thead><tr><th date-id=\"id\" data-header><div></div></th>");
    printf("<th data-id=\"user_name\" sortable></th></tr></thead></table></div>");
    printf("<div class=\"centered\"><table> <tr><th>POST_ID</th><th>Title</th><th>Comments Count</th><th>Is Published</th>");
    printf("</tr>");
    
    view_user_all_post(conn, &result, &row, user_id);

    printf("</table></div></div></body></html>");

    mysql_free_result(result);
    mysql_close(conn);

    free(user_id);
    free(user_name);

    return 0;
}
