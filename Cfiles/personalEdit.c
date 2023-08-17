#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
#include <c:\Program Files\MySQL\MySQL Server 8.0\include\method.h>

typedef struct {
    char user_id[USER_ID_MAX];
} Id;

int main(void)
{
  printf("Content-Type: text/html\n\n");
  printf("<meta charset=\"UTF-8\">");

/*
  // データの取得/
  char *data;
  long data_len;

  data_len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
  data = malloc(data_len + 1);
  fgets(data, data_len + 1, stdin);

  char *user_id;
  user_id = strtok(data, "&");
  user_id = strchr(user_id, '=') + 1;    

  char *userid = malloc(200);
  strcpy(userid , "3");
*/

  MYSQL *conn;
  MYSQL_RES *result;
  MYSQL_ROW row;

  connect_sql(&conn);

  char *session_id = calloc(SESSION_MAX, sizeof(char));
  cookie(session_id);

  Session session;   
  getSession(conn, &result, &row, session_id, &session);

  char *user_id = calloc(SESSION_MAX, sizeof(char));
  strcpy(user_id, session.user_user_id);

    // id数の取得
  if (mysql_query(conn, "SELECT COUNT(*) FROM users")) {
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
  if (id_count == -1) {
    fprintf(stderr, "ERROR: id_count == -1\n");
    return 1;
  }
    // id数の取得

  Id *id = malloc(id_count * sizeof(Id));

  if (mysql_query(conn, "SELECT user_id FROM users")) {
      printf("Failed to execute query: %s\n", mysql_error(conn));
      return 1;
  }

  result = mysql_store_result(conn);
  if (result == NULL) {
    printf("Failed to get query result: %s\n", mysql_error(conn));
    return 1;
  }

  int idIndex = 0;
  while ((row = mysql_fetch_row(result)) != NULL) {
    strcpy(id[idIndex].user_id, row[0]);
    idIndex++;
  }

  char *lastnamef = malloc(200);
  char *firstnamef = malloc(200);
  char *lastname = malloc(200);
  char *firstname = malloc(200);
  char *user_name = malloc(200);
  char *password = malloc(200);
  char *pref = malloc(200);
  char *address = malloc(200);
  char *email = malloc(200);
  char *phone = malloc(200);
  char *blog_name = malloc(200);

    // ユーザー情報　取得。
  char *query = malloc(1000);
  sprintf(query, "select lastnamef, firstnamef, lastname, firstname, user_name, password, pref, address, email, phone, blog_name from users where user_id = \"%s\";", user_id);
  if (mysql_query(conn, query)) {
    fprintf(stderr, "Failed to execute query: %s\n", mysql_error(conn));
    return 1;
  }

  result = mysql_store_result(conn);
  if (result == NULL) {
    printf("Failed to get query result: %s\n", mysql_error(conn));
    return 1;
  }

  if ((row = mysql_fetch_row(result)) != NULL){
    strcpy(lastnamef, row[0]);
    strcpy(firstnamef, row[1]);   
    strcpy(lastname, row[2]);
    strcpy(firstname, row[3]);
    strcpy(user_name, row[4]);
    strcpy(password, row[5]);
    strcpy(pref, row[6]);
    strcpy(address, row[7]);
    strcpy(email, row[8]);
    strcpy(phone, row[9]);
    strcpy(blog_name, row[10]);
  }
  printf("<html>\n");
  printf("<form name=\"per_update_form\" action=\"perIntermediation.exe\" method=\"post\">\n");
  printf("<head>\n");
  printf("<link href=\"personalEdit1.css\" rel=\"stylesheet\" type=\"text/css\">\n");
  printf("<title>新規登録</title>\n");
  printf("</head>\n");
  printf("<body>\n");
  printf("<div class=\"per_update_form\">\n");
  printf("<h1>登録情報変更</h1>\n");
  printf("<h2>変更後、更新ボタンへ</h2>\n");
  printf("</div>\n");
  printf("<div class=\"per_update_form\">\n");
  printf("<p>氏名フリガナ(セイ)　　　　　　(メイ)</p><p2>必須</p2>\n");
  printf("<input class=\"name_input\" type=\"text\" id=\"lastnamef\" name=\"lastnamef\" value=\"%s\"><input class=\"name_input\" type=\"text\" id=\"firstnamef\" name=\"firstnamef\" value=\"%s\">", lastnamef, firstnamef);
  printf("<br><br>");
  printf("<h5 id=\"namefError\"></h5>\n");
  printf("<p>氏名(姓)　　　　　　　　　　　(名)</p><p2>必須</p2>\n");
  printf("<input class=\"name_input\" type=\"text\" id=\"lastname\" name=\"lastname\" value=\"%s\"><input class=\"name_input\" type=\"text\" id=\"firstname\" name=\"firstname\" value=\"%s\">", lastname, firstname);
  printf("<br><br>\n");
  printf("<h5 id=\"nameError\"></h5>\n");
  printf("<p>ユーザーネーム(1文字以上20文字以内)</p><p2>必須</p2>\n");
  printf("<input class=\"name_input\" type=\"user_name\" id=\"user_name\" name=\"user_name\" value=\"%s\"><br>\n", user_name);
  printf("<h5 id=\"user_nameError\"></h5>\n");
//    printf("<p>ユーザー ID(半角英数字8文字以上20文字以内)</p><p2>必須</p2>\n");
//    printf("<input type=\"user_id\" id=\"user_id\" name=\"user_id\" value=\"%s\"><br>\n", user_id);
//    printf("<h5 id=\"user_idError\"></h5>\n");
/*
  printf("<p>パスワード(英数字8文字以上20文字以内)</p><p2>必須</p2>\n");
  printf("<input type=\"password1\" id=\"password1\" name=\"password1\"value=\"%s\">\n", password);
  printf("<h5 id=\"password1Error\"></h5>\n");
  printf("<p>パスワード確認(再度入力)</p><p2>必須</p2>\n");
  printf("<input type=\"password2\" id=\"password2\" name=\"password2\"placeholder=\"パスワード確認\">\n");
  printf("<h5 id=\"password2Error\"></h5>\n");
*/
  printf("<p>住所</p><p2>必須</p2>\n");
  printf("<select name=\"pref\" class=\"pref_select\">\n");
  printf("<option >%s</option>\n", pref);
  printf("<option value=\"北海道\">北海道</option>\n");
  printf("<option value=\"青森県\">青森県</option>\n");
  printf("<option value=\"岩手県\">岩手県</option>\n");
  printf("<option value=\"宮城県\">宮城県</option>\n");
  printf("<option value=\"秋田県\">秋田県</option>\n");
  printf("<option value=\"山形県\">山形県</option>\n");
  printf("<option value=\"福島県\">福島県</option>\n");
  printf("<option value=\"茨城県\">茨城県</option>\n");
  printf("<option value=\"栃木県\">栃木県</option>\n");
  printf("<option value=\"群馬県\">群馬県</option>\n");
  printf("<option value=\"埼玉県\">埼玉県</option>\n");
  printf("<option value=\"千葉県\">千葉県</option>\n");
  printf("<option value=\"東京都\">東京都</option>\n");
  printf("<option value=\"神奈川県\">神奈川県</option>\n");
  printf("<option value=\"新潟県\">新潟県</option>\n");
  printf("<option value=\"富山県\">富山県</option>\n");
  printf("<option value=\"石川県\">石川県</option>\n");
  printf("<option value=\"福井県\">福井県</option>\n");
  printf("<option value=\"山梨県\">山梨県</option>\n");
  printf("<option value=\"長野県\">長野県</option>\n");
  printf("<option value=\"岐阜県\">岐阜県</option>\n");
  printf("<option value=\"静岡県\">静岡県</option>\n");
  printf("<option value=\"愛知県\">愛知県</option>\n");
  printf("<option value=\"三重県\">三重県</option>\n");
  printf("<option value=\"滋賀県\">滋賀県</option>\n");
  printf("<option value=\"京都府\">京都府</option>\n");
  printf("<option value=\"大阪府\">大阪府</option>\n");
  printf("<option value=\"兵庫県\">兵庫県</option>\n");
  printf("<option value=\"奈良県\">奈良県</option>\n");
  printf("<option value=\"和歌山県\">和歌山県</option>\n");
  printf("<option value=\"鳥取県\">鳥取県</option>\n");
  printf("<option value=\"島根県\">島根県</option>\n");
  printf("<option value=\"岡山県\">岡山県</option>\n");
  printf("<option value=\"広島県\">広島県</option>\n");
  printf("<option value=\"山口県\">山口県</option>\n");
  printf("<option value=\"徳島県\">徳島県</option>\n");
  printf("<option value=\"香川県\">香川県</option>\n");
  printf("<option value=\"愛媛県\">愛媛県</option>\n");
  printf("<option value=\"高知県\">高知県</option>\n");
  printf("<option value=\"福岡県\">福岡県</option>\n");
  printf("<option value=\"佐賀県\">佐賀県</option>\n");
  printf("<option value=\"長崎県\">長崎県</option>\n");
  printf("<option value=\"熊本県\">熊本県</option>\n");
  printf("<option value=\"大分県\">大分県</option>\n");
  printf("<option value=\"宮崎県\">宮崎県</option>\n");
  printf("<option value=\"鹿児島県\">鹿児島県</option>\n");
  printf("<option value=\"沖縄県\">沖縄県</option>\n");
  printf("</select>\n");
  printf("<input class=\"pref_input\" type=\"address\" id=\"address\" name=\"address\" value=\"%s\">\n", address);
  printf("<h5 id=\"addressError\"></h5>\n");
  printf("<p>メールアドレス</p><p2>必須</p2>\n");
  printf("<input class=\"name_input\" type=\"email\" id=\"email\" name=\"email\" value=\"%s\">\n", email);
  printf("<h5 id=\"emailError\"></h5>\n");
  printf("<p>電話番号</p><p2>必須</p2>\n");
  printf("<input class=\"name_input\" type=\"phone\" id=\"phone\" name=\"phone\" value=\"%s\">\n", phone);
  printf("<h5 id=\"phoneError\"></h5>\n");
/*
   printf("<label>\n");
      printf("<p>誕生日</p>\n");
      printf("</label>\n");
      printf("<p></p>\n");
      printf("<select name=\"birthyear\" id=\"birthyear\">\n");
      printf("<option value=\"%s\"></option>\n", birthdate);
      for (int year = 1923; year <= 2030; year++) {
          printf("<option value=\"%d\">%d</option>\n", year, year);
      }
      printf("</select>\n");
      printf("</select> 年\n");
        printf("<select name=\"birthmonth\" id=\"birthmonth\">\n");
        printf("<option value=\"%s\"></option>\n", birthdate);
        for (int month = 1; month <= 12; month++) {
            printf("<option value=\"%02d\">%d</option>\n", month, month);
        }
        printf("</select> 月\n");
        printf("<select name=\"birthday\" id=\"birthday\">\n");
        printf("<option value=\"%s\"></option>\n", birthdate);
        for (int day = 1; day <= 31; day++) {
            printf("<option value=\"%02d\">%d</option>\n", day, day);
        }
        printf("</select> 日\n");
        printf("<h5 id=\"birthdayError\"></h5>\n");

        printf("<p>性別</p></p>\n");
        printf("<div>\n");
        printf("<input type=\"radio\" name=\"gender\" value=\"0\"/>男\n");
        printf("<input type=\"radio\" name=\"gender\" value=\"1\"/>女\n");
        printf("<input type=\"radio\" name=\"gender\" value=\"2\"/>その他\n");
        printf("</div>\n");
        printf("</select>\n");
        printf("<h5 id=\"genderError\"></h5>\n");
        printf("</div>\n");
*/
  printf("<p>ブログ名</p>\n");
  printf("<p2>必須</p2>");
  printf("<input class=\"name_input\" id=\"blog_name\" type=\"text\" id=\"blog_name\" name=\"blog_name\" value=\"%s\">\n", blog_name);
  printf("<h5 id=\"blog_nameError\"></h5>");
  printf("<p></p>\n");
  printf("<input type=\"hidden\" id=\"id\" name=\"user_id\" value=\"%s\">", user_id);
  printf("<button type=\"registration\" action=\"perIntermediation.exe\" onclick=\"namefInput(event), nameInput(event), user_nameInput(event), addressInput(event), emailInput(event), phoneInput(event), blog_nameInput(event);\">更新</button>\n");
  printf("</form>\n");


// jSコード
    printf("<script>\n");
    printf("function symbolError(input) {\n");
    printf("var forbiddenChars = [\"'\", '\"', ';', '-', '/', '*', ':', '%%', '_', '&', '='];\n");
    printf("for (var i = 0; i < forbiddenChars.length; i++) {\n");
    printf("if (input.includes(forbiddenChars[i])) {\n");
    printf("return true; // エラーを検出\n");
    printf("}\n");
    printf("}\n");
    printf("return false; // エラーを検出しない\n");
    printf("}\n");
    printf("// フリガナのチェック\n");
    printf("function namefInput(event) {\n");
    printf("var lastnamefInput = document.getElementById(\"lastnamef\")\n");
    printf("var firstnamefInput = document.getElementById(\"firstnamef\")\n");
    printf("var lastnamef = lastnamefInput.value;\n");
    printf("var firstnamef = firstnamefInput.value;\n");
    printf("var regex = /^[\u30A0-\u30FFァ-ンー\s]+$/;\n");
    printf("var isValidL = regex.test(lastnamef);\n");
    printf("var isValidF = regex.test(firstnamef);\n");
    printf("var namefError = document.getElementById(\"namefError\");\n");
    printf("if (lastnamefInput.value === \"\" || firstnamefInput.value === \"\") {\n");
    printf("event.preventDefault();\n");
    printf("namefError.textContent = \"氏名フリガナが未入力です\";\n");
    printf("}\n");
    printf("else if (!isValidL || !isValidF) {\n");
    printf("event.preventDefault();\n");
    printf("namefError.textContent = \"カタカナ以外の文字が含まれています\";\n");
    printf("}\n");
    printf("else {\n");
    printf("namefError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
    printf("function nameInput(event) {\n");
    printf("var lastnameInput = document.getElementById(\"lastname\");\n");
    printf("var firstnameInput = document.getElementById(\"firstname\");\n");
    printf("var nameError = document.getElementById(\"nameError\");\n");
    printf("if (lastnameInput.value === \"\" || firstnameInput.value === \"\") {\n");
    printf("event.preventDefault();\n");
    printf("nameError.textContent = \"氏名が未入力です\";\n");
    printf("return;\n");
    printf("} else if (symbolError(lastnameInput.value) || symbolError(firstnameInput.value)) {\n");
    printf("event.preventDefault();\n");
    printf("nameError.textContent = \"特殊記号文字が含まれています\";\n");
    printf("} else {\n");
    printf("nameError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
    printf("function user_nameInput(event) {\n");
    printf("var user_nameInput = document.getElementById(\"user_name\");\n");
    printf("var user_nameError = document.getElementById(\"user_nameError\");\n");
    printf("var user_name = user_nameInput.value;\n");
    printf("if (user_name.length === 0) {\n");
    printf("event.preventDefault();\n");
    printf("user_nameError.textContent = \"ユーザネームが未入力です\";\n");
    printf("} else if (user_name.length > 20) {\n");
    printf("event.preventDefault();\n");
    printf("user_nameError.textContent = \"20文字以内で入力してください\";\n");
    printf("} else if (symbolError(user_nameInput.value)) {\n");
    printf("event.preventDefault();\n");
    printf("user_nameError.textContent = \"特殊記号文字が含まれています\";\n");
    printf("} else {\n");
    printf("user_nameError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
    printf("function user_idInput(event) {\n");
    printf("var user_idInput = document.getElementById(\"user_id\");\n");
    printf("var user_idError = document.getElementById(\"user_idError\");\n");
    printf("var user_id = user_idInput.value;\n");
    printf("var regex = /^[a-zA-Z0-9]{8,20}$/;\n");
    printf("var isValid = regex.test(user_id);\n");
    printf("if (user_id.length === 0) {\n");
    printf("event.preventDefault();\n");
    printf("user_idError.textContent = \"ユーザIDが未入力です\";\n");
    printf("} else if (!isValid) {\n");
    printf("event.preventDefault();\n");
    printf("user_idError.textContent = \"半角英数字で8文字以上20文字以内で入力してください\";\n");
    printf("} else {\n");
    for (int i = 0; i < id_count; i++) {
      printf("if (user_id === \"%s\") {\n", id[i].user_id);
      printf("event.preventDefault();\n");
      printf("user_idError.textContent = \"このユーザIDは登録済みです\";\n");
      printf("return;\n");
      printf("}\n");
    }
    printf("user_idError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
/*
    printf("function passwordInput(event) {\n");
    printf("var password1Input = document.getElementById(\"password1\");\n");
    printf("var password1Error = document.getElementById(\"password1Error\");\n");
    printf("var password2Input = document.getElementById(\"password2\");\n");
    printf("var password2Error = document.getElementById(\"password2Error\");\n");
    printf("var password1 = password1Input.value;\n");
    printf("var password2 = password2Input.value;\n");
    printf("var regex = /^[a-zA-Z0-9]{8,20}$/;\n");
    printf("var isValid = regex.test(password1);\n");
    printf("if (password1.length === 0) {\n");
    printf("event.preventDefault();\n");
    printf("password1Error.textContent = \"パスワードが未入力です\";\n");
    printf("} else if (!isValid) {\n");
    printf("event.preventDefault();\n");
    printf("password1Error.textContent = \"半角英数字で8文字以上20文字以内で入力してください\";\n");
    printf("} else {\n");
    printf("password1Error.textContent = \"\";\n");
    printf("}\n");
    printf("if (password2.length === 0) {\n");
    printf("event.preventDefault();\n");
    printf("password2Error.textContent = \"確認パスワードが未入力です\";\n");
    printf("} else if (password1Input.value !== password2Input.value) {\n");
    printf("event.preventDefault();\n");
    printf("password2Error.textContent = \"パスワードが一致しません\";\n");
    printf("} else {\n");
    printf("password2Error.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
*/
    printf("function addressInput(event) {\n");
    printf("var addressInput = document.getElementById(\"address\");\n");
    printf("var addressError = document.getElementById(\"addressError\");\n");
    printf("if (addressInput.value === \"\") {\n");
    printf("event.preventDefault();\n");
    printf("addressError.textContent = \"市町村番地が未入力です\";\n");
    printf("}");
    printf("else if (symbolError(addressInput.value)) {");
    printf("event.preventDefault();\n");
    printf("addressError.textContent = \"特殊記号文字が含まれています\";\n");
    printf("} else {\n");
    printf("addressError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
    printf("function emailInput(event) {\n");
    printf("var emailInput = document.getElementById(\"email\");\n");
    printf("var emailError = document.getElementById(\"emailError\");\n");
    printf("var email = emailInput.value;\n");
    printf("var regex = /^[^\\s@]+@[^\\s@]+\\.[^\\s@]+$/;\n");
    printf("var isValid = regex.test(email);\n");
    printf("if (emailInput.value === \"\") {\n");
    printf("event.preventDefault();\n");
    printf("emailError.textContent = \"メールアドレスが未入力です\";\n");
    printf("} else if (!isValid) {\n");
    printf("event.preventDefault();\n");
    printf("emailError.textContent = \"メールアドレスを正しく入力してください\";\n");
    printf("} else if (symbolError(emailInput.value)) {\n");
    printf("event.preventDefault();\n");
    printf("emailError.textContent = \"特殊記号文字が含まれています\";\n");
    printf("} else {\n");
    printf("emailError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
    printf("function phoneInput(event) {\n");
    printf("var phoneInput = document.getElementById(\"phone\");\n");
    printf("var phoneError = document.getElementById(\"phoneError\");\n");
    printf("var phone = phoneInput.value;\n");
    printf("var regex = /^[0-9]{10,11}$/;\n");
    printf("var isValid = regex.test(phone);\n");
    printf("if (phoneInput.value === \"\") {\n");
    printf("event.preventDefault();\n");
    printf("phoneError.textContent = \"電話番号が未入力です\";\n");
    printf("} else if (!isValid) {\n");
    printf("event.preventDefault();\n");
    printf("phoneError.textContent = \"正しい電話番号を入力してください\";\n");
    printf("} else {\n");
    printf("phoneError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
    
    printf("function blog_nameInput(event) {\n");
    printf("var blog_nameInput = document.getElementById(\"blog_name\");\n");
    printf("var blog_nameError = document.getElementById(\"blog_nameError\");\n");
    printf("if (blog_nameInput.value === \"\") {\n");
    printf("event.preventDefault();\n");
    printf("blog_nameError.textContent = \"ブログ名が未入力です\";\n");
    printf("}");
    printf("else if (symbolError(blog_nameInput.value)) {");
    printf("event.preventDefault();\n");
    printf("blog_nameError.textContent = \"特殊記号文字が含まれています\";\n");
    printf("} else {\n");
    printf("blog_nameError.textContent = \"\";\n");
    printf("}\n");
    printf("}\n");
    
    
/*
        printf("function birthdayInput(event) {\n");
        printf("var birthyearInput = document.getElementById(\"birthyear\");\n");
        printf("var birthmonthInput = document.getElementById(\"birthmonth\");\n");
        printf("var birthdayInput = document.getElementById(\"birthday\");\n");
        printf("var birthdayError = document.getElementById(\"birthdayError\");\n");
        printf("if (birthyearInput.value === \"\") {\n");
        printf("event.preventDefault();\n");
        printf("birthdayError.textContent = \"年が選択されていません\";\n");
        printf("} else if (birthmonthInput.value === \"\") {\n");
        printf("event.preventDefault();\n");
        printf("birthdayError.textContent = \"月が選択されていません\";\n");
        printf("} else if (birthdayInput.value === \"\") {\n");
        printf("event.preventDefault();\n");
        printf("birthdayError.textContent = \"日が選択されていません\";\n");
        printf("} else {\n");
        printf("birthdayError.textContent = \"\";\n");
        printf("}\n");
        printf("}\n");
        printf("function genderInput(event) {\n");
        printf("var genderInput = document.getElementsByName(\"gender\");\n");
        printf("var genderError = document.getElementById(\"genderError\");\n");
        printf("var selectedGender = null;\n");
        printf("for (var i = 0; i < genderInput.length; i++) {\n");
        printf("if (genderInput[i].checked) {\n");
        printf("selectedGender = genderInput[i].value;\n");
        printf("break;\n");
        printf("}\n");
        printf("}\n");
        printf("if (selectedGender === null) {\n");
        printf("event.preventDefault();\n");
        printf("genderError.textContent = \"性別が選択されていません\";\n");
        printf("} else {\n");
        printf("genderError.textContent = \"\";\n");
        printf("}\n");
        printf("}\n");
*/
  printf("</script>\n");
  printf("</body>\n");
  printf("</html>\n");

    free(lastnamef);
    free(firstnamef);
    free(lastname);
    free(firstname);
    free(user_name);
    free(password);
    free(pref);
    free(address);
    free(email);
    free(phone);
    free(blog_name);

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}