#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\OpenSSL-Win64\include\openssl\sha.h>

#define MAX_HASH_LENGTH 129

// パスワードをハッシュ化する関数
void hashPassword(const char* password, char* hashedPassword) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX sha512;

    SHA512_Init(&sha512);
    SHA512_Update(&sha512, password, strlen(password));
    SHA512_Final(hash, &sha512);

    // 二進数の値の文字列への変換
    int i;
    for (i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
        sprintf(&hashedPassword[i * 2], "%02x", hash[i]);
    }
    hashedPassword[SHA512_DIGEST_LENGTH * 2] = '\0';
}


//     // Example usage
//     const char* password = "mypassword";
//     char hashedPassword[MAX_HASH_LENGTH];

//     hashPassword(password, hashedPassword);

//     printf("Hashed Password: %s\n", hashedPassword);




/**
 * 日本語に直す
 * ※decoded はフリーしないといけない
 * @param encoded 日本語になる前の記号の文字列
 * @param decoded 日本語に変換した後の文字列を指すポインタ、この関数を呼び出す前に宣言する
 * 
*/
char *urlDecode(const char* encoded, char *decoded) {
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

    return decoded;
}

int insert(char *data, char *send_user_id)
{
    
    // SQLへ接続 開始処理
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    conn = mysql_init(NULL);
    if (conn == NULL) {
        printf("Failed to initialize MySQL connection\n");
        return 1;
    }

    if (mysql_real_connect(conn, "localhost", "root", "1111", "project", 0, NULL, 0) == NULL) {
        printf("Failed to connect to MySQL server: %s\n", mysql_error(conn));
        return 1;
    }
    // SQLへ接続 開始処理途中

    // lastnamefを格納 日本語化
    char *lastnamefStill = strtok(data, "&");
    lastnamefStill = strchr(lastnamefStill, '=') + 1;


    char *lastnamef = malloc(strlen(lastnamefStill) + 1);
    urlDecode(lastnamefStill, lastnamef);

    // firstnamefを格納 日本語化
    char *firstnamefStill = strtok(NULL, "&");
    firstnamefStill = strchr(firstnamefStill, '=') + 1;

    char *firstnamef = malloc(strlen(firstnamefStill) + 1);
    urlDecode(firstnamefStill, firstnamef);

    // lastnameを格納 日本語化
    char *lastnameStill = strtok(NULL, "&");
    lastnameStill = strchr(lastnameStill, '=') + 1;

    char *lastname = malloc(strlen(lastnameStill) + 1);
    urlDecode(lastnameStill, lastname);

    // firstnameを格納 日本語化
    char *firstnameStill = strtok(NULL, "&");
    firstnameStill = strchr(firstnameStill, '=') + 1;

    char *firstname = malloc(strlen(firstnameStill) + 1);
    urlDecode(firstnameStill, firstname);

    // user_nameを格納 日本語化
    char *user_nameStill = strtok(NULL, "&");
    user_nameStill = strchr(user_nameStill, '=') + 1;

    char *user_name = malloc(strlen(user_nameStill) + 1);
    urlDecode(user_nameStill, user_name);

    // user_idを格納 
    char *user_id = strtok(NULL, "&");
    user_id = strchr(user_id, '=') + 1;

    strcpy(send_user_id, user_id);

    // password1を格納
    char *password1 = strtok(NULL, "&");
    password1 = strchr(password1, '=') + 1;
    
    // password2を格納
    char *password2 = strtok(NULL, "&");
    password2 = strchr(password2, '=') + 1;

    // prefを格納 日本語化
    char *prefStill = strtok(NULL, "&");
    prefStill = strchr(prefStill, '=') + 1;
    
    char *pref = malloc(strlen(prefStill) + 1);
    urlDecode(prefStill, pref);

    // addressを格納 日本語化
    char *addressStill = strtok(NULL, "&");
    addressStill = strchr(addressStill, '=') + 1;

    char *address = malloc(strlen(addressStill) + 1);
    urlDecode(addressStill, address);

    // emailを格納
    char *emailStill = strtok(NULL, "&");
    emailStill = strchr(emailStill, '=') + 1;

    char *email = malloc(strlen(emailStill) + 1);
    urlDecode(emailStill, email);

    // phoneを格納 文字列のまま
    char *phone = strtok(NULL, "&");
    phone = strchr(phone, '=') + 1;

    // birthyearを格納 文字列のまま
    char *birthyear = strtok(NULL, "&");
    birthyear = strchr(birthyear, '=') + 1;

    // birthmonthを格納 文字列のまま
    char *birthmonth = strtok(NULL, "&");
    birthmonth = strchr(birthmonth, '=') + 1;

    // birthdayを格納 文字列のまま
    char *birthday = strtok(NULL, "&");
    birthday = strchr(birthday, '=') + 1;

    // genderを格納 数字
    char *gender = strtok(NULL, "&");
    gender = strchr(gender, '=') + 1;
    
    int genderInt = atoi(gender);
    // password1のハッシュ化
    char hashedPassword[MAX_HASH_LENGTH];
    hashPassword(password1, hashedPassword);

    char *query = malloc(1000);
    sprintf(query, "insert into users (lastnamef, firstnamef, lastname, firstname, user_name, user_id, password, pref, address, email, phone, birthdate, gender, blog_name) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s%s%s', %d, '%s');", 
    lastnamef, firstnamef, lastname, firstname, user_name, user_id, hashedPassword, pref, address, email, phone, birthyear, birthmonth, birthday, genderInt, user_name);

    if (mysql_query(conn, query)) {
        printf("Failed to execute query\n");
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }
    
    // SQL文が正常に作動したか？
    // ここを通ってしまうのでコメントアウト
    result = mysql_store_result(conn);
    if (result == NULL) {
        // printf("Failed to get query result: %s\n", mysql_error(conn));
        // return 1;
    }

    free(lastnamef);
    free(firstnamef);
    free(lastname);
    free(firstname);
    free(user_name);
    free(pref);
    free(address);

    free(query);
    
    // 後始末処理
    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}

