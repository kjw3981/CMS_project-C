#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>

#define SESSION_MAX 16
#define NAME_MAX 100
#define PASSWORD_MAX 200
#define DATE_MAX 20
#define USER_ID_MAX 11

int check_cookie(char *session_id, MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row);

typedef struct {   
    char user_id[USER_ID_MAX];
    char user_user_id[NAME_MAX];
    char user_name[NAME_MAX];
} Session;

typedef struct {
  char id[11];
  char title[200];
  char contents[10000];
  char blog_name[200];
  char user_id[200];
} Post;

// mysql 初期化
int connect_sql(MYSQL **conn)
{
    *conn = mysql_init(NULL);
    if (*conn == NULL) {
        fprintf(stderr, "Failed to initialize MySQL connection\n");
        return 1;
    }

    if (mysql_real_connect(*conn, "localhost", "root", "1111", "project", 0, NULL, 0) == NULL) {
        fprintf(stderr, "Failed to connect to MySQL server: %s\n", mysql_error(*conn));
        return 1;
    }
    return 0;
}

/**
 * ※session_idの中身が無いなら何もしない
*/
int getSession(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *session_id, Session *session)
{
    if (session_id[0] != '\0') { // session_idが\0ならcookieないからゲストとしての処理になる
        if (check_cookie(session_id, conn, result, row) == 0){ // cookieあるならsession_idもってくる
            char *query = (char *)malloc(1000);
        
            sprintf(query, "select u.id, u.user_id, u.user_name from sessions as s left join users as u on s.user_id = u.id where s.session_id = '%s'", session_id);
            if (mysql_query(conn, query)) {
                fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
                free(query);
                return 1;
            }

            *result = mysql_store_result(conn);
            if (*result == NULL) {
                fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
                free(query);
                return 1;
            }

            if ((*row = mysql_fetch_row(*result)) != NULL){
                strcpy(session->user_id, (*row)[0]);
                strcpy(session->user_user_id, (*row)[1]);
                strcpy(session->user_name, (*row)[2]);
            }

            free(query);
        } else {
            session_id[0] = '\0';
        }
    }
    
    return 0;
}

/* メイン関数での宣言
    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);
*/

/**
 * cookieからユーザID(sessionID)を取得してそのcharポインタを返す
 * @param session_id cookieを格納する、不要になったらfreeする必要がある
 * @retval \0 ゲストログイン
*/
void cookie(char *session_id)
{
    char* cookieData = getenv("HTTP_COOKIE");

    if (cookieData != NULL) {
        // Split the cookie string into individual cookies
        char* cookie = strtok(cookieData, "; ");
        while (cookie != NULL) {
            // Split each cookie into name and value
            char* equalsPos = strchr(cookie, '=');
            if (equalsPos != NULL) {
                *equalsPos = '\0';  // Null-terminate the name
                char* name = cookie;
                char* value = equalsPos + 1;  // Move to the value

                // Compare the cookie name with "session_id"
                if (strcmp(name, "session_id") == 0) {
                    // Cookie found, do something
                    strcpy(session_id, value);
                }
            }
            cookie = strtok(NULL, "; ");
        }
    }
}

/**
 * ヘッダー画面の表示(<body>タグの先頭で呼び出す)
 * ※session_idの中身が無いなら何もしない
 * @param session_id cookieを格納する(※不要になったらfreeする必要がある)
*/
void header(char *session_id , char *session_user_name)
{   
    printf("<head><link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css\"></head>");
    printf("<header class=\"header\">\n");
    printf("<div class=\"blog-logo\">\n");
    printf("<a href=\"top.exe\">\n");
    printf("<img alt=\"いしたなBlogロゴ\" id=\"site-top\" src=\"blog-logo.png\" width=\"500\" height=\"100\" />\n");
    printf("</a>\n");
    printf("</div>\n");
    printf("<nav class=\"global-nav\">\n");
    printf("<ul>\n");

    if (session_id[0] == '\0') {
        printf("<li><a href=\"consent.html\">新規登録</a></li>\n");
        printf("<li><a href=\"login.exe\">ログイン</a></li>\n");
    } else {
        printf("<li class=\"notChange\"> ようこそ、%sさん</li>\n", session_user_name);
        printf("<li><a href=\"mypage.exe\"><i class=\"fas fa-home\"></i> マイページ</a></li>\n");
        printf("<div class=\"menu\">\n");
        printf("<li onclick=\"toggleDropdown(event)\">メニュー</li>\n");
        printf("<div class=\"dropdown\">\n");
        printf("<li><a href=\"allpost.exe\">記事一覧</a></li>\n");
        printf("<li><a href=\"search.exe\">記事検索</a></li>\n");
        printf("<li><a href=\"write.exe\">新規投稿</a></li>\n");
        printf("<li><a href=\"personalLogout.exe\">登録情報の変更</a></li>\n");
        printf("<li><a href=\"inquiry.exe\">お問い合わせ</a></li>");
        
        // printf("<li><a href=\"リンク未定\">ログアウト</a></li>\n");
        printf("<li><a href=\"#\" onclick=\"document.getElementById('logout').submit();\">ログアウト</a></li>\n");
        printf("<form id=\"logout\" action=\"logout.exe\" method=\"post\">");
        printf("<input type=\"hidden\" name=\"session_id\" value=\"%s\">", session_id);
        printf("</form>");
        printf("</div>\n");
        printf("</div>\n");
    }

    printf("</nav>\n");
    printf("</header>\n");
}



