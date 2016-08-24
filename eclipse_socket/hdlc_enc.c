/*
 * hdlc_enc.c
 *
 *  Created on: 2016. 8. 19.
 *      Author: Administrator
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "hdlc.h"

#define FLAG 0x7E

extern unsigned short fcstab[256];

extern void hdlc_enc_init(hdlc_enc *hdlc_encode);
extern void hdlc_dec_init(hdlc_dec *hdlc_decode);
extern void hdlc_encode(char *buf, unsigned int buflen,  hdlc_enc *hdlc_encode);
extern void hdlc_decode(hdlc_enc *hdlc_encode, hdlc_dec *hdlc_decode);// decode
extern unsigned short compute_fcs(unsigned char *data, int length);


void hdlc_enc_init(hdlc_enc *hdlc_encode)
{
	int i;

	hdlc_encode->start_flag = 0x00;
	hdlc_encode->finish_flag = 0x00;

	for (i = 0; i < sizeof(hdlc_encode->info); i++)
		hdlc_encode->info[i]='\0';

	hdlc_encode->fcs = 0x0000;

}

void hdlc_encode(char *buf, size_t buflen,  hdlc_enc *hdlc_encode)
{
    int i;

    for(i=0;i<(int)buflen;i++)
    {
       hdlc_encode->info[i] = buf[i];
    }

    hdlc_encode->start_flag = FLAG;

    printf("\nBefore Encoding Data : ");

    for (i = 0; i < (int)buflen; i++)
        printf("%02X ", hdlc_encode->info[i]);

    printf("\n\n");

    for (i = 0; i < (int)buflen; i++)
    {
        if ((hdlc_encode->info[i] == FLAG) |( hdlc_encode->info[i] == 0x7d)) {
            hdlc_encode->info[i]^=ESCAPE_BYTE;

            memmove(hdlc_encode->info + i + 1, hdlc_encode->info + i, (int)buflen - i + 1);
            hdlc_encode->info[i]=0x7d;
            (int)buflen++;
        }
    }

    hdlc_encode->fcs = compute_fcs(hdlc_encode->info, (int)buflen);
    hdlc_encode->crcH = hdlc_encode->fcs >> 8;
    hdlc_encode->crcL = hdlc_encode->fcs;
    hdlc_encode->size = (int)buflen;
    hdlc_encode->finish_flag = FLAG;

    printf("After Encoding Data : %02X ", hdlc_encode->start_flag);

    for (i = 0; i < (int)buflen; i++)
    {
        printf("%02X ", hdlc_encode->info[i]);
    }

    printf("%02X %02X %02X\n\n",hdlc_encode->crcH,hdlc_encode->crcL, hdlc_encode->finish_flag);
}