int main(void) 
{


    // 入力フォームからデータ受け取り 開始
    char *content_length_str = getenv("CONTENT_LENGTH");
    int content_length = atoi(content_length_str);

    char *data = malloc(content_length + 1);
    fgets(data, content_length + 1, stdin);
    // 入力フォームからデータ受け取り 終了

    char *send_user_id = malloc(100);

    if (insert(data, send_user_id) == 0) {

    printf("content-Type: text/html\r\n");
    printf("Location: login.html\r\n\r\n");
//   printf("<head>");
//   printf("<title>登録完了</title>");
//   printf(" <meta charset=\"UTF-8\">");
//   printf("<link href=\"completion.css\" rel=\"stylesheet\" type=\"text/css\">");

//   printf("</head>");
//   printf("<body>");
//   printf("<h1>登録が完了しました</h1>");
//     printf("<br>");
//     printf("<br>");
//     printf("<br>");
//     printf("<br>");

//   printf("<p>ご登録ありがとうございました。<br></p>");
//     printf("<p>マイページより新規投稿などお楽しみください。<br></p>");
//     printf("<p>他のアカウントの投稿検索などは、ホーム画面よりをお楽しみください。</p>");

//    printf("<form id=\"toMyPageForm\" action=\"mypage.exe\" method=\"post\">");
//     printf("<input type=\"hidden\" name=\"user_id\" value=\"%s\">", send_user_id);
//     printf("</form>");
//     printf("<a href=\"#\" class=\"btn-square-shadow\" onclick=\"document.getElementById('toMyPageForm').submit();\">マイページ</a>");
  
//   printf("<a href=\"リンク先のURL\" class=\"btn-square-shadow\">ホーム</a><br>");

//     printf("<a href=\"リンク先URL\">プライバシーポリシー</a><br>");
//   printf("<a href=\"リンク先URL\">お問い合わせ</a><br>");
//   printf("<a href=\"リンク先URL\">利用規約</a><br>");
  
//   printf("</body>");
//   printf("</html>");

  
    }
    else {
        printf("error");
    }
   
    

    // printf("</body></html>");

    free(data);
    free(send_user_id);

  return 0;
}