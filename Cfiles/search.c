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

int main(void) {

    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

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

    if (id_count > 0) {     
        
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
    printf("<html>\n");
    printf("    <head>\n");
    printf("<meta charset=\"utf-8\" />\n");
    printf("        <title>記事検索</title>\n");
    printf("<link href=\"search.css\" rel=\"stylesheet\" type=\"text/css\">");
    printf("<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css\" />");
    printf("    </head>\n");
    printf("    <body>\n");

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
    printf("<h1 class=\"site-title\">記事検索 </h1>\n");

    if (id_count == 0) {
        printf("<h2>まだ記事はありません。</h2>\n");
    }
    else if (id_count > 0) {
        printf("        <input type=\"text\" id=\"user_name\" placeholder=\"著者名\">\n");
        printf("        <button onclick=\"user_nameCheck();\">検索</button> \n");
        printf("        <br>\n");
        printf("        <br>\n");
        printf("        <input type=\"text\" id=\"title\" placeholder=\"タイトル名\">\n");
        printf("        <button onclick=\"titleCheck();\">検索</button>\n");
        printf("        <h5 class=\"Error\" id=\"myTableError\"></h5>\n");
        printf("        <table id=\"myTable\">\n");
        printf("            <!-- ここに項目を追加する -->\n");
        printf("        </table>\n");
    }

    
    printf("        <script>\n");
    printf("            'use strict';\n");
    printf("            function tableReset() {\n");
    printf("                const table = document.getElementById(\"myTable\");\n");
    printf("                while (table.firstChild) {\n");
    printf("                    table.removeChild(table.firstChild);\n");
    printf("                }\n");
    printf("            }\n");

    printf("            const Article = [];\n");

    for(int i = 0; i < id_count; i++) {
        printf("Article.push({ user_name: '%s', title: '%s', upload_date: '%s',  comment_conunt: '%s', post_id: '%s', visit_cnt: '%s', user_id: '%s' });\n", 
            articleNew[i].user_name, articleNew[i].title, articleNew[i].upload_date, articleNew[i].comment_conunt, articleNew[i].char_post_id, articleNew[i].visit_cnt, articleNew[i].user_id);
    }
/*
    printf("            Article.push({ user_name: 'わらびもち', title: '日本はいい天気です' });\n");
    printf("            Article.push({ user_name: 'サーモン', title: '日本は元気' });\n");
    printf("            Article.push({ user_name: 'タナカ', title: 'お寿司大好き' });\n");
    printf("            Article.push({ user_name: 'わらび', title: 'サーモンたまらん' });\n");
*/
    printf("            function user_nameCheck() {\n");
    printf("                var input = document.getElementById('user_name').value;\n");
    printf("                if (input === \"\") {\n");
    printf("                    tableReset();\n");
    printf("                    document.getElementById(\"myTableError\").innerText = \"未入力のまま検索はできません\";\n");
    printf("                    return;\n");
    printf("                }\n");
    printf("                const regex = new RegExp(input);\n");
    printf("                let displayIndex = [%d];\n", id_count);
    printf("                let count = 0;\n");
    printf("                for (let i = 0; i < %d; i++) {\n", id_count);
    printf("                    const stringMatch = regex.test(Article[i][\"user_name\"]);\n");
    printf("                    if (stringMatch) {\n");
    printf("                        displayIndex[count] = i;\n");
    printf("                        count++;\n");
    printf("                    }\n");
    printf("                }\n");
    printf("                if (count === 0) {\n");
    printf("                    document.getElementById(\"myTableError\").innerText = \"著者名'\" + input + \"'の検索結果 0件\";\n");
    printf("                    tableReset();\n");
    printf("                }\n");
    printf("                else {\n");
    printf("                    document.getElementById(\"myTableError\").innerText = \"著者名'\" + input + \"'の検索結果 \" + count + \"件\";\n");
    printf("                    tableReset();\n");
    printf("                    const table = document.getElementById(\"myTable\");\n");
    printf("                    for (let i = 0; i < count; i++) {\n");
    printf("                        const tr1 = document.createElement(\"tr\");\n"); 
    printf("                        const tr2 = document.createElement(\"tr\");\n"); 
    printf("                        const td1 = document.createElement(\"td\");\n");
    printf("                        const td2 = document.createElement(\"td\");\n");
    printf("                        const td3 = document.createElement(\"td\");\n"); 
    printf("                        const link = document.createElement(\"a\");\n");
    printf("                        const icon1 = document.createElement(\"i\");\n"); 
    printf("                        const icon2 = document.createElement(\"i\");\n"); 
    printf("icon1.className = \"far fa-comment small-icon\";"); 
    printf("icon2.className = \"far fa-eye small-icon\";"); 
    printf("                        link.href = \"look.exe?post_id=\" + Article[displayIndex[i]][\"post_id\"]\n");
    printf("                        link.textContent = Article[displayIndex[i]][\"title\"];\n");
    printf("                        const comentText = document.createTextNode(\" \"+ Article[displayIndex[i]][\"comment_conunt\"]);\n");
    printf("const dateText = document.createTextNode(Article[displayIndex[i]][\"upload_date\"] + \"　\");"); 
    printf("const visitText = document.createTextNode(\" \" + Article[displayIndex[i]][\"visit_cnt\"] + \"　\");\n"); 
    printf("const nameText = document.createTextNode(Article[displayIndex[i]][\"user_name\"]);"); 
    printf("link.className = \"link-class\";");
    printf("td1.appendChild(link);\n");
    printf("td1.appendChild(icon1);\n"); 
    printf("td1.appendChild(comentText);\n");
    printf("td2.appendChild(dateText);\n"); 
    printf("td2.appendChild(icon2);\n"); 
    printf("td2.appendChild(visitText);\n"); 
    printf("td3.appendChild(nameText);\n"); 
    printf("td3.classList.add(\"small_font\");");
    printf("if (Article[displayIndex[i]][\"user_id\"] === \"1\") {"); 
    printf("td3.classList.add(\"rainbow-text\");"); 
    printf("}"); 
    printf("td2.classList.add(\"small_font\");");
    printf("                        tr1.appendChild(td1);\n"); 
    printf("                        tr2.appendChild(td2);\n"); 
    printf("                        tr2.appendChild(td3);\n");
    printf("                        table.appendChild(tr1);\n"); 
    printf("                        table.appendChild(tr2);\n"); 
    printf("                    }\n");
    printf("                }\n");
    printf("            }\n");

    printf("            function titleCheck() {\n");
    printf("                var input = document.getElementById('title').value;\n");
    printf("                if (input === \"\") {\n");
    printf("                    tableReset();\n");
    printf("                    document.getElementById(\"myTableError\").innerText = \"未入力のまま検索はできません\";\n");
    printf("                    return;\n");
    printf("                }\n");
    printf("                const regex = new RegExp(input);\n");
    printf("                let displayIndex = [%d];\n", id_count);
    printf("                let count = 0;\n");
    printf("                for (let i = 0; i < %d; i++) {\n", id_count);
    printf("                    const stringMatch = regex.test(Article[i][\"title\"]);\n");
    printf("                    if (stringMatch) {\n");
    printf("                        displayIndex[count] = i;\n");
    printf("                        count++;\n");
    printf("                    }\n");
    printf("                }\n");
    printf("                if (count === 0) {\n");
    printf("                    document.getElementById(\"myTableError\").innerText = \"タイトル名'\" + input + \"'の検索結果 0件\";\n");
    printf("                    tableReset();\n");
    printf("                }\n");
    printf("                else {\n");
    printf("                    document.getElementById(\"myTableError\").innerText = \"タイトル名'\" + input + \"'の検索結果 \" + count + \"件\";\n");
    printf("                    tableReset();\n");
    printf("                    const table = document.getElementById(\"myTable\");\n");

    printf("                    for (let i = 0; i < count; i++) {\n");
    printf("                        const tr1 = document.createElement(\"tr\");\n"); 
    printf("                        const tr2 = document.createElement(\"tr\");\n"); 
    printf("                        const td1 = document.createElement(\"td\");\n");
    printf("                        const td2 = document.createElement(\"td\");\n");
    printf("                        const td3 = document.createElement(\"td\");\n"); //
    printf("                        const link = document.createElement(\"a\");\n");
    printf("                        const icon1 = document.createElement(\"i\");\n"); 
    printf("                        const icon2 = document.createElement(\"i\");\n"); 
    printf("icon1.className = \"far fa-comment small-icon\";"); 
    printf("icon2.className = \"far fa-eye small-icon\";"); 
    printf("                        link.href = \"look.exe?post_id=\" + Article[displayIndex[i]][\"post_id\"]\n");
    printf("                        link.textContent = Article[displayIndex[i]][\"title\"];\n");
    printf("                        const comentText = document.createTextNode(\" \" + Article[displayIndex[i]][\"comment_conunt\"]);\n");
    printf("const dateText = document.createTextNode(Article[displayIndex[i]][\"upload_date\"] + \"　\");");
    printf("const visitText = document.createTextNode(\" \" + Article[displayIndex[i]][\"visit_cnt\"] + \"　\");\n"); 
    printf("const nameText = document.createTextNode(Article[displayIndex[i]][\"user_name\"]);"); 
    printf("link.className = \"link-class\";");
    printf("                        td1.appendChild(link);\n");
    printf("td1.appendChild(icon1);\n");
    printf("                        td1.appendChild(comentText);\n");
    printf("td2.appendChild(dateText);\n"); 
    printf("td2.appendChild(icon2);\n"); 
    printf("td2.appendChild(visitText);\n"); 
    printf("td3.appendChild(nameText);\n"); 
    printf("td3.classList.add(\"small_font\");");
    printf("if (Article[displayIndex[i]][\"user_id\"] === \"1\") {"); 
    printf("td3.classList.add(\"rainbow-text\");"); 
    printf("}"); 
    printf("td2.classList.add(\"small_font\");");
    printf("                        tr1.appendChild(td1);\n"); 
    printf("                        tr2.appendChild(td2);\n");
    printf("                        tr2.appendChild(td3);\n"); 
    printf("                        table.appendChild(tr1);\n"); 
    printf("                        table.appendChild(tr2);\n"); 
    printf("                    }\n");
    printf("                }\n");
    printf("            }\n");
    printf("    </script>");
    printf("</body>");
    printf("</html>");

    return 0;
}

   