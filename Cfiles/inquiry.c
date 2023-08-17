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
    printf("<form name=\"login_form\" action=\"inquiry_process.exe\" method=\"post\">");
    printf("<head>");
    printf("<title>お問い合わせページ</title>");
    printf("<link href=\"inquiry.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("</head>");
    printf("<div class=\"write\">");
    printf("<h1>お問い合わせ</h1>");
    printf("</div>");
    printf("<div class=\"login_form_btm\">");
    // printf("<input type=\"hidden\" name=\"myValue\" value=\"%s\">", session.user_id);
    // printf("<p>ブログ名</p>");
    // printf("<input type=\"title\" name=\"title\" placeholder=\"例) 〇〇ブログ\"><br>");
    printf("<p>タイトル</p>");
    printf("<input id=\"title\" class=\"input\" type=\"title\" name=\"title\" placeholder=\" 発生した事案\" ><br>");
    printf("<h2 id=\"titleError\"></h2>");
    printf("<p>本文</p>");
    printf("<textarea id=\"mainText\" rows=\"4\" class=\"input\" name=\"maintext\" placeholder=\" 日付や内容\"></textarea>");
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
    printf("<br><button id=\"login-btn\" type=\"submit\" onclick=\"titleInput(event), mainTextInput(event);\">送信</button>");
    printf("<body>");
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