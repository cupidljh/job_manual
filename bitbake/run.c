#include <stdio.h>
#include <stdlib.h>
#include "hdlc.h"


int main(int argc, char *argv[])
{
	hdlc_enc hdlc_enc[10];
	hdlc_dec hdlc_dec;

	if (argc == 1) {
		fputs("[error] no file \n", stderr);
		exit(1);
	}

	hdlc_init(hdlc_enc, &hdlc_dec); // hdlc encode, decode 구조체 초기화
	hdlc_encode(argv[1], hdlc_enc); // file data encode

	hdlc_decode(hdlc_enc, &hdlc_dec);//hdlc_decode(hdlc_enc, hdlc_dec);// file data decode

	return 0;
}
