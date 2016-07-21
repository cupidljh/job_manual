
// CRC16 방식을 이용한 CRC 체크 프로그램...

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned short tbl_crcr[1024];

typedef unsigned char UCHAR;

void Util_Compress(UCHAR *bySource, UCHAR *byDestination, int byCount)
{
	
	UCHAR d, f;
	int i, j;

	j = 0;
	for (i = 0; i<(int)byCount; i = i + 2)
	{
		
		d = bySource[i];

		if (d < 0x3a)           /* Numeric */
		{
			f = d & 0x0F;
			d = f << 4;
			byDestination[j] = d;
		}
		else if ((d >= 0x3A) && (d <= 0x3F))              /* 특수 문자 */
		{
			f = d & 0x0F;
			d = f << 4;
			f = d;
			byDestination[j] = f;
		}
		else                         /* 알파벳 */
		{
			f = d & 0x0F;
			d = f << 4;
			f = d + 0x90;
			byDestination[j] = f;
		}

		d = bySource[i + 1];
		if (d < 0x3a)            /* Numeric */
		{
			f = d & 0x0f;
			byDestination[j] = f + byDestination[j];
		}
		else if ((d >= 0x3A) && (d <= 0x3F))              /* 특수 문자 */
		{
			f = d & 0x0f;
			d = f;
			byDestination[j] = d + byDestination[j];
		}
		else                         /* 알파벳 */
		{
			f = d & 0x0f;
			d = f + 0x09;
			byDestination[j] = d + byDestination[j];
		}
		j++;
	}
	
}

unsigned short crcrevhware(unsigned short data, unsigned short genpoly, unsigned short accum)
{
	static int i;

	for (i = 8; i > 0; i--)
	{
		if ((accum ^ data) & 0x0001)
		{
			accum >>= 1;
			accum = accum ^ genpoly;
		}
		else
		{
			accum >>= 1;
		}

		data >>= 1;
	}

	return accum;
}

void InitCrcTable(void)
{
	int i, j;

	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 8; j++)
		{
			tbl_crcr[i * 8 + j] = crcrevhware(8 * i + j, 0x8408, 0x0000);
		}
	}
}

unsigned short crcrevtable(unsigned short data, unsigned short accum, unsigned short *crcrevtab)
{
	static int comb_val;

	comb_val = accum ^ data;
	accum = (accum >> 8) ^ crcrevtab[comb_val & 0x00ff];
	return accum;
}

unsigned short crc(UCHAR *buf, int nCnt, UCHAR *RetcrcL, UCHAR *RetcrcH)
{
	int i;
	unsigned short c;
	unsigned short accum;
	unsigned char crcL, crcH;


	if (buf == NULL)
		//		return R_CRC_NULL_BYTE;
		return NULL;

	accum = 0xffff;
	for (i = 0; i < nCnt; i++)
	{
		c = buf[i];
		accum = crcrevtable(c, accum, tbl_crcr);
	}

	crcL = accum & 0x00ff;
	crcH = (accum & 0xff00) >> 8;


	if (crcL < 0x80) crcL ^= 0xff;
	if (crcH < 0x80) crcH ^= 0xff;

	*RetcrcL = crcL;
	*RetcrcH = crcH;

	return  accum;
}

void print_crc(char *filename)
{
	int i, len;
	UCHAR crcL, crcH;

	printf("Crc_Test\n");

	UCHAR buf[1024];
	UCHAR dbuf[1024];
	size_t buflen;
	FILE *in;


	memset(buf, 0x00, sizeof(buf));
	memset(dbuf, 0x00, sizeof(dbuf));
	memset(tbl_crcr, 0x00, sizeof(tbl_crcr));



	if ((in = fopen(filename, "rb")) == NULL) {
		fputs("file error\n", stderr);
		exit(1);
	}
	else {
		
	}

	while ((buflen = fread(buf, sizeof(char), sizeof(buf), in)) != NULL)
	{
		
		len = (int)buflen;

		
		Util_Compress(buf, dbuf, len);

		for (i = 0; i< len / 2; i++)
		{
			printf("%x ", dbuf[i]);
		}
		printf("\n");

		printf("\nStart CRC Check\n");

		InitCrcTable();
		crc(dbuf, len / 2, &crcL, &crcH);

		printf("CRC : %X %X\n", crcL, crcH);


		printf("\n");
	}


	fclose(in);

}

void main(int argc, char *argv[])
{
	if (argc == 1) {
		fputs("error\n", stderr);
		exit(1);
	}

	print_crc(argv[1]);

	return 0;
	



}

