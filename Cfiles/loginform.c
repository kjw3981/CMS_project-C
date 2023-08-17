#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <C:\Program Files\OpenSSL-Win64\include\openssl\sha.h>

#define MAX_HASH_LENGTH 129

// session_id生成。ランダムで15文字
void generateSessionID(char *session_id) {
    // Generate a random session ID
    // You can implement your own random ID generation logic here
    // For simplicity, this example uses a fixed-length random string
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < 15; ++i) {
        int index = rand() % (sizeof(charset) - 1);
        session_id[i] = charset[index];
    }

    session_id[15] = '\0';
}
// OpenSSlのコードぽい
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

int main(void)
{
    srand(time(NULL));

    // データの取得/
    char *data;
    long data_len;

    data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    data = malloc(data_len + 1);
    fgets(data, data_len + 1, stdin);

    char *user_id;
    char *password;

    user_id = strtok(data, "&");
    password = strtok(NULL, "&");

    user_id = strchr(user_id, '=') + 1;
    password = strchr(password, '=') + 1;
    
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    
    // my_sql_connect(conn);
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Failed to initialize MySQL connection\n");
        return 1;
    }

    if (mysql_real_connect(conn, "localhost", "root", "1111", "project", 0, NULL, 0) == NULL) {
        fprintf(stderr, "Failed to connect to MySQL server: %s\n", mysql_error(conn));
        return 1;
    }
    
    char *db_hashed_password = malloc(200);
    // select_username_id(conn, result, row ,user_name, user_id ,id);
    char *query = malloc(1000);
    sprintf(query, "select password from users where user_id = '%s';", user_id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
        return 1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
        return 1;
    }

    if ((row = mysql_fetch_row(result)) != NULL){
        strcpy(db_hashed_password, row[0]);
    }    

    // insert into sessions (session_id, user_id, expiry_time) values ('aaa', 4, now() + interval 3 hour);
    char hashedPassword[MAX_HASH_LENGTH];
    hashPassword(password, hashedPassword);
    if (strcmp(hashedPassword, db_hashed_password) == 0){
        // printf("Location: success.exe\r\n\r\n");
        printf("Content-type: text/html\r\n");
        
        char *session_id = (char *)malloc(16 * sizeof(char));
        generateSessionID(session_id);
        
        printf("Set-Cookie: session_id=%s\r\n",session_id);
                
        // userの id 取得
        char *id = malloc(200);
        // select_username_id(conn, result, row ,user_name, user_id ,id);
        sprintf(query, "select id from users where user_id = '%s';", user_id);
        if (mysql_query(conn, query)) {
            fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
            return 1;
        }

        result = mysql_store_result(conn);
        if (result == NULL) {
            fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
            return 1;
        }

        if ((row = mysql_fetch_row(result)) != NULL){
            strcpy(id, row[0]);
        }        

        // sessionsテーブルに情報を追加
        sprintf(query, "insert into sessions (session_id, user_id, expiry_time) values ('%s', %s, now() + interval 24 hour);",session_id,id);
        if (mysql_query(conn, query)) {
            fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
            return 1;
        }
        result = mysql_store_result(conn);
        
        if (result == NULL) {
            // fprintf(stderr, "Failed to get query result: %s\n", mysql_error(conn));
            // return 1;
        }
        
        free(session_id);
        free(id);

        printf("Location: top.exe\r\n\r\n");
    } else {
        
        printf("Content-type: text/html\r\n");
        printf("Location: login.exe\r\n\r\n");

    }
 
    mysql_free_result(result);
    mysql_close(conn);

    free(data);
    free(query);
    free(db_hashed_password);

    return 0;
}