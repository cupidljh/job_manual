#include <winsock2.h>
#include <winbase.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hdlc.h"

#define BUFSIZE 512

extern void hdlc_dec_init(hdlc_dec *hdlc_decode);
extern void hdlc_enc_init(hdlc_enc *hdlc_encode);
extern void hdlc_encode(char *buf, size_t buflen, hdlc_enc *hdlc_encode);
extern void hdlc_decode(hdlc_enc *hdlc_encode, hdlc_dec *hdlc_decode);
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


int main(int argc, char* argv[])
{
  ///////////////////////     ���� �ʱ�ȭ    ///////////////////////

  WSADATA wsa;
  int retval;
//  char buf[BUFSIZE+1];

  if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
    return -1;
  }
  // ���� �ʱ�ȭ ������ �����ߴٴ� �޽����ڽ� ���
 printf("�����ʱ�ȭ ����\n");
 fflush(stdout);

  ///////////////////////     1. socket() ����    ///////////////////////

  SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(listen_sock == INVALID_SOCKET){
    err_quit("socket()");
  }

  ///////////////////////     2. bind() �Լ�    ///////////////////////

  // ������ ���� IP�ּҿ� ��Ʈ ��ȣ ����
  SOCKADDR_IN serveraddr;
  ZeroMemory(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(9000);
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
  if(retval == SOCKET_ERROR){
    err_quit("bind()");
  }


  ///////////////////////     3. listen() �Լ�    ///////////////////////

  // listen_sock�̶�� ���ϰ� ���յ� ��Ʈ ���¸� �����·� ����
  // SOMAXCONN ����ŭ ���� ���� ����, SOMAXCONN = 0x7fffffff;
  retval = listen(listen_sock, SOMAXCONN);
  // SOCKET_ERROR = -1
  if(retval == SOCKET_ERROR){
    err_quit("listen()");
  }

  ///////////////////////     4. accept() �Լ�    //////////////////////////

  // ������ ��ſ� ����� ����
  SOCKET client_sock;
  SOCKADDR_IN clientaddr;

  hdlc_enc hdlc_enc;
  hdlc_dec hdlc_dec;

  int addrlen;
  int i;

  // Ŭ���̾�Ʈ�� ������ ���
  while(1){
    addrlen = sizeof(clientaddr);
    // ������ Ŭ���̾�Ʈ�� ��Ű��� �ϵ��� ���ο� client_sock�̶�� ���� ����
    client_sock = accept(listen_sock,(SOCKADDR*)&clientaddr, &addrlen);
    // INVALID_SOCKET = ~0
    if(client_sock == INVALID_SOCKET){
      err_display("accept()");
      continue;
    }
    // inet_ntoa(clientaddr.sin_addr) = 32��Ʈ ���ڷ� IP�ּҸ� �Է¹޾�
    //���ڿ� ���·� ����
    // ntohs(clientaddr.sin_port) = ��Ʈ��ũ(�� �����)���� ȣ��Ʈ
    //(��Ʋ �����)�� short���� ��Ʈ��ȣ ����
    // ������ ��Ȯ�� �Ǿ����� Ȯ�� �ϱ� ����(UI�� �����Ƿ�)
    printf("\n[TCP ����] Ŭ���̾�Ʈ ���� : IP�ּ�=%s, ��Ʈ��ȣ=%d\n"
      ,inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    fflush(stdout);


    while(1){
      ///////////////////////     5. recv() �Լ�    ///////////////////////
      // ������ �ޱ�
      // BUFSIZE�� define���� 512
      retval = recv(client_sock, &hdlc_enc, sizeof(hdlc_enc), 0);
      if(retval == SOCKET_ERROR){
        err_display("recv()");
        break;
      }
      else if(retval == 0){
        break;
      }

      // ���� ������ ���
      // ������ ������ ������ NULL�Է�
//      buf[retval] = '\0';

      // inet_ntoa(clientaddr.sin_addr) = 32��Ʈ ���ڷ� IP�ּҸ� �Է¹޾�
      //���ڿ� ���·� ����
      // ntohs(clientaddr.sin_port) = ��Ʈ��ũ(�� �����)���� ȣ��Ʈ
      //(��Ʋ �����)�� short���� ��Ʈ��ȣ ����
      // ������ ��Ȯ�� �Ǿ����� Ȯ�� �ϱ� ����(UI�� �����Ƿ�)
      printf("[TCP/%s:%d] \n",inet_ntoa(clientaddr.sin_addr)
        , ntohs(clientaddr.sin_port));
      fflush(stdout);

      printf("%02X ", hdlc_enc.start_flag);
      fflush(stdout);
      for(i = 0;i<hdlc_enc.size;i++)
      {
    	  printf("%02X ",hdlc_enc.info[i]);
    	  fflush(stdout);
      }
      printf("%02X %02X ", hdlc_enc.crcH,hdlc_enc.crcL);
      fflush(stdout);
      printf("%02X\n",hdlc_enc.finish_flag);
      fflush(stdout);

      hdlc_dec_init(&hdlc_dec);
      hdlc_decode(&hdlc_enc, &hdlc_dec);

      ///////////////////////     6. send() �Լ�  ///////////////////////

      // ������ ������
//      retval = send(client_sock, buf, retval, 0);
//      if(retval == SOCKET_ERROR){
//        err_display("send()");
//        break;
//      }
    }
    ///////////////////////     client_sock �����Լ� �Ҹ�  ///////////////////////
    // Ŭ���̾�Ʈ�� ����ϱ� ���� �����ߴ� client_sock �����Լ� �Ҹ�
    closesocket(client_sock);

    // inet_ntoa(clientaddr.sin_addr) = 32��Ʈ ���ڷ� IP�ּҸ� �Է¹޾� ���ڿ� ���·� ����
    // ntohs(clientaddr.sin_port) = ��Ʈ��ũ(�� �����)���� ȣ��Ʈ(��Ʋ �����)�� short����
    // ��Ʈ��ȣ ����
    // ������ ��Ȯ�� �Ǿ����� Ȯ�� �ϱ� ����(UI�� �����Ƿ�)
    printf("[TCP ����] Ŭ���̾�Ʈ ���� : IP�ּ�=%s, ��Ʈ��ȣ=%d\n"
      ,inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    fflush(stdout);
  }




  ///////////////////////     7. closesocket() �Լ�  ///////////////////////

  // �������� accept������ ������� listen_sock �����Լ� �Ҹ�
  closesocket(listen_sock);


  ///////////////////////     ���� ����    ///////////////////////

  WSACleanup();

  return 0;
}
