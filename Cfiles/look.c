#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define COMENT_MAX 500
// getenvで住所持ってきて必要な情報とる
void get_page(char *page, char *post_id)
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
            if (strcmp(name, "page") == 0) {
                // Parameter found, store the value
                strcpy(page, value);
                // post_id = value;
                
            } else if (strcmp(name, "post_id") == 0) {
                strcpy(post_id, value);
            }
        }

        param = strtok(NULL, "&");
        }
    }
}

// データベースから情報もってくる
int view_post(MYSQL_RES **result, MYSQL *conn, MYSQL_ROW *row,char *post_id, char *title, char* contents, char *post_user_id, char *blog_name, char *post_date, char *post_user_name)
{   
    char *query = malloc(1000);
    // postsとusersのテーブルくっつけて情報持ってくる
    sprintf(query, "select p.title, p.contents, p.upload_date, u.user_id, u.blog_name, u.user_name from posts as p left join users as u on p.user_id = u.id where p.id = %s;", post_id);
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
        strcpy(title, (*row)[0]);
        strcpy(contents, (*row)[1]);
        strcpy(post_user_id, (*row)[3]);
        strcpy(blog_name, (*row)[4]);
        strcpy(post_date, (*row)[2]);
        strcpy(post_user_name, (*row)[5]);
    }
    free(query);

    return 0;
}

