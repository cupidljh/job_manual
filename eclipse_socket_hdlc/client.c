#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hdlc.h"

#define BUFSIZE 512

extern void hdlc_enc_init(hdlc_enc *hdlc_encode);
extern void hdlc_encode(char *buf, size_t buflen, hdlc_enc *hdlc_encode);
extern unsigned short compute_fcs(unsigned char *data, int length);

///////////////////////     에러 발생 종료 함수    ///////////////////////

void err_quit(char *msg)
{
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)&lpMsgBuf, 0, NULL);
  MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
  LocalFree(lpMsgBuf);
  exit(-1);
}

///////////////////////     에러 발생 출력 함수    ///////////////////////

void err_display(char *msg)
{
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)&lpMsgBuf, 0, NULL);
  printf("[%s]%s", msg, (LPCTSTR)lpMsgBuf);
  LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char *buf, int len, int flags)
{
  int received;
  char *ptr = buf;
  int left = len;

  while(left > 0){
    received = recv(s, ptr, left, flags);
    if(received == SOCKET_ERROR){
      return SOCKET_ERROR;
    }
    else if(received == 0){
      break;
    }
    left -= received;
    ptr += received;
  }
  return (len - left);
}

int main(int argc, char* argv[])
{
  ///////////////////////     윈속 초기화    ///////////////////////

  WSADATA wsa;
  int retval;

  if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
    return -1;
  }
  // 윈속 초기화 성공시 성공했다는 메시지박스 출력
  printf("윈속초기화 성공\n");
  fflush(stdout);
  ///////////////////////     1. socket() 생성    ///////////////////////

  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == INVALID_SOCKET){
    err_quit("socket()");
  }

  ///////////////////////     2. connect() 함수    ///////////////////////

  SOCKADDR_IN serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(9000);
  // 127.0.0.1 = 하나의 컴퓨터상에서 서버와 클라이언트를 실행한다는 가정하에 자신의
  // 주소를 참조하는 roll back
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
  if(retval == SOCKET_ERROR){
    err_quit("connect()");
  }

  ///////////////////////     3. send() 함수    ///////////////////////

//  int len;

  hdlc_enc hdlc_enc;
  unsigned int buflen;
  char buf[16];
  FILE *in;

  if(( in = fopen(argv[1], "rb")) == NULL){
	  fputs("file error\n", stderr);
	  exit(1);
  }

  hdlc_enc_init(&hdlc_enc);

  while((buflen = fread(buf, sizeof(char), sizeof(buf), in)) != '\0')
  {
	  hdlc_encode(buf, buflen, &hdlc_enc);
    // 데이터 입력
//    ZeroMemory(buf, sizeof(buf));

    printf("\n[보낼 데이터]\n");
    fflush(stdout);
//    if(fgets(buf, BUFSIZE+1, stdin) == NULL){
//      break;
//    }

    /*
    len = strlen(buf);

    // '\n'문자 제거하고 NULL문자로 체움
    if(buf[len-1] == '\n'){
      buf[len-1] = '\0';
    }
    if(strlen(buf) == 0){
      break;
    }
    */

    // 데이터 보내기
    retval = send(sock, &hdlc_enc, sizeof(hdlc_enc), 0);
    if(retval == SOCKET_ERROR){
      err_display("send()");
      break;
    }

    // retval = 보낸 데이터의 크기
    // 접속이 정확히 되었는지 확인 하기 위함(UI가 없으므로)
    printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n",retval);
    fflush(stdout);

    ///////////////////////     4. recv() 함수    ///////////////////////

    // 데이터 받기
    /*
    retval = recvn(sock, buf, retval, 0);
    if(retval == SOCKET_ERROR){
      err_display("recv()");
      break;
    }
    else if(retval == 0){
      break;
    }
    buf[retval] = '\0';

    // 받은 데이터 출력
    // retval = 받은 데이터의 크기
    // 접속이 정확히 되었는지 확인 하기 위함(UI가 없으므로)
    printf("[TCP 클라이언트] %d바이트를 받았습니다.\n",retval);
    fflush(stdout);
    // buf = 받은 데이터
    printf("[받은 데이터] %s\n",buf);
    fflush(stdout);
    */
  }


  ///////////////////////    5. closesocket() 함수    ///////////////////////

  // sock 소켓함수 소멸
  closesocket(sock);


  ///////////////////////     윈속 종료    ///////////////////////

  WSACleanup();

  return 0;
}
