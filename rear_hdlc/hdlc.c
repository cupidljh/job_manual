#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "crc_table.h"
#include "hdlc.h"



void hdlc_init(hdlc_enc *hdlc_encode, hdlc_dec *hdlc_decode) //hdlc 구조체를 초기화 해준다.
{
	//hdlc_encode init

	hdlc_encode->start_flag = 0x7e;
	hdlc_encode->finish_flag = 0x7e;

	for (int i = 0; i < sizeof(hdlc_encode->info); i++)
		hdlc_encode->info[i] = 0;

	hdlc_encode->fcs = 0x0000;

	//hdlc_decode init

	for (int i = 0; i < strlen(hdlc_decode->info); i++)
		hdlc_decode->info[i] = 0;

	hdlc_decode->fcs = 0x0000;
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

	buflen = fread(hdlc_encode->info, sizeof(char), sizeof(buf), in);

	for (i = 0; i < (int)buflen; i++)
		printf("%c", hdlc_encode->info[i]);

	printf("\n");

	for (i = 0; i < (int)buflen; i++)
	{
		if (hdlc_encode->info[i] == 0x7e | hdlc_encode->info[i] == 0x7d) { // data 중간 7E가 있으면 안됨으로 처리
			hdlc_encode->info[i] ^= ESCAPE_BYTE; // 7E와 0x20을 XOR해줌 그 후 XOR 값 앞에 7D 넣어주면 끝

			memmove(hdlc_encode->info + i + 1, hdlc_encode->info + i, (int)buflen - i + 1); //배열 중간에 7d 삽입 할 공간 만듦
			hdlc_encode->info[i] = 0x7d; // 7D 삽입
			(int)buflen++;
		}
	}

	printf("input data = ");
	for (i = 0; i< (int)buflen; i++)
		printf(" %02X", hdlc_encode->info[i]); //검사한 data 출력

	printf("\n");

	hdlc_encode->fcs = compute_fcs(hdlc_encode->info, (int)buflen); // fcs 계산
	printf("fcs = %02X\n", hdlc_encode->fcs);

	hdlc_encode->size = (int)buflen;

}


void hdlc_decode(hdlc_enc *hdlc_encode, hdlc_dec *hdlc_decode)
{
	int i;

	hdlc_decode->size = hdlc_encode->size; // input data size를 가져온다

	for (i = 0; i < hdlc_decode->size; i++)  // encode에 data를 decode 에 저장
		hdlc_decode->info[i] = hdlc_encode->info[i];

	for (i = 0; i < hdlc_decode->size; i++)
	{
		if (hdlc_decode->info[i] == 0x7D) // decode data 에 7D가 있다면 그 뒤에 인자값을 0x20 과 XOR 해줘서 원래 값으로 되돌린 후
		{                                // 7D를 없애고 배열을 한 칸씩 땡긴다
			hdlc_decode->info[i + 1] ^= ESCAPE_BYTE;
			memmove(hdlc_decode->info + i, hdlc_decode->info + i + 1, hdlc_decode->size - i);
		}
	}

	hdlc_decode->fcs = compute_fcs(hdlc_decode->info, hdlc_decode->size); // fcs 계산

	if (hdlc_decode->fcs != hdlc_encode->fcs)
		printf("transmit error\n");
	else
		printf("transmit success\n");
}
