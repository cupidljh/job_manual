#define DEBUG

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "crc_table.h"
#include "hdlc.h"
#include "debug.h"


#define FLAG 0x7E

void hdlc_init(hdlc_enc *hdlc_encode, hdlc_dec *hdlc_decode) //hdlc 구조체를 초기화 해준다.
{
	//hdlc_encode init
	int len = 10;
	int i;
	for (i = 0; i < len; i++)
	{
		hdlc_encode->start_flag = 0x00;
		hdlc_encode->finish_flag = 0x00;

		for (i = 0; i < sizeof(hdlc_encode->info); i++)
			hdlc_encode->info[i] = 0;

		hdlc_encode->fcs = 0x0000;
	}
	//hdlc_decode init

	hdlc_decode->start_flag = 0x00;

	for (i = 0; i < strlen(hdlc_decode->info); i++)
		hdlc_decode->info[i] = 0;

	hdlc_decode->fcs = 0x0000;

	hdlc_decode->finish_flag = 0x00;
}

u16 compute_fcs(unsigned char *data, int length)
{
	u16 fcs;

	fcs = 0xffff;
	for (int i = 0; i< length; i++)
	{
		fcs = (fcs >> 8) ^ fcstab[(fcs ^ ((u16)*data)) & 0xff];
		data++;
	}
	return (fcs);
}

void hdlc_encode(char *filename, hdlc_enc *hdlc_encode) // 파일에서 16byte씩 데이터를 가져온다.
{
	int i;
	size_t buflen;
	char buf[16];
	FILE *in;

	if ((in = fopen(filename, "rb")) == NULL) {
		fputs("file error\n", stderr);
		exit(1);
	}

	while ((buflen = fread(hdlc_encode->info, sizeof(char), sizeof(buf), in)) != NULL)//파일이 끝날 때 까지 읽기
	{
		hdlc_encode->start_flag = FLAG;

		printf("\nBefore Encoding Data : ");

		for (i = 0; i < (int)buflen; i++)
			printf("%02X ", hdlc_encode->info[i]);

		printf("\n\n");
		

		for (i = 0; i < (int)buflen; i++)
		{
			if (hdlc_encode->info[i] == FLAG | hdlc_encode->info[i] == 0x7d) { // data 중간 7E가 있으면 안됨으로 처리
				hdlc_encode->info[i] ^= ESCAPE_BYTE; // 7E와 0x20을 XOR해줌 그 후 XOR 값 앞에 7D 넣어주면 끝

				memmove(hdlc_encode->info + i + 1, hdlc_encode->info + i, (int)buflen - i + 1); //배열 중간에 7d 삽입 할 공간 만듦
				hdlc_encode->info[i] = 0x7d; // 7D 삽입
				(int)buflen++;
			}
		}

		hdlc_encode->fcs = compute_fcs(hdlc_encode->info, (int)buflen); // fcs 계산

		hdlc_encode->size = (int)buflen;

		hdlc_encode->finish_flag = FLAG;

		printf("After Encoding Data : %02X ", hdlc_encode->start_flag);   //Encoding 한 HDLC 출력하기.
		for (i = 0; i < (int)buflen; i++)
			printf("%02X ", hdlc_encode->info[i]);
		printf("%02X %02X\n\n",hdlc_encode->fcs, hdlc_encode->finish_flag);

		hdlc_encode++; // 구조체 배열 증가
	}
}


void hdlc_decode(hdlc_enc *hdlc_encode, hdlc_dec *hdlc_decode)
{
	int len = 10;
	while (len>0)
	{
		if (hdlc_encode->start_flag == FLAG) { // 시작 플래그에 0x7E가 들어갔을 때만 decoding한다.
			int i;

			hdlc_decode->size = hdlc_encode->size; // input data size를 가져온다

			hdlc_decode->start_flag = hdlc_encode->start_flag;
			hdlc_decode->finish_flag = hdlc_encode->finish_flag;

			for (i = 0; i < hdlc_decode->size; i++)  // encode에 data를 decode 에 저장
				hdlc_decode->info[i] = hdlc_encode->info[i];

			hdlc_decode->fcs = compute_fcs(hdlc_decode->info, hdlc_decode->size); // fcs 계산

			for (i = 0; i < hdlc_decode->size; i++)
			{
				if (hdlc_decode->info[i] == 0x7D) // decode data 에 7D가 있다면 그 뒤에 인자값을 0x20 과 XOR 해줘서 원래 값으로 되돌린 후
				{                                // 7D를 없애고 배열을 한 칸씩 땡긴다
					hdlc_decode->info[i + 1] ^= ESCAPE_BYTE;
					memmove(hdlc_decode->info + i, hdlc_decode->info + i + 1, hdlc_decode->size - i);
					hdlc_decode->size--;
				}
			}

			printf("After Decoding Data = ");

			for (i = 0; i < hdlc_decode->size; i++)
				printf("%02X ", hdlc_decode->info[i]);

			printf("\n\n");

			if (hdlc_decode->fcs != hdlc_encode->fcs)
				printf("receive error\n\n");
			else
				printf("receive success\n\n");

			
		}
		hdlc_encode++;
		len--;
	}
}
