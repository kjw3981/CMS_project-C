#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>


int main()
{
    char *session_id = calloc(SESSION_MAX, sizeof(char));
    Session session;
    cookie(session_id);
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    
    connect_sql(&conn);
    
    getSession(conn, &result, &row, session_id, &session);


    // char *user_id = "abdbgefdd";

    printf("Content-Type: text/html\n\n");
    printf("<meta charset=\"UTF-8\">");
    printf("<form name=\"login_form\" action=\"write_process.exe\" method=\"post\">");
    printf("<head>");
    printf("<title>新規投稿</title>");
    printf("<link href=\"write1.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head>");
    printf("<body>");
    printf("<div class=\"write\">");
    printf("<h1>新規投稿</h1>");
    printf("</div>");
    
    // printf("<input type=\"hidden\" name=\"myValue\" value=\"%s\">", session.user_id);
    // printf("<p>ブログ名</p>");
    // printf("<input type=\"title\" name=\"title\" placeholder=\"例) 〇〇ブログ\"><br>");
    printf("<p>タイトル</p>");
    printf("<textarea id=\"title\" class=\"input\" type=\"title\" name=\"title\" cols=\"40\" rows=\"1\" placeholder=\"例) 今日のできごと\" ></textarea><br>");
    printf("<h2 id=\"titleError\"></h2>");
    printf("<p>本文</p>");
    printf("<textarea id=\"mainText\" class=\"input\" name=\"maintext\" cols=\"80\" rows=\"40\" placeholder=\"例) 〇月〇日\"></textarea>");
    printf("<h2 id=\"mainTextError\"></h2>");
    // printf("<div class=\"input_file\">");
    // printf("<div class=\"preview\">");
    // printf("<input accept=\"image/*\" id=\"imgFile\" type=\"file\">");
    // printf("</div>");
    // printf("<p class=\"btn_upload\">");
    // printf("画像ファイルを選択してアップロード");
    // printf("</p>");
    // printf("</div>");
/*
    printf("<p></p>");
    printf("<p>ブログ</p>");
    printf("<input type=\"radio\" name=\"blog\" value=\"blogpublic\"/>公開");
    printf("<input type=\"radio\" name=\"blog\" value=\"blogprivete\"/>非公開");
    printf("<p></p>");
    printf("<p>いいね</p>");
    printf("<input type=\"radio\" name=\"iine\" value=\"iinepublic\"/>公開");
    printf("<input type=\"radio\" name=\"iine\" value=\"iineprivete\"/>非公開");
    printf("<p>コメント</p>");
    printf("<input type=\"radio\" name=\"comment\" value=\"commentpublic\"/>公開");
    printf("<input type=\"radio\" name=\"comment\" value=\"commentprivete\"/>非公開");
    printf("</div>");
*/
    printf("<br><button class=\"contribution-btn\" id=\"login-btn\" onclick=\"titleInput(event), mainTextInput(event);\">投稿</button>");

    printf("<script>");
    printf("'use strict';");
    symbolErrorInit();
    symbolError("title");
    symbolError("mainText");
    printf("</script>");
    printf("</body>");
    printf("</form>");

    return 0;
}