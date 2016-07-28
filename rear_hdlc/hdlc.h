#ifndef _HDLC_H
#define _HDLC_H

#define ESCAPE_BYTE 0x20   // 7E값이 나왔을 때 연산해줄 HEX값


typedef struct _Hdlc_encode {  // 구조체
	unsigned int start_flag;
	unsigned char info[256];
	unsigned short fcs;
	unsigned int finish_flag;
	int size;
}hdlc_enc;



typedef struct _Hdlc_decode {
	unsigned int start_flag;
	unsigned char info[256];
	unsigned short fcs;
	unsigned int finish_flag;
	int size;
}hdlc_dec;

void hdlc_init(hdlc_enc *hdlc_encode, hdlc_dec *hdlc_decode); //hdlc 구조체를 초기화 해준다.
void hdlc_encode(char *filename, hdlc_enc *hdlc_encode); // 파일에서 16byte씩 데이터를 가져온다.
void hdlc_decode(hdlc_enc *hdlc_encode, hdlc_dec *hdlc_decode);// decode
unsigned short compute_fcs(unsigned char *data, int length); //  CRC값 계산

#endif