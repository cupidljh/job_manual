/*
 * hdlc.h
 *
 *  Created on: 2016. 8. 19.
 *      Author: Administrator
 */

#ifndef HDLC_H_
#define HDLC_H_

#define ESCAPE_BYTE 20

typedef struct _Hdlc_encode {

	unsigned int start_flag;
	unsigned char info[256];
	unsigned short fcs;
	unsigned int finish_flag;
	int size;
	unsigned char crcH, crcL;

}hdlc_enc;

typedef struct _Hdlc_decode {

	unsigned int start_flag;
	unsigned char info[256];
	unsigned short fcs;
	unsigned int finish_flag;
	int size;
	unsigned char crcH, crcL;

}hdlc_dec;


#endif /* HDLC_H_ */
