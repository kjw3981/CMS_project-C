#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

/**
 * 日本語に直す
 * ※decoded はフリーしないといけない
 * @param encoded 日本語になる前の記号の文字列
 * @param decoded 日本語に変換した後の文字列を指すポインタ、この関数を呼び出す前に宣言する
 * 
*/
void urlDecode(const char* encoded, char *decoded) {
    char temp[3] = {0};
    int j = 0;

    for (size_t i = 0; i < strlen(encoded); i++) {
        if (encoded[i] == '%') {
            temp[0] = encoded[i + 1];
            temp[1] = encoded[i + 2];
            decoded[j++] = strtol(temp, NULL, 16);
            i += 2;
        }
        else if (encoded[i] == '+') {
            decoded[j++] = ' ';
        }
        else {
            decoded[j++] = encoded[i];
        }
    }

    decoded[j] = '\0';

}

int main()
{


/*
    char *user_id = malloc(200);
    strcpy(user_id , "1234");
*/
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    connect_sql(&conn);

    char *session_id = calloc(SESSION_MAX, sizeof(char));
    cookie(session_id);

    Session session;   
    getSession(conn, &result, &row, session_id, &session);


    char *lastnamef;
    char *firstnamef;
    char *lastname;
    char *firstname;
    char *user_name;
    char *pref;
    char *address;
    char *email;
    char *phone;
    char *blog_name;
    char *ssuser_id;

    // データの取得/
    char *data;
    long data_len;

    data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    data = malloc(data_len + 1);
    fgets(data, data_len + 1, stdin);

    // 個人情報 を格納 日本語化
    lastnamef = strtok(data, "&");
    firstnamef = strtok(NULL, "&");
    lastname = strtok(NULL, "&");
    firstname = strtok(NULL, "&");
    user_name = strtok(NULL, "&");
    pref = strtok(NULL, "&");
    address = strtok(NULL, "&");
    email = strtok(NULL, "&");
    phone = strtok(NULL, "&");
    blog_name = strtok(NULL, "&");
    ssuser_id = strtok(NULL, "&");

    lastnamef = strchr(lastnamef, '=') + 1;
    firstnamef = strchr(firstnamef, '=') + 1;
    lastname = strchr(lastname, '=') + 1;
    firstname = strchr(firstname, '=') + 1;
    user_name = strchr(user_name, '=') + 1;
    pref = strchr(pref, '=') + 1;
    address = strchr(address, '=') + 1;
    email = strchr(email, '=') + 1;
    phone = strchr(phone, '=') + 1;
    blog_name = strchr(blog_name, '=') + 1;
    ssuser_id = strchr(ssuser_id, '=') + 1;

    
    char *ulastnamef = malloc(strlen(lastnamef) + 1);
    char *ufirstnamef = malloc(strlen(firstnamef) + 1);
    char *ulastname = malloc(strlen(lastname) + 1);
    char *ufirstname = malloc(strlen(firstname) + 1);
    char *uuser_name = malloc(strlen(user_name) + 1);
    char *upref = malloc(strlen(pref) + 1);
    char *uaddress = malloc(strlen(address) + 1);
    char *uemail = malloc(strlen(email) + 1);
    char *uphone = malloc(strlen(phone) + 1);
    char *ublog_name = malloc(strlen(blog_name) + 1);
    char *uuser_id = malloc(strlen(ssuser_id) + 1);
    

    urlDecode(lastnamef, ulastnamef);
    urlDecode(firstnamef, ufirstnamef);
    urlDecode(lastname, ulastname);
    urlDecode(firstname, ufirstname);
    urlDecode(user_name, uuser_name);
    urlDecode(pref, upref);
    urlDecode(address, uaddress);
    urlDecode(email, uemail);
    urlDecode(phone, uphone);
    urlDecode(blog_name, ublog_name);
    urlDecode(ssuser_id, uuser_id);
    




// // session_user_id　から usertableのidを取得
//     char *user_id = calloc(SESSION_MAX, sizeof(char));
//     char *tmp = malloc(1000);
//     sprintf(tmp, "select user_id from users where user_id = %s;", ssuser_id);
//     if (mysql_query(conn, tmp)) {
//         fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
//         return 1;
//     }

//     result = mysql_store_result(conn);
//     if (result == NULL) {
//         fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
//         return 1;
//     }
//     if ((row = mysql_fetch_row(result)) != NULL){
//         strcpy(user_id, row[0]);
//     }
//     printf("%s", user_id);


    // 記事のタイトル、本文の修正。
    char *query = malloc(10000);
    sprintf(query, "update users set lastnamef = \"%s\", firstnamef = \"%s\", lastname = \"%s\", firstname = \"%s\", user_name = \"%s\", pref = \"%s\", address = \"%s\", email = \"%s\", phone = \"%s\", blog_name = \"%s\" where user_id = \"%s\";", ulastnamef, ufirstnamef, ulastname, ufirstname, uuser_name, upref, uaddress, uemail, uphone, ublog_name, uuser_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
    }

    printf("<Content-Type: text/html>\n\n");
    printf("Location: mypage.exe\r\n\r\n");

    // printf("<!DOCTYPE html>");
    // printf("<html>");
    // printf("<head>\n");
    // printf("<meta charset=\"UTF-8\">\n");
    // printf("<title>ユーザー情報修正完了</title>");
    // printf("<link href=\"correctComment.css\" rel=\"stylesheet\" type=\"text/css\">");
    // printf("</head>");
    // printf("<body>");
    // printf("<h1>ユーザー情報を修正しました。</h1>");
    // printf("<p>マイページにお戻りいただき、引き続きお楽しみください。</p>");
    // printf("<a href=\"http://localhost/cgi-bin/mypage.exe\" class=\"btn-square-shadow\">マイページ</a><br>");
    // printf("</body>");
    // printf("</html>");


    free(data);
    free(query);
    free(ulastnamef);
    free(ufirstnamef);
    free(ulastname);
    free(ufirstname);
    free(upref);
    free(uaddress);
    free(uemail);
    free(uphone);
    free(ublog_name);
    free(uuser_id);
//    free(tmp);

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}