/**
 * onclick=\"namefInput(event);\">のように呼び出し namefに当たる部分にid名、複数呼び出す場合カンマで区切る
 * inputするところのidを"id名"で引数に渡す
 * 入力欄の下にinputErrorを無表示タグで
 * 関数群は<body>タグ内に
 * 
 * (例)
 * printf("<script>\n");
 * symbolErrorInit();
 * symbolError("id名"); ←必要な分だけ呼び出し
 * printf("</script>\n");
 * 
*/
// 特殊記号をチェック。一致した場合エラー
void symbolErrorInit(void)
{
    printf("function symbolError(input) {\n");
    printf("var forbiddenChars = [\"'\", '\"', ';', '-', '/', '*', ':', '%%', '_', '&', '='];\n");
    printf("for (var i = 0; i < forbiddenChars.length; i++) {\n");
    printf("if (input.includes(forbiddenChars[i])) {\n");
    printf("return true; // エラーを検出\n");
    printf("}\n");
    printf("}\n");
    printf("return false; // エラーを検出しない\n");
    printf("}\n");
}
// 入力に対しての処理
void symbolError(char *input)
{
    printf("function %sInput(event) {\n", input);
    printf("var inputInput = document.getElementById(\'%s\');\n", input);
    printf("var inputError = document.getElementById(\'%sError\');\n", input);
    printf("if (inputInput.value === \"\") {\n");
    printf("event.preventDefault();\n");
    printf("inputError.textContent = \"未入力です\";\n");
    printf("}");
    printf("else if (symbolError(inputInput.value)) {\n"); 
    printf("event.preventDefault();\n");
    printf("inputError.textContent = \"特殊記号文字が含まれています\";\n");
    printf("} else {\n");
    printf("inputError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
}
// getenvで住所持ってきて中身(post_id)のみを取り出す
void get_post_id(char *post_id)
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
            if (strcmp(name, "post_id") == 0) {
                // Parameter found, store the value
                strcpy(post_id, value);
                // post_id = value;
                break;
            }
        }

        param = strtok(NULL, "&");
        }
    }
}

/**
 * ※session_idがデータベースにないなら、login.htmlに飛ばす
*/
// session_idがデータベースにあるかチェック
int check_cookie(char *session_id, MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row)
{
    char *query = (char *)malloc(1000);
    sprintf(query, "select user_id from sessions where session_id = '%s';", session_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }

    free(query);
    
    if ((*row = mysql_fetch_row(*result)) == NULL){

        return 1;
        // printf("Content-type: text/html\r\n");
        // printf("Location: login.html\r\n\r\n");
    }
    return 0;
}

/**
 * ※時間が過ぎた全てのsessionを削除
*/
int delete_expiry_session(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row){
    char *query = (char *)malloc(1000);
    sprintf(query, "delete from sessions where expiry_time < now();"); // 最初データベースでexpiry_timeを+24時間してるからそれより今が小さいなら消す
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
    }

    free(query);
    return 0;    
}

/**
 * 閲覧ページ専用リダイレクト
 * @param post_id 
*/
void redirect(char *post_id)
{
    printf("Content-type: text/html\r\n");
    printf("Location: look.exe?post_id=%s\r\n\r\n", post_id);
}



int check_admin(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *user_id){
    if (strcmp(user_id, "1")){
        printf("Content-type: text/html\r\n");
        printf("Location: login.html\r\n\r\n");
    }
    return 0;
}


/**
 * ※user_idがpublishedじゃない場合0を返す, publishedだったら1を返す
*/
// ブロック(管理者権限での表示非表示)のための関数
int check_user_id(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *user_id){

    char *query = (char *)malloc(1000);
    sprintf(query, "select is_published from users where id = %s", user_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }
    char *now = (char *)malloc(2);

    if ((*row = mysql_fetch_row(*result)) != NULL){
        strcpy(now, (*row)[0]);
    }

    // 1の場合 0を返す
    if (strcmp(now, "1") == 0){
        
        free(query);
        free(now);
        return 1;
    }

    free(now);
    free(query);    
    
    return 0;
}

/**
 * ※post_idがpublishedじゃない場合0を返す, publishedだったら1を返す
*/
int check_post_id(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *post_id){

    char *query = (char *)malloc(1000);
    sprintf(query, "select is_published from posts where id = %s", post_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }
    char *now = (char *)malloc(2);

    if ((*row = mysql_fetch_row(*result)) != NULL){
        strcpy(now, (*row)[0]);
    }

    // 1の場合 0を返す
    if (strcmp(now, "1") == 0){
        
        free(query);
        free(now);
        return 1;
    }

    free(now);
    free(query);    
    
    return 0;
}

/**
 * ※comment_idがpublishedじゃない場合0を返す, publishedだったら1を返す
*/
int check_comment_id(MYSQL *conn, MYSQL_RES **result, MYSQL_ROW *row, char *comment_id){

    char *query = (char *)malloc(1000);
    sprintf(query, "select is_published from coments where id = %s", comment_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    *result = mysql_store_result(conn);
    if (*result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }
    char *now = (char *)malloc(2);

    if ((*row = mysql_fetch_row(*result)) != NULL){
        strcpy(now, (*row)[0]);
    }

    // 1の場合 0を返す
    if (strcmp(now, "1") == 0){
        
        free(query);
        free(now);
        return 1;
    }

    free(now);
    free(query);    
    
    return 0;
}