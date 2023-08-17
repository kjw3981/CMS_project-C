#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define UPLOAD_DATE_MAX 50
#define TITLE_MAX 100
#define DISPLAY_MAX 5

typedef struct {
  int id;
  char upload_date[UPLOAD_DATE_MAX];
  char title[TITLE_MAX];
} Article;

int main()
{
   

    // データの取得/
    char *data;
    long data_len;

    data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    data = malloc(data_len + 1);
    fgets(data, data_len + 1, stdin);
    
    char *post_id;
    post_id = strtok(data, "&");
    post_id = strchr(post_id, '=') + 1;

    MYSQL *conn;
    MYSQL_RES *result;  // 構造体の名前かな？
    MYSQL_ROW row;      // 文字列の配列としてデータを返す

    connect_sql(&conn);

/*
    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    Session session;   
    getSession(conn, &result, &row, session_id, &session);

    char *user_id = calloc(SESSION_MAX, sizeof(char));
    char *tmp = malloc(1000);
    sprintf(tmp, "select user_id from posts where id = %s;", post_id);
    if (mysql_query(conn, tmp)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }
    if ((row = mysql_fetch_row(result)) != NULL){
       strcpy(user_id, row[0]);
    }
*/

    // 記事のタイトル、本文　取得。
    char *query = malloc(1000);
    sprintf(query, "select title, contents from posts where id = %s;", post_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }

    char *title = malloc(200);
    char *contents = malloc(10000);
    if ((row = mysql_fetch_row(result)) != NULL){
        strcpy(title, row[0]);
        strcpy(contents, row[1]);
    }

    printf("<Content-Type: text/html>\n\n");
    printf("<html>");
    printf("<meta charset=\"UTF-8\">");

    printf("<form name=\"login_form\" action=\"intermediation.exe\" method=\"post\">");
    printf("<head>");
    printf("<title>新規投稿の編集画面</title>");
    printf("<link href=\"update.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head>");
    printf("<div class=\"write\">");
    printf("<h1>記事修正</h1>");
    printf("</div>");
    printf("<div class=\"login_form_btm\">");
    printf("<input type=\"hidden\" name=\"myValue\" value=\"%s\">", post_id);
    printf("<p>タイトル</p>");
    printf("<input class=\"input\" type=\"title\" name=\"title\" id=\"title\" value=\"%s\"><br>", title);
    printf("<h5 id=\"titleError\"></h5>");
    char *post_contents = malloc(10000);
    
    int contents_cnt;
    int post_contents_cnt = 0;
    /* contentsの各文字に対し、*/
    for ( contents_cnt = 0; contents[contents_cnt] != '\0'; ++contents_cnt ) 
    {
        /* '\r' じゃない文字を bに追加する */
        if (contents[contents_cnt] != '\r' ) 
        {
            post_contents[post_contents_cnt] = contents[contents_cnt];
            ++post_contents_cnt;
        }
    }
    post_contents[post_contents_cnt] = '\0'; /* '\0'で終端する */
    
    printf("<p>本文</p>");
    printf("<textarea class=\"input\" name=\"maintext\" id=\"maintext\">%s</textarea>", post_contents);
    printf("<h5 id=\"maintextError\"></h5>");
// 更新ボタン
    printf("<input type=\"hidden\" id=\"id\" name=\"user_id\" value=\"%s\">", post_id);
    printf("<div><button id=\"login-btn\" type=\"submit\" onclick=\"titleInput(event), maintextInput(event);\">更新</button></div></form>");
    printf("</div>");
    printf("</form>");
    printf("<body>");
    printf("<script>");
    symbolErrorInit();
    symbolError("title");
    symbolError("maintext");
    printf("</script>");
    printf("</body>");
    printf("</html>");

    free(data);  
    free(query);
    free(title);
    free(contents);
    free(post_contents);

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}