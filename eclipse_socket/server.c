#include<winsock2.h>
#include<winbase.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define BUFSIZE 512

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
  char buf[BUFSIZE+1];

  if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
    return -1;
  }
  // ���� �ʱ�ȭ ������ �����ߴٴ� �޽����ڽ� ���
  MessageBox(NULL, "�����ʱ�ȭ ����","����",MB_OK);


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
  int addrlen;


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



    while(1){
      ///////////////////////     5. recv() �Լ�    ///////////////////////

      // ������ �ޱ�
      // BUFSIZE�� define���� 512
      retval = recv(client_sock, buf, BUFSIZE, 0);
      if(retval == SOCKET_ERROR){
        err_display("recv()");
        break;
      }
      else if(retval == 0){
        break;
      }

      // ���� ������ ���
      // ������ ������ ������ NULL�Է�
      buf[retval] = '\0';

      // inet_ntoa(clientaddr.sin_addr) = 32��Ʈ ���ڷ� IP�ּҸ� �Է¹޾�
      //���ڿ� ���·� ����
      // ntohs(clientaddr.sin_port) = ��Ʈ��ũ(�� �����)���� ȣ��Ʈ
      //(��Ʋ �����)�� short���� ��Ʈ��ȣ ����
      // ������ ��Ȯ�� �Ǿ����� Ȯ�� �ϱ� ����(UI�� �����Ƿ�)
      printf("[TCP/%s:%d] %s\n",inet_ntoa(clientaddr.sin_addr)
        , ntohs(clientaddr.sin_port), buf);


      ///////////////////////     6. send() �Լ�  ///////////////////////

      // ������ ������
      retval = send(client_sock, buf, retval, 0);
      if(retval == SOCKET_ERROR){
        err_display("send()");
        break;
      }
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
  }




  ///////////////////////     7. closesocket() �Լ�  ///////////////////////

  // �������� accept������ ������� listen_sock �����Լ� �Ҹ�
  closesocket(listen_sock);


  ///////////////////////     ���� ����    ///////////////////////

  WSACleanup();

  return 0;
}
