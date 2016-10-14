
#include <stdio.h>
#include <stdlib.h>
#include "/Users/jang-won-u/Desktop/cryptology/AES/test_XTS/XTS_AES.h"
#include "/Users/jang-won-u/Desktop/cryptology/AES/test_XTS/AES128.h"
#include "/Users/jang-won-u/Desktop/cryptology/AES/test_XTS/AES128.c"
#define BLOCK_SIZE 16

void encrypt_XTS(BYTE *key1, BYTE *key2, int N,const BYTE *plain, BYTE *cipher)
{
	int i,j,k;
	int S = 0;
	BYTE T[BLOCK_SIZE];
	BYTE x[BLOCK_SIZE];
	int Cin,Count;
	BYTE T_C[BLOCK_SIZE];
	BYTE x_C[BLOCK_SIZE];
	for( j = 0; j < BLOCK_SIZE; j++)
	{
		T[j] = S & 0XFF;
		S = S >> 8;
	}
	AES128(T,T_C,key2,ENC);
	for(i = 0; i < N; i += BLOCK_SIZE)
	{
		for(j = 0; j < BLOCK_SIZE; j++)
			x[j] = plain[i+j] ^ T_C[j];
		AES128(x,x_C,key1,ENC);
		for(j = 0; j < BLOCK_SIZE; j++)
			cipher[i + j] = x_C[j] ^ T_C[j];
		Cin = 0;
		for(j = 0; j < BLOCK_SIZE; j++)
		{
			Count = (T_C[j] >> 7) & 1;
			T_C[j] = ((T_C[j] << 1) + Cin) & 0xFF;
			Cin = Count;
		}
		if(Count)
			T_C[0] = T_C[0] ^ 0x87;
		printf("\n ENC \n");
	}	
	if(i < N)
	{
		for(j = 0; i+j < N; j++)
		{
			x[j] = plain[i + j] ^ T[j];
			cipher[i + j] = cipher[i + j - BLOCK_SIZE];
		}
		for(j = 0; j <BLOCK_SIZE; j++)
			x[j] = cipher[i + j - BLOCK_SIZE] ^ T[j];
		AES128(x,x_C,key1,ENC);
		for(j = 0; j < BLOCK_SIZE; j++)
			cipher[i + j - BLOCK_SIZE] = x_C[j] ^ T[j];
	}	
	printf("\n ENC \n");	
}
void decrypt_XTS(BYTE *key1, BYTE *key2, int N, BYTE *plain,const BYTE *cipher)
{
	int i,j,k;
	int S = 0;
	BYTE T[BLOCK_SIZE];
	BYTE x[BLOCK_SIZE];
	int Cin,Count;
	BYTE T_C[BLOCK_SIZE]; //pp
	BYTE x_C[BLOCK_SIZE]; //CC
	for( j = 0; j < BLOCK_SIZE; j++)
	{
		T[j] = S & 0XFF;
		S = S >> 8;
	}
	AES128(T,T_C,key2,ENC);  //T
	for(i = 0; i < N; i += BLOCK_SIZE)
	{
		for(j = 0; j < BLOCK_SIZE; j++)
			x[j] = cipher[i+j] ^ T_C[j]; //CC
		AES128(x_C,x,key1,DEC);  //PP
		for(j = 0; j < BLOCK_SIZE; j++)
			plain[i + j] = x_C[j] ^ T_C[j]; //plain
		Cin = 0;
		for(j = 0; j < BLOCK_SIZE; j++)
		{
			Count = (T_C[j] >> 7) & 1;
			T_C[j] = ((T_C[j] << 1) + Cin) & 0xFF;
			Cin = Count;
		}
		if(Count)
			T_C[0] = T_C[0] ^ 0x87;
		printf("\n DEC \n");
	}	
	if(i < N)
	{
		for(j = 0; i+j < N; j++)
		{
			x[j] = cipher[i + j] ^ T[j];
			plain[i + j] = plain[i + j - BLOCK_SIZE];
		}
		for(j = 0; j <BLOCK_SIZE; j++)
			x[j] = plain[i + j - BLOCK_SIZE] ^ T[j];
		AES128(x_C,x,key1,DEC);
		for(j = 0; j < BLOCK_SIZE; j++)
			plain[i + j - BLOCK_SIZE] = x_C[j] ^ T[j];
	}	
	printf("\n DEC\n");	
}
int main()
{
	BYTE key[] =
	{
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00
	};
	BYTE plain[] =
	{
		0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00
	};
	BYTE *cipher = (BYTE *)malloc(sizeof(BYTE) * 31);
	BYTE *plain2 = (BYTE *)malloc(sizeof(BYTE) * 31);
	int i;
	printf("input\n");
	for(i = 0; i < 31; i++)
		printf("%x",plain[i]);
	encrypt_XTS(key, key+16, 31, plain, cipher);
	for(i = 0; i < 31; i++)
		printf("%x",cipher[i]);
	printf("\n"); 
	decrypt_XTS(key, key+16, 31, plain2, cipher);
	for(i = 0; i < 31; i++)
		printf("%x",plain2[i]);
	printf("\n");
	return 0;
}



/*
Vector 1
Key1 00000000000000000000000000000000
Key2 00000000000000000000000000000000
Data Unit Sequence number 0
PTX 0000000000000000000000000000000000000000000000000000000000000000 
TWK 66e94bd4ef8a2c3b884cfa59ca342b2eccd297a8df1559761099f4b39469565c 
CTX 917cf69ebd68b2ec9b9fe9a3eadda692cd43d2f59598ed858c02c2652fbf922e
Vector 2
Key1 11111111111111111111111111111111
Key2 22222222222222222222222222222222
Data Unit Sequence number 3333333333
PTX 4444444444444444444444444444444444444444444444444444444444444444 
TWK 3f803bcd0d7fd2b37558419f59d5cda6f900779a1bfea467ebb0823eb3aa9b4d 
CTX c454185e6a16936e39334038acef838bfb186fff7480adc4289382ecd6d394f0
Vector 3
Key1 fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0
Key2 22222222222222222222222222222222
Data Unit Sequence number 3333333333
PTX 4444444444444444444444444444444444444444444444444444444444444444 
TWK 3f803bcd0d7fd2b37558419f59d5cda6f900779a1bfea467ebb0823eb3aa9b4d 
CTX af85336b597afc1a900b2eb21ec949d292df4c047e0b21532186a5971a227a89
*/