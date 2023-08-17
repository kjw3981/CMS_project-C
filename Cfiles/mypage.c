#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define UPLOAD_DATE_MAX 50
#define TITLE_MAX 100
#define DISPLAY_MAX 5

typedef struct {
  int id;
  char upload_date[UPLOAD_DATE_MAX];
  char title[TITLE_MAX];
  char visit_cnt[11];
} Article;

// getenvで住所持ってきて必要情報とる 
void get_page(char *page)
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
                break;
            }
        }

        param = strtok(NULL, "&");
        }
    }
}

int main(void)
{
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    char *page = calloc(11, sizeof(char));
    get_page(page);
    int page_num;

    if (page[0] == '\0'){
        page_num = 1;
    } else {
        page_num = atoi(page);
    }

    connect_sql(&conn);
    delete_expiry_session(conn, &result, &row);
    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    Session session;   
    getSession(conn, &result, &row, session_id, &session);

    if (session_id[0] == '\0') {
        printf("content-Type: text/html\r\n");
        printf("Location: top.exe\r\n\r\n");
    }
    
    // 管理者(user_idが1)なら管理者ページに飛ぶ
    if(strcmp(session.user_id, "1") == 0){
        printf("content-Type: text/html\r\n");
        printf("Location: administrator.exe\r\n\r\n");
    }

    char *query = malloc(1000);
    sprintf(query, "SELECT COUNT(*) from posts where user_id = %s", session.user_id);
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

    Article *articleNew = malloc(id_count * sizeof(Article));

    if (id_count == -1) {
        fprintf(stderr, "ERROR: id_count == -1\n");
        return 1;
    }
    else if (id_count > 0) {     
        sprintf(query, "select title, upload_date, id, visit_cnt from posts where user_id = %s order by update_date desc;", session.user_id);
        if (mysql_query(conn, query)) {
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
                strcpy(articleNew[i].title, row[0]);
                strcpy(articleNew[i].upload_date, row[1]);
                articleNew[i].id = atoi(row[2]);
                strcpy(articleNew[i].visit_cnt, row[3]);
            }        
        }
    }
    char *blog_name = malloc(100);
    sprintf(query, "SELECT blog_name from users where id = %s", session.user_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
    fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
    return 1;
    }
    if ((row = mysql_fetch_row(result)) != NULL) {
        strcpy(blog_name, row[0]);
    }

    printf("content-Type: text/html\n\n"); 

    printf("<html lang=\"ja\">\n");
    printf("<head>\n");
    printf("<meta charset=\"utf-8\" />\n");
    printf("<title>マイページ</title>");
    printf("<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css\">");
    printf("<link href=\"mypage1.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head>\n");
    printf("<body>\n");
    
    // ヘッダー
    printf("<header class=\"header\">\n");
    printf("<div class=\"blog-logo\">\n");
    printf("<a href=\"top.exe\">\n");
    printf("<img alt=\"いしたなBlogロゴ\" id=\"site-top\" src=\"blog-logo.png\" width=\"500\" height=\"100\" />\n");
    printf("</a>\n");
    printf("</div>\n");
    printf("<nav class=\"global-nav\">\n");
    printf("<ul>\n");
    
    printf("<li class=\"notChange\"> ようこそ、%sさん</li>\n", session.user_name);
    printf("<div class=\"menu\">\n");
    printf("<li onclick=\"toggleDropdown(event)\">メニュー</li>\n");
    printf("<div class=\"dropdown\">\n");
    printf("<li><a href=\"allpost.exe\">記事一覧</a></li>\n");
    printf("<li><a href=\"search.exe\">記事検索</a></li>\n");
    printf("<li><a href=\"write.exe\">新規投稿</a></li>\n");
    printf("<li><a href=\"personalLogout.exe\">登録情報の変更</a></li>\n");
    printf("<li><a href=\"inquiry.exe\">お問い合わせ</a></li>");
    printf("<li><a href=\"#\" onclick=\"document.getElementById('logout').submit();\">ログアウト</a></li>\n");
    printf("<form id=\"logout\" action=\"logout.exe\" method=\"post\">");
    printf("<input type=\"hidden\" name=\"session_id\" value=\"%s\">", session_id);
    printf("</form>");
    printf("</div>\n");
    printf("</div>\n");
    

    printf("</nav>\n");
    printf("</header>\n");

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
    printf("<h1 class=\"banner\">マイページ</h1>");
    printf("</div>\n");
    printf("</div>\n");
    printf("</section>\n");
    printf("<h1 class=\"site-title\">Blog %s</h1>\n", blog_name);
    printf("<h2 class=\"contents-title\">%sさんの過去記事</h2>\n", session.user_name);
    if (id_count == 0) {
        printf("<h2>まだ記事はありません。</h2>\n");
    }
    else if (id_count > 0) {
        printf("<div class=\"news\">\n");
        char *char_post_id = malloc(11);
        int temp = 10;
        if (id_count - (page_num-1)*10 < 10){
            temp = id_count - (page_num-1)*10;
        }
        int start = (page_num-1)*10;
        for (int i = start; i < start + temp; i++) {
            sprintf(char_post_id, "%d", articleNew[i].id);
            if (check_post_id(conn, &result, &row, char_post_id) == 0){
                printf("<h3>%s(管理者よりブラックされた記事です)</h3>",articleNew[i].title);
                printf("<div class=\"small_font\">%s</div>", articleNew[i].upload_date);
                continue;
            }
              printf("<form id=\"toLookNew%d\" action=\"look.exe\" method=\"get\">", i);
              printf("<input type=\"hidden\" name=\"post_id\" value=%d> ", articleNew[i].id);
              printf("</form>");
              printf("<h3><a href=\"#\" onclick=\"document.getElementById('toLookNew%d').submit();\">%s</a></h3>", i, articleNew[i].title);
              printf("<div class=\"small_font\">%s　%s</div>", articleNew[i].upload_date, articleNew[i].visit_cnt);
          }
        // 10ページごと切り替え
        printf("<div class=\"pageswitch\">");
        int pageswitch = (id_count - 1) / 10;
        for (int i = 0; i < pageswitch + 1; i++)
        {
            printf("<h3 style=\"display:inline\"><a href=mypage.exe?page=%d>%d　</a></h3>", i+1, i+1); 
        }
        printf("</div>");
        printf("</div>\n");
        free(char_post_id);
    }

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
    printf("</body>\n");
    printf("</html>\n");
         
    free(blog_name);
    free(session_id);
    free(query);
    free(articleNew);
    free(page);

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}