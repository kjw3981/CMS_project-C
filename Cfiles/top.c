#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>
#include <time.h>

#define UPLOAD_DATE_MAX 50
#define TITLE_MAX 100
#define DISPLAY_MAX 5
#define USERNAME_MAX 200

typedef struct {
  char post_id[11];
  char upload_date[UPLOAD_DATE_MAX];
  char title[TITLE_MAX];
  char user_name[USERNAME_MAX];
  char is_published[2];
  char visit_cnt[11];
  char comment_cnt[11];
  char user_id[11];
} Article;

int select_display_posts_orderby_date(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, Article *articleNew)
{
  char *query = (char *)malloc(1000);
  sprintf(query, "select p.id, p.title, p.update_date, u.user_name, p.visit_cnt, COUNT(c.id), u.id from posts p left join users u on p.user_id = u.id left join coments c on c.post_id = p.id where p.is_published = 1 GROUP BY p.id order by p.update_date desc limit %d;", DISPLAY_MAX);
  if (mysql_query(conn, query)) {
      fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
      return 1;
  }

  *result = mysql_store_result(conn);
  if (*result == NULL) {
    fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
    return 1;
  }
  int i = 0;
  while ((*row = mysql_fetch_row(*result)) != NULL) {
      strcpy(articleNew[i].post_id, (*row)[0]);
      strcpy(articleNew[i].title, (*row)[1]);
      strcpy(articleNew[i].upload_date, (*row)[2]);
      strcpy(articleNew[i].user_name, (*row)[3]);
      strcpy(articleNew[i].visit_cnt, (*row)[4]);
      strcpy(articleNew[i].comment_cnt, (*row)[5]);
      strcpy(articleNew[i].user_id, (*row)[6]);
      i++;
  }

  return 0;
}

int select_display_posts_orderby_comments(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, Article *articleReco)
{
  char *query = (char *)malloc(1000);
  sprintf(query, "SELECT p.id, p.title, p.update_date, u.user_name, COUNT(c.id) AS comment_count, p.visit_cnt, u.id  FROM posts p LEFT JOIN users u ON p.user_id = u.id LEFT JOIN coments c ON c.post_id = p.id where p.is_published = 1 GROUP BY p.id ORDER BY comment_count desc limit %d;", DISPLAY_MAX);
  if (mysql_query(conn, query)) {
      fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
      return 1;
  }

  *result = mysql_store_result(conn);
  if (*result == NULL) {
    fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
    return 1;
  }

  int i = 0;
  while ((*row = mysql_fetch_row(*result)) != NULL) {
      strcpy(articleReco[i].post_id, (*row)[0]);
      strcpy(articleReco[i].title, (*row)[1]);
      strcpy(articleReco[i].upload_date, (*row)[2]);
      strcpy(articleReco[i].user_name, (*row)[3]);
      strcpy(articleReco[i].comment_cnt, (*row)[4]);
      strcpy(articleReco[i].visit_cnt, (*row)[5]);
      strcpy(articleReco[i].user_id, (*row)[6]);

      i++;
  }

  return 0;
}

int main(void)
{
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    connect_sql(&conn);
    delete_expiry_session(conn, &result, &row);
    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    Session session;   
    getSession(conn, &result, &row, session_id, &session);
    
    // 全ての記事を格納
    Article* articleReco = malloc(DISPLAY_MAX * sizeof(Article));
    select_display_posts_orderby_comments(conn, &result, &row, articleReco);
    Article *articleNew = malloc(DISPLAY_MAX * sizeof(Article));
    select_display_posts_orderby_date(conn, &result, &row, articleNew);
    

    printf("content-Type: text/html\n\n");  
    printf("<html lang=\"ja\">\n");
    printf("<head>\n");
    printf("<meta charset=\"utf-8\" />\n");
    printf("<title>トップページ</title>\n");
    printf("<link href=\"blogtop4.css\" rel=\"stylesheet\" />\n");
    printf("<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css\" />");
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
    printf("<img alt=\"mkeiri2fa\" src=\"mkeiri2fa.jpg\" id=\"bigimg\" width=\"1000\" height=\"496\" />\n");
    printf("</div>\n");
    printf("<ul>\n");
    printf("<li><img alt=\"mkeiri2f\" src=\"thumb-mkeiri2fa.jpg\" class=\"thumb\" data-image=\"mkeiri2fa.jpg\"></li>\n");
    printf("<li><img alt=\"mkeiri2f\" src=\"thumb-mkeiri2fb.jpg\" class=\"thumb\" data-image=\"mkeiri2fb.jpg\"></li>\n");
    printf("<li><img alt=\"mkeiri2f\" src=\"thumb-mkeiri2fc.jpg\" class=\"thumb\" data-image=\"mkeiri2fc.jpg\"></li>\n");
    printf("<li><img alt=\"mkeiri2f\" src=\"thumb-mkeiri2fd.jpg\" class=\"thumb\" data-image=\"mkeiri2fd.jpg\"></li>\n");
    printf("</ul>\n");
    printf("</div>\n");
    printf("</section>\n");
    printf("<h1 class=\"site-title\">Blog ISITANA</h1>\n");
    printf("<h2 class=\"contents-title\">新着記事</h2>\n");
    printf("<div class=\"news\">\n");

    for (int i = 0; i < DISPLAY_MAX; i++) {

          printf("<form id=\"toLookNew%d\" action=\"look.exe\" method=\"get\">", i);
          printf("<input type=\"hidden\" name=\"post_id\" value=%s> ", articleNew[i].post_id);
          printf("</form>");
          printf("<h3><a href=\"#\" onclick=\"document.getElementById('toLookNew%d').submit();\">%s</a><i class=\"far fa-comment small-icon\"></i> %s</h3>", i, articleNew[i].title, articleNew[i].comment_cnt);
          printf("<div class=\"small_font\">%s　<i class=\"far fa-eye\"></i> %s　", articleNew[i].upload_date, articleNew[i].visit_cnt);
          if (strcmp(articleNew[i].user_id,"1") == 0){
              printf("<span class=\"rainbow-text\">%s</span>",articleNew[i].user_name);
          } else {
              printf("%s",articleNew[i].user_name);
          }
          printf("</div>");
      }

    printf("</div>\n");
    printf("<h2 class=\"contents-title\">おすすめ記事</h2>\n");

    printf("<div class=\"news\">\n");
    
    for (int i = 0; i < DISPLAY_MAX; i++) {
          printf("<form id=\"toLookReco%d\" action=\"look.exe\" method=\"get\">", i);
          printf("<input type=\"hidden\" name=\"post_id\" value=%s> ", articleReco[i].post_id);
          printf("</form>");
          printf("<p>\n");
          printf("<h3><a href=\"#\" onclick=\"document.getElementById('toLookReco%d').submit();\">%s</a><i class=\"far fa-comment small-icon\"></i> %s</h3>", i, articleReco[i].title, articleReco[i].comment_cnt);
          printf("<div class=\"small_font\">%s　<i class=\"far fa-eye\"></i> %s　", articleReco[i].upload_date, articleReco[i].visit_cnt);
          if (strcmp(articleReco[i].user_id,"1") == 0){
              printf("<span class=\"rainbow-text\">%s</span>",articleReco[i].user_name);
          } else {
              printf("%s",articleReco[i].user_name);
          }
          printf("</div>");
          
          printf("</p>\n");
      }
    printf("</div>\n");

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
    free(articleNew);
    free(articleReco);
    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}