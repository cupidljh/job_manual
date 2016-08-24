#include<winsock2.h>
#include<winbase.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define BUFSIZE 512

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


int main(int argc, char* argv[])
{
  ///////////////////////     윈속 초기화    ///////////////////////

  WSADATA wsa;
  int retval;
  char buf[BUFSIZE+1];

  if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
    return -1;
  }
  // 윈속 초기화 성공시 성공했다는 메시지박스 출력
  MessageBox(NULL, "윈속초기화 성공","성공",MB_OK);


  ///////////////////////     1. socket() 생성    ///////////////////////

  SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(listen_sock == INVALID_SOCKET){
    err_quit("socket()");
  }

  ///////////////////////     2. bind() 함수    ///////////////////////

  // 서버의 지역 IP주소와 포트 번호 결정
  SOCKADDR_IN serveraddr;
  ZeroMemory(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(9000);
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
  if(retval == SOCKET_ERROR){
    err_quit("bind()");
  }


  ///////////////////////     3. listen() 함수    ///////////////////////

  // listen_sock이라는 소켓과 결합된 포트 상태를 대기상태로 변경
  // SOMAXCONN 수만큼 소켓 생성 가능, SOMAXCONN = 0x7fffffff;
  retval = listen(listen_sock, SOMAXCONN);
  // SOCKET_ERROR = -1
  if(retval == SOCKET_ERROR){
    err_quit("listen()");
  }

  ///////////////////////     4. accept() 함수    //////////////////////////

  // 데이터 통신에 사용할 변수
  SOCKET client_sock;
  SOCKADDR_IN clientaddr;
  int addrlen;


  // 클라이언트와 데이터 통신
  while(1){
    addrlen = sizeof(clientaddr);
    // 접속한 클라이언트와 통신가능 하도록 새로운 client_sock이라는 소켓 생성
    client_sock = accept(listen_sock,(SOCKADDR*)&clientaddr, &addrlen);
    // INVALID_SOCKET = ~0
    if(client_sock == INVALID_SOCKET){
      err_display("accept()");
      continue;
    }
    // inet_ntoa(clientaddr.sin_addr) = 32비트 숫자로 IP주소를 입력받아
    //문자열 형태로 리턴
    // ntohs(clientaddr.sin_port) = 네트워크(빅 엔디안)에서 호스트
    //(리틀 엔디안)로 short형의 포트번호 리턴
    // 접속이 정확히 되었는지 확인 하기 위함(UI가 없으므로)
    printf("\n[TCP 서버] 클라이언트 접속 : IP주소=%s, 포트번호=%d\n"
      ,inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));



    while(1){
      ///////////////////////     5. recv() 함수    ///////////////////////

      // 데이터 받기
      // BUFSIZE는 define으로 512
      retval = recv(client_sock, buf, BUFSIZE, 0);
      if(retval == SOCKET_ERROR){
        err_display("recv()");
        break;
      }
      else if(retval == 0){
        break;
      }

      // 받은 데이터 출력
      // 버퍼의 마지막 번지에 NULL입력
      buf[retval] = '\0';

      // inet_ntoa(clientaddr.sin_addr) = 32비트 숫자로 IP주소를 입력받아
      //문자열 형태로 리턴
      // ntohs(clientaddr.sin_port) = 네트워크(빅 엔디안)에서 호스트
      //(리틀 엔디안)로 short형의 포트번호 리턴
      // 접속이 정확히 되었는지 확인 하기 위함(UI가 없으므로)
      printf("[TCP/%s:%d] %s\n",inet_ntoa(clientaddr.sin_addr)
        , ntohs(clientaddr.sin_port), buf);


      ///////////////////////     6. send() 함수  ///////////////////////

      // 데이터 보내기
      retval = send(client_sock, buf, retval, 0);
      if(retval == SOCKET_ERROR){
        err_display("send()");
        break;
      }
    }
    ///////////////////////     client_sock 소켓함수 소멸  ///////////////////////
    // 클라이언트와 통신하기 위해 생성했던 client_sock 소켓함수 소멸
    closesocket(client_sock);

    // inet_ntoa(clientaddr.sin_addr) = 32비트 숫자로 IP주소를 입력받아 문자열 형태로 리턴
    // ntohs(clientaddr.sin_port) = 네트워크(빅 엔디안)에서 호스트(리틀 엔디안)로 short형의
    // 포트번호 리턴
    // 접속이 정확히 되었는지 확인 하기 위함(UI가 없으므로)
    printf("[TCP 서버] 클라이언트 종료 : IP주소=%s, 포트번호=%d\n"
      ,inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
  }




  ///////////////////////     7. closesocket() 함수  ///////////////////////

  // 서버에서 accept이전에 만들었던 listen_sock 소켓함수 소멸
  closesocket(listen_sock);


  ///////////////////////     윈속 종료    ///////////////////////

  WSACleanup();

  return 0;
}
