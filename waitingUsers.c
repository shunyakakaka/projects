#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT_NO_SERVER ((unsigned short)65432)

#define BUFFSIZE 128

// ログイン要請が来た時に受け取るデータの構造体
typedef struct {
  char userId[BUFFSIZE];
  char password[BUFFSIZE];
} USERID_AND_PASSWORD_INFO;

void allowLogin(USERID_AND_PASSWORD_INFO pUser);
void allowLogout(int userId);

int waitingUsers(void) {
  WSADATA data;

  int ret; // 全てソケット通信に関する実行結果はこの変数に格納される

  SOCKET sockListen; // 接続待ちソケット記述子
  SOCKET sockConnect; // 接続ソケット

  struct sockaddr_in addrSrv = {0}; // ホストサーバーのアドレス情報を格納する構造体
  struct sockaddr_in addrCli = {0}; // クライアントサーバーのアドレス情報を格納する構造体
  int lenAddr = sizeof(struct sockaddr_in); // sockaddr_in構造体のサイズ

  int numRecv; // 受信したバイト数

  USERID_AND_PASSWORD_INFO requestLoginOrLogoutUserInfo; // ログイン要求が来た時に受け取るバッファ
  int lenRequestLoginOrLogoutUserInfo = sizeof(requestLoginOrLogoutUserInfo);

  ret = WSAStartup(MAKEWORD(2,0), &data);
  if (ret != 0) {
    fprintf(stderr, "socket error (WSAStartup) : code=%d\n", ret);
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  // 接続待ちソケット
  sockListen = socket(AF_INET, SOCK_STREAM, 0);
  if (sockListen == INVALID_SOCKET) {
    fprintf(stderr, "socket error (socket) : code=%d\n", WSAGetLastError());
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  // サーバーのアドレス情報を格納していく
  addrSrv.sin_family = AF_INET; // ネットワークの種類をIPv4に設定
  addrSrv.sin_addr.s_addr = htonl(INADDR_ANY); // どのIPアドレスからも受け付けるように設定
  addrSrv.sin_port = htons(PORT_NO_SERVER); // ポート番号を設定

  // ローカルアドレスの割り当て
  ret = bind(sockListen, (struct sockaddr *)&addrSrv, sizeof(addrSrv));
  if (sockConnect == INVALID_SOCKET) {
    fprintf(stderr, "socket error (recv) : code=%d\n", WSAGetLastError());
    closesocket(sockListen);
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  // 接続要求を受け付ける状態で待機する
  ret = listen(sockListen, SOMAXCONN);
  if (ret == SOCKET_ERROR) {
    fprintf(stderr, "socket error (listen) : code=%d\n", WSAGetLastError());
    closesocket(sockListen);
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  // 接続待ち受け

  while(1) {
    printf("Connect Waiting...\n");
    fflush(stdout);
    sockConnect = accept(sockListen, (struct sockaddr *)&addrCli, &lenAddr);
    if (sockConnect == INVALID_SOCKET) {
      fprintf(stderr, "socket error (accept) : code=%d\n", WSAGetLastError());
      continue;
    }
    // ログインかログアウトかのメッセージを受信
    numRecv = recv(sockConnect, &requestLoginOrLogoutUserInfo, lenRequestLoginOrLogoutUserInfo, 0);
    if (numRecv == SOCKET_ERROR) {
      fprintf(stderr, "socket error (recv) : code=%d\n", WSAGetLastError());
      closesocket(sockConnect);
      continue;
    }


    if (strlen(requestLoginOrLogoutUserInfo.password) != 0) {
      allowLogin(requestLoginOrLogoutUserInfo);
    } else {
      allowLogout(atoi(requestLoginOrLogoutUserInfo.userId));
    }
  }

  return 0;
}
