#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hdlc.h"

#define BUFSIZE 512

extern void hdlc_enc_init(hdlc_enc *hdlc_encode);
extern void hdlc_encode(char *buf, size_t buflen, hdlc_enc *hdlc_encode);
extern unsigned short compute_fcs(unsigned char *data, int length);

///////////////////////     ���� �߻� ���� �Լ�    ///////////////////////

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

///////////////////////     ���� �߻� ��� �Լ�    ///////////////////////

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
  ///////////////////////     ���� �ʱ�ȭ    ///////////////////////

  WSADATA wsa;
  int retval;

  if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
    return -1;
  }
  // ���� �ʱ�ȭ ������ �����ߴٴ� �޽����ڽ� ���
  printf("�����ʱ�ȭ ����\n");
  fflush(stdout);
  ///////////////////////     1. socket() ����    ///////////////////////

  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == INVALID_SOCKET){
    err_quit("socket()");
  }

  ///////////////////////     2. connect() �Լ�    ///////////////////////

  SOCKADDR_IN serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(9000);
  // 127.0.0.1 = �ϳ��� ��ǻ�ͻ󿡼� ������ Ŭ���̾�Ʈ�� �����Ѵٴ� �����Ͽ� �ڽ���
  // �ּҸ� �����ϴ� roll back
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
  if(retval == SOCKET_ERROR){
    err_quit("connect()");
  }

  ///////////////////////     3. send() �Լ�    ///////////////////////

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
    // ������ �Է�
//    ZeroMemory(buf, sizeof(buf));

    printf("\n[���� ������]\n");
    fflush(stdout);
//    if(fgets(buf, BUFSIZE+1, stdin) == NULL){
//      break;
//    }

    /*
    len = strlen(buf);

    // '\n'���� �����ϰ� NULL���ڷ� ü��
    if(buf[len-1] == '\n'){
      buf[len-1] = '\0';
    }
    if(strlen(buf) == 0){
      break;
    }
    */

    // ������ ������
    retval = send(sock, &hdlc_enc, sizeof(hdlc_enc), 0);
    if(retval == SOCKET_ERROR){
      err_display("send()");
      break;
    }

    // retval = ���� �������� ũ��
    // ������ ��Ȯ�� �Ǿ����� Ȯ�� �ϱ� ����(UI�� �����Ƿ�)
    printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n",retval);
    fflush(stdout);

    ///////////////////////     4. recv() �Լ�    ///////////////////////

    // ������ �ޱ�
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

    // ���� ������ ���
    // retval = ���� �������� ũ��
    // ������ ��Ȯ�� �Ǿ����� Ȯ�� �ϱ� ����(UI�� �����Ƿ�)
    printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n",retval);
    fflush(stdout);
    // buf = ���� ������
    printf("[���� ������] %s\n",buf);
    fflush(stdout);
    */
  }


  ///////////////////////    5. closesocket() �Լ�    ///////////////////////

  // sock �����Լ� �Ҹ�
  closesocket(sock);


  ///////////////////////     ���� ����    ///////////////////////

  WSACleanup();

  return 0;
}
