#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

#define UPLOAD_DATE_MAX 50
#define TITLE_MAX 100
#define DISPLAY_MAX 5
#define USER_NAME_MAX 200

typedef struct {
  char char_post_id[11];
  char title[TITLE_MAX];
  char upload_date[UPLOAD_DATE_MAX];
  char user_name[USER_NAME_MAX];
  char comment_conunt[11];
  char visit_cnt[11];
  char user_id[11];
} Article;

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
    

    char *query = malloc(1000);
    sprintf(query, "SELECT COUNT(*) from posts where is_published = 1;");

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
    
        
            sprintf(query, "SELECT p.id, p.title, p.update_date, u.user_name, COUNT(c.id) AS comment_count, visit_cnt, u.id FROM posts p LEFT JOIN users u ON p.user_id = u.id LEFT JOIN coments c ON c.post_id = p.id where p.is_published = 1 GROUP BY p.id ORDER BY p.update_date desc;");
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
                    strcpy(articleNew[i].char_post_id, row[0]);
                    strcpy(articleNew[i].title, row[1]);
                    strcpy(articleNew[i].upload_date, row[2]);
                    strcpy(articleNew[i].user_name, row[3]);
                    strcpy(articleNew[i].comment_conunt, row[4]);
                    strcpy(articleNew[i].visit_cnt, row[5]);
                    strcpy(articleNew[i].user_id, row[6]);
                }        
        }
    }

    
    printf("content-Type: text/html\n\n");  
    printf("<html lang=\"ja\">\n");
    printf("<head>\n");
    printf("<meta charset=\"utf-8\" />\n");
    printf("<title>マイページ</title>");
    printf("<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css\" />");
    printf("<link href=\"allpost.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head>\n");
    printf("<body>\n");
    

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
    // printf("<h1 class=\"banner\">記事一覧</h1>");
    // printf("<img alt=\"mkeiri2fa\" src=\"mkeiri2fa.jpg\" id=\"bigimg\" width=\"1000\" height=\"496\" />\n");
    printf("</div>\n");
    // printf("<ul>\n");
    // printf("<li><img alt=\"mkeiri2f\" src=\"thumb-mkeiri2fa.jpg\" class=\"thumb\" data-image=\"mkeiri2fa.jpg\"></li>\n");
    // printf("<li><img alt=\"mkeiri2f\" src=\"thumb-mkeiri2fb.jpg\" class=\"thumb\" data-image=\"mkeiri2fb.jpg\"></li>\n");
    // printf("</ul>\n");
    printf("</div>\n");
    printf("</section>\n");
    printf("<h1 class=\"site-title\">記事一覧 </h1>\n");
    // printf("<h2 class=\"contents-title\">%sさんの過去記事</h2>\n", session.user_name);
    if (id_count == 0) {
        printf("<h2>まだ記事はありません。</h2>\n");
    }
    else if (id_count > 0) {
        printf("<div class=\"news\">\n");
        // char *char_post_id = malloc(11);
        int temp = 10;
        if (id_count - (page_num-1)*10 < 10){
            temp = id_count - (page_num-1)*10;
        }
        int start = (page_num-1)*10;
        for (int i = start; i < start + temp; i++) {
            // sprintf(char_post_id, "%d", articleNew[i].id);
            // if (check_post_id(conn, &result, &row, char_post_id) == 0){
            //     printf("<h3>%s(管理者よりブラックされた記事です)</h3>",articleNew[i].title);
            //     printf("<div class=\"small_font\">%s</div>", articleNew[i].upload_date);
            //     continue;
            // }
            printf("<form id=\"toLookNew%d\" action=\"look.exe\" method=\"get\">", i);
            printf("<input type=\"hidden\" name=\"post_id\" value=%s> ", articleNew[i].char_post_id);
            printf("</form>");
            printf("<h3><a href=\"#\" onclick=\"document.getElementById('toLookNew%d').submit();\">%s</a><i class=\"far fa-comment small-icon\"></i> %s</h3>", i, articleNew[i].title,articleNew[i].comment_conunt);
            printf("<div class=\"small_font\">%s　<i class=\"far fa-eye\"></i> %s　", articleNew[i].upload_date,articleNew[i].visit_cnt);
            if (strcmp(articleNew[i].user_id,"1") == 0){
                printf("<span class=\"rainbow-text\">%s</span>",articleNew[i].user_name);
            } else {
                printf("%s",articleNew[i].user_name);
            }
            printf("</div>");
          }
        // 10ページごと切り替え
        printf("<div class=\"pageswitch\">");
        int pageswitch = (id_count - 1) / 10;
        for (int i = 0; i < pageswitch + 1; i++)
        {
            printf("<h3 style=\"display:inline\"><a href=allpost.exe?page=%d>%d　</a></h3>", i+1, i+1); 
        }
        printf("</div>");
        printf("</div>\n");
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
         
    free(session_id);
    free(query);
    free(articleNew);
    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}