int visit_cnt_plus(MYSQL_RES **result, MYSQL *conn, MYSQL_ROW *row, char *post_id){
    char *query = malloc(1000);
    // postsとusersのテーブルくっつけて情報持ってくる
    sprintf(query, "UPDATE posts SET visit_cnt = visit_cnt + 1 where id = %s",post_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {

    }

    free(query);
    return 0;
}

int main(void)
{   

    Post post;
    // コメントのページ情報

    char *page = calloc(11, sizeof(char));
    get_page(page,post.id);
    int page_num;
    
    if (page[0] == '\0'){
        page_num = 1;
    } else {
        page_num = atoi(page);
    }
    
    // get_post_id(post.id);

    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    
    connect_sql(&conn);
    if (check_post_id(conn, &result, &row, post.id) == 0){
        printf("Content-type: text/html\r\n");
        printf("Location: top.exe\r\n\r\n");
    }
    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    Session session;   
    getSession(conn, &result, &row, session_id, &session);
    char *post_date = malloc(100);
    char *post_user_name = malloc(100);
    view_post(&result, conn, &row, post.id, post.title, post.contents, post.user_id, post.blog_name, post_date, post_user_name);

    
    char *query = malloc(1000);
    sprintf(query, "SELECT COUNT(*) FROM coments WHERE post_id=%s and is_published = 1;", post.id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Query execution failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to store result set: %s\n", mysql_error(conn));
        mysql_free_result(result);
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

// 構造体ではなく二次元配列ぽく作りたかった
    int id_count = atoi(row[0]); // コメントの個数

    char **comentStr = (char**)malloc(id_count * sizeof(char*));
    char **coment_user_id = (char**)malloc(id_count * sizeof(char*));
    char **coment_user_name = (char**)malloc(id_count * sizeof(char*));
    char **coment_update_date = (char**)malloc(id_count * sizeof(char*));
    char **coments_id = (char**)malloc(id_count * sizeof(char*));

// id_countの分だけまわす
    if (id_count > 0) { 
        for (int i = 0; i < id_count; i++) {
            comentStr[i] = malloc(COMENT_MAX);
        }  

        for (int i = 0; i < id_count; i++) {
            coment_user_id[i] = malloc(USER_ID_MAX);
        }
        
        for (int i = 0; i < id_count; i++) {
            coment_user_name[i] = malloc(NAME_MAX);
        }
                
        for (int i = 0; i < id_count; i++) {
            coment_update_date[i] = malloc(DATE_MAX);
        }
        
        for (int i = 0; i < id_count; i++) {
            coments_id[i] = malloc(USER_ID_MAX);
        }
        // 降順表示
        sprintf(query, "SELECT contents, user_id, update_date, id FROM coments WHERE post_id=%s and is_published = 1 order by update_date desc;", post.id);
        if (mysql_query(conn, query) != 0) {
            fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
            return 1;
        }
    
        result = mysql_store_result(conn);
        if (result == NULL) {
            fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
            return 1;
        }

        for (int i = 0; i < id_count; i++) {
            if ((row = mysql_fetch_row(result)) != NULL) {
                strcpy(comentStr[i], row[0]);
                strcpy(coment_user_id[i], row[1]);
                strcpy(coment_update_date[i], row[2]);
                strcpy(coments_id[i], row[3]);
            }
        }

        for (int i = 0; i < id_count; i++) {
            sprintf(query, "SELECT user_name FROM users WHERE id=%s;", coment_user_id[i]);
            if (mysql_query(conn, query) != 0) {
                fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
                return 1;
            }
        
            result = mysql_store_result(conn);
            if (result == NULL) {
                fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
                return 1;
            }
        
            if ((row = mysql_fetch_row(result)) != NULL) {
                strcpy(coment_user_name[i], row[0]);
            }
        }
    }

    
    printf("content-Type: text/html\n\n");  
    printf("<html lang=\"ja\">\n");
    printf("<head>\n");
    printf("<meta charset=\"utf-8\" />\n");
    printf("<title>トップページ</title>\n");
    printf("<link href=\"look.css\" rel=\"stylesheet\" />\n");
    printf("</head>\n");
    printf("<body>\n");
    // ヘッダー
    header(session_id, session.user_name);
    printf("<div class=\"container\">\n");
    printf("<aside class=\"sidebar\">\n");
    printf("<a href=\"#\"><img alt=\"広告1\" src=\"sidebanner1.png\" width=\"230\" height=\"182\" /></a>\n");
    printf("<a href=\"#\"><img alt=\"広告2\" src=\"sidebanner2.png\" width=\"230\" height=\"182\" /></a>\n");
    printf("<a href=\"#\"><img alt=\"広告3\" src=\"sidebanner3.png\" width=\"230\" height=\"182\" /></a>\n");
    printf("</aside>\n");
    printf("<main class=\"main-contents\">\n");
    printf("<section>\n");
    printf("<div class=\"center\">\n");
    printf("<div class=\"banner-space\">\n");
    
    printf("</div>\n");
    
    printf("</div>\n");
    printf("</section>\n");

    printf("<h2 class=\"contents-title\">%s</h2>\n", post.title);
    printf("<div class=\"small_font\">%s　%s</div>",post_date, post_user_name);
    char *post_contents = malloc(10000);
    
    int contents_cnt;
    int post_contents_cnt = 0;
    /* post.contents(データベースからもってきた本文)の各文字に対し、*/
    for ( contents_cnt = 0; post.contents[contents_cnt] != '\0'; ++contents_cnt ) 
    {
        /* '\r' じゃない文字を bに追加する */
        if ( post.contents[contents_cnt] != '\r' ) 
        {
            post_contents[post_contents_cnt] = post.contents[contents_cnt];
            ++post_contents_cnt;
        }
    }
    post_contents[post_contents_cnt] = '\0'; /* '\0'で終端する */

// post.contents と post_contents　ワンクッション必要で名前変えてる
    printf("<h4 class=\"contents\">%s</h4>\n", post_contents);
    // user_idとpost_user_idが一致したらボタンの表示
    if (strcmp(session.user_user_id, post.user_id) == 0){
        printf("<div class=\"article_btn\">");
        // 修正ボタン
        printf("<form name=\"syusei_form\" action=\"update.exe\" method=\"post\">");
        printf("<input type=\"hidden\" name=\"postId\" value=%s>\n", post.id);
        printf("<div class=\"syusei_form_btm\"><button type=\"syusei\">記事修正</button></div></form>");
        // 削除ボタン
        printf("<form name=\"delete_form\" action=\"deletionOk.exe\" method=\"post\">");
        printf("<input type=\"hidden\" name=\"postId\" value=%s>\n", post.id);
        printf("<div class=\"delete_form_btm\"><button type=\"sakuzyo\" id=\"articleDelete\">記事削除</button></div></form>");
        printf("</div>");
    }
  
    printf("<h2 class=\"contents-title\">コメント</h2>\n");

    if (session_id[0] == '\0')
    {
        printf("ログインしてコメントしてね！！");
    }
    else
    {
        printf("<form id=\"Tocoment\" action=\"articleComment.exe\" method=\"post\">\n");
        printf("<input type=\"textarea\" name=\"coment\" id=\"coment\" placeholder=\"コメントを入力\">\n");
        printf("<input type=\"hidden\" name=\"postId\" value=%s>\n", post.id);
        printf("<h2 class=\"Error\" id=\"comentError\"></h2>");
        printf("<button class=\"comment_btn\" type=\"submit\" onclick=\"comentInput(event);\">送信</button>");
        printf("</form>\n");
    }
    // 1page10コ　①1～10, ②11～20的な
    int start = (page_num-1)*10;
    int temp = 10;
    if (temp + (page_num-1)*10 > id_count){
        temp = id_count - (page_num-1)*10;
    }
    for (int i = start; i < start + temp; i++) {
       
        printf("<div class=\"coment-table\">\n");
        
        printf("\n");
        printf("\n");
        printf("\n");
        
        printf("<h4 class=\"coment-contents\">%s</h4>\n", comentStr[i]);
        printf("<div class=\"coment-row\" class=\"small_font\">%s %s</div>\n", coment_user_name[i], coment_update_date[i]);
        if (strcmp(session.user_id, coment_user_id[i]) == 0) {
            printf("<table>\n");
            printf("<tr>\n");
            
            printf("<div class=\"edit\">");
        // 修正ボタン
            printf("<form name=\"syusei_form\" action=\"editComment.exe\" method=\"post\">");
            printf("<input type=\"hidden\" id=\"id\" name=\"comment_id\" value=\"%s\">", coments_id[i]);
            printf("<input type=\"hidden\" name=\"postId\" value=%s>\n", post.id);
            printf("<td><button class=\"form_btm\" type=\"syusei\">修正</button></form>");
        // 削除ボタン
            printf("<form name=\"delete_form\" action=\"deletionComment.exe\" method=\"post\">");
            printf("<input type=\"hidden\" id=\"id\" name=\"comment_id\" value=\"%s\">", coments_id[i]);
            printf("<input type=\"hidden\" name=\"postId\" value=%s>\n", post.id);
            printf("<td><button class=\"form_btm\" type=\"sakuzyo\" id=\"comentDelete\">削除</button></form>");
            printf("</div>"); //追加
            printf("</tr>\n");
            printf("</table>\n");
        }
        printf("</div>\n");  
        printf("<br>\n");
    }
    // 10コごとページ切り替え
    printf("<div class=\"pageswitch\">");
    int pageswitch = (id_count - 1) / 10;
    for (int i = 0; i < pageswitch + 1; i++)
    {
        printf("<h3 style=\"display:inline\"><a href=look.exe?post_id=%s&page=%d class=\"link-none\">%d　</a></h3>", post.id, i + 1, i + 1);
    }
    printf("</div>");

    printf("<h2 class=\"Error\" id=\"comentError\"><h2>");
    
    printf("\n");
    printf("\n");
    printf("\n");


    printf("<h2 class=\"contents-title\"></h2>\n");
    printf("<nav class=\"footer-nav\">\n");
    printf("<ul>\n");
    printf("<li><a href=\"#site-top\">ページTOP</a></li>\n");
    printf("<li><a href=\"promise.html\">利用規約</a></li>\n");
    printf("<li><a href=\"privacy.html\">プライバシポリシー</a></li>\n");
    printf("</ul>\n");
    printf("</nav>\n");
    printf("</main>\n");
    printf("</div>\n");
    printf("<script>\n");
    printf("'use strict';\n");

    printf("document.getElementById(\"articleDelete\").addEventListener(\"click\", function(event) {");
    printf("var confirmation = window.confirm(\"本当に削除しますか？\");");
    printf("if (confirmation) {");
    printf("window.alert(\"記事が削除されました。\")");
    printf("}");
    printf("else {");
    printf("event.preventDefault();");
    printf("}");
    printf("});");

    printf("document.getElementById(\"comentDelete\").addEventListener(\"click\", function(event) {");
    printf("var confirmation = window.confirm(\"本当に削除しますか？\");");
    printf("if (confirmation) {");
    printf("window.alert(\"コメントが削除されました。\")");
    printf("}");
    printf("else {");
    printf("event.preventDefault();");
    printf("}");
    printf("});");

    symbolErrorInit();
    symbolError("coment"); 
    printf("</script>\n");
    printf("</body>\n");
    printf("</html>\n");


      
    for (int i = 0; i < id_count; i++) {
        free(comentStr[i]);   
    }
    
    for (int i = 0; i < id_count; i++) {
        free(coment_user_id[i]);
    }

    for (int i = 0; i < id_count; i++) {
        free(coment_user_name[i]);
    }

    for (int i = 0; i < id_count; i++) {
        free(coment_update_date[i]);
    }
    visit_cnt_plus(&result, conn,&row, post.id);
    free(comentStr);
    free(coment_user_id);
    free(coment_user_name);
    free(coment_update_date);
    free(coments_id);
    free(page);
    free(post_contents);
    free(post_date);
    free(post_user_name);
    free(session_id);
    free(query);
    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}