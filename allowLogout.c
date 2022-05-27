#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <winsock2.h>
#define CONNECTION_INFO "dbname=hostServerdb user=postgres password=root client_encoding=sjis"

void allowLogout(int userId) { 
  PGconn *connection; // db接続に必要な情報が入った構造体の変数
  PGresult *response; // SQLコマンドを実行した時の結果を格納する変数

  char buffSql[256]; // SQLコマンド文を格納しておくバッファ

  connection = PQconnectdb(CONNECTION_INFO); // dbに接続要求を行う
  if (PQstatus(connection) != CONNECTION_OK) {
    printf(stderr, "Connection failed: %s CON_INFO:%s\n", PQerrorMessage());
  }

  // SQL実行文を格納
  sprintf(buffSql, "UPDATE t_login SET login_flag = 0 where user_id = '%d'", userId);

  // SQL文を実行
  response = PQexec(connection, buffSql);
  if (PQresultStatus(response) != PGRES_COMMAND_OK) {
    fprintf(stderr, "UPDATE failed : %s", PQerrorMessage(connection));
    PQclear(response);
    PQfinish(connection);
    exit(EXIT_FAILURE);
  }

  printf("%dがログアウトしました\n", userId);
}