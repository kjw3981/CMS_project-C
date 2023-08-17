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
} Article;

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
  
    char *query = malloc(1000);
    sprintf(query, "SELECT COUNT(*) from posts where user_id = %s", session.user_id);
    // printf("select posts query<br>%s<br>", query);

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
    
        
            sprintf(query, "select title, upload_date, id from posts where user_id = %s", session.user_id);
            // printf("select posts query<br>%s<br>", query);
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
                }        
        }
    }
    char *blog_name = malloc(100);
    sprintf(query, "SELECT blog_name from users where id = %s", session.user_id);
    // printf("select posts query<br>%s<br>", query);
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
    printf("<title>記事一覧</title>");
    printf("<link href=\"mypage.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head>\n");
    printf("<body>\n");
    
    // header関数 マイページのみ削除
    printf("<header class=\"header\">\n");
    printf("<div class=\"blog-logo\">\n");
    printf("<a href=\"top.exe\">\n");
    printf("<img alt=\"いしたなBlogロゴ\" id=\"site-top\" src=\"blog-logo.png\" width=\"500\" height=\"100\" />\n");
    printf("</a>\n");
    printf("</div>\n");
    printf("<nav class=\"global-nav\">\n");
    printf("<ul>\n");
    printf("<li> ようこそ、%sさん</li>\n", session.user_name);
    printf("<div class=\"menu\">\n");
    printf("<li onclick=\"toggleDropdown(event)\">メニュー</li>\n");
    printf("<div class=\"dropdown\">\n");
    printf("<li><a href=\"write.exe\">新記事作成</a></li>\n");
    printf("<li><a href=\"postlist.exe\">記事一覧</a></li>\n");
    printf("<li><a href=\"personalEdit.exe\">登録情報の変更</a></li>\n");
    printf("<li><a href=\"リンク未定\">ログアウト</a></li>\n");
    printf("</div>\n");
    printf("</div>\n");
    printf("</nav>\n");
    printf("</header>\n");
    // header関数

    printf("<div class=\"container\">\n");
    printf("<aside class=\"sidebar\">\n");
    printf("<a href=\"#\"><img alt=\"広告1\" src=\"sidebanner1.png\" width=\"230\" height=\"182\" /></a>\n");
    printf("<a href=\"#\"><img alt=\"広告2\" src=\"sidebanner2.png\" width=\"230\" height=\"182\" /></a>\n");
    printf("<a href=\"#\"><img alt=\"広告3\" src=\"sidebanner3.png\" width=\"230\" height=\"182\" /></a>\n");
    printf("</aside>\n");
    printf("<h1 class=\"site-title\">Blog %s</h1>\n", blog_name);
    printf("<h2 class=\"contents-title\">%sさんの記事一覧</h2>\n", session.user_name);
    if (id_count == 0) {
        printf("<h2>まだ記事はありません。</h2>\n");
    }
    else if (id_count > 0) {
        printf("<div class=\"news\">\n");

        for (int i = 0; i < id_count; i++) {

              printf("<form id=\"toLookNew%d\" action=\"look.exe\" method=\"get\">", i);
              printf("<input type=\"hidden\" name=\"post_id\" value=%d> ", articleNew[i].id);
              printf("</form>");
              printf("<a href=\"#\" onclick=\"document.getElementById('toLookNew%d').submit();\">%s</a>", i, articleNew[i].title);
              printf("<h3>%s</h3>", articleNew[i].upload_date);
          }

        printf("</div>\n");
    }

    printf("<h2 class=\"contents-title\"></h2>\n");
    printf("<nav class=\"footer-nav\">\n");
    printf("<ul>\n");
    printf("<li><a href=\"#site-top\">ページTOP</a></li>\n");
    printf("<li><a href=\"#\">利用規約</a></li>\n");
    printf("<li><a href=\"\">プライバシポリシー</a></li>\n");
    printf("</ul>\n");
    printf("</nav>\n");
    printf("</main>\n");
    printf("</div>\n");
    printf("<script>\n");
    printf("'use strict';\n");
    printf("const thumbs = document.querySelectorAll('.thumb');\n");
    printf("const bigImg = document.getElementById('bigimg');\n");
    printf("const interval = 3000;\n");
    printf("const pauseDuration = 5000;\n");
    printf("let currentIndex = 0;\n");
    printf("let timer;\n");
    printf("let paused = false;\n");
    printf("function showImage(index) {\n");
    printf("bigImg.src = thumbs[index].dataset.image;\n");
    printf("}\n");
    printf("thumbs.forEach(function(item, index) {\n");
    printf("item.onclick = function() {\n");
    printf("currentIndex = index;\n");
    printf("showImage(currentIndex);\n");
    printf("clearInterval(timer);\n");
    printf("startSlideshowWithDelay();\n");
    printf("}\n");
    printf("});\n");
    printf("function startSlideshow() {\n");
    printf("timer = setInterval(function() {\n");
    printf("currentIndex++;\n");
    printf("if (currentIndex >= thumbs.length) {\n");
    printf("currentIndex = 0;\n");
    printf("}\n");
    printf("showImage(currentIndex);\n");
    printf("}, interval);\n");
    printf("}\n");
    printf("function startSlideshowWithDelay() {\n");
    printf("if (paused) return;\n");
    printf("paused = true;\n");
    printf("setTimeout(function() {\n");
    printf("paused = false;\n");
    printf("startSlideshow();\n");
    printf("}, pauseDuration);\n");
    printf("}\n");

    printf("// 初期表示\n");
    printf("showImage(currentIndex);\n");
    printf("startSlideshow();\n");
    printf("</script>\n");
    printf("</body>\n");
    printf("</html>\n");
         
    free(blog_name);
    free(session_id);
    free(query);
    free(articleNew);
    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}