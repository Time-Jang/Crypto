/*  ============================================================================================================ *

                                    주 의 사 항


    1. 주석으로 표현된 구현 블록 13에서 14번까지 구현하여 128비트 AES 암호화 알고리즘을 구현해야 함
    2. AES.c를 먼저 구현한 다음, 해당 파일을 구현함
    3. 사전에 주어진 메뉴얼 속 수도코드를 참고하여 구현함
    4. 구현은 다양한 방식으로 이뤄질 수 있음
    5. AES.h에 정의된 AES128(...) 함수만을 이용해서 구현해야 함
    6. XTS_AES128(...) 함수의 호출과 리턴이 여러번 반복되더라도 메모리 누수가 생기지 않게 함

 *  ============================================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include "XTS_AES.h"
#include "AES128.h"


/*********************************************** { 구현 13 시작 } ********************************************/
typedef unsigned int WORD;
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
}
/*********************************************** { 구현 13 종료 } ********************************************/


/*  <128비트 XTS_AES 암복호화 함수>
 *  
 *  mode가 ENC일 경우 평문을 암호화하고, DEC일 경우 암호문을 복호화하는 함수
 *
 *  [ENC 모드]
 *  plain   평문 바이트 배열
 *  cipher  결과(암호문)이 담길 바이트 배열. 호출하는 사용자가 사전에 메모리를 할당하여 파라미터로 넘김
 *  size    평문 크기 (바이트 단위)
 *  key     256비트 암호키 (32바이트). 상위 16바이트는 key1, 하위 16바이트는 key2
 *
 *  [DEC 모드]
 *  plain   결과(평문)가 담길 바이트 배열. 호출하는 사용자가 사전에 메모리를 할당하여 파라미터로 넘김
 *  cipher  암호문 바이트 배열
 *  size    암호문 크기 (바이트 단위)
 *  key     256비트 암호키 (32바이트). 상위 16바이트는 key1, 하위 16바이트는 key2
 */
void XTS_AES128(BYTE *plain, BYTE *cipher, unsigned int size, BYTE* key, int mode){

	/*********************************************** { 구현 14 시작 } ********************************************/
	if(mode == ENC)
	{
		encrypt_XTS(key, key+16, size, plain, cipher);
	}
	else if(mode == DEC)
	{
		decrypt_XTS(key, key+16, size, plain, cipher);
	}
	else
	{
		fprintf(stderr, "Invalid mode!\n");
		exit(1);
	}
	/*********************************************** { 구현 14 종료 } ********************************************/
}
