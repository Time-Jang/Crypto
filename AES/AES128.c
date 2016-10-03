/*  ============================================================================================================ *

                                    주 의 사 항


    1. 주석으로 표현된 구현 블록 0에서 12번까지 구현하여 128비트 AES 암호화 알고리즘을 구현해야 함
    2. AES128(...) 함수의 호출과 리턴이 여러번 반복되더라도 메모리 누수가 생기지 않게 함
    3. AddRoundKey 함수를 구현할 때에도 파라미터 rKey는 사전에 선언된 지역 배열을 가리키도록 해야 함
       (정확한 구현을 위해서는 포인터 개념의 이해가 필요함)
    4. 배열의 인덱스 계산시 아래에 정의된 KEY_SIZE, ROUNDKEY_SIZE, BLOCK_SIZE를 이용해야 함
       (상수 그대로 사용하면 안됨. 예로, 4, 16는 안되고 KEY_SIZE/4, BLOCK_SIZE로 사용해야 함)

 *  ============================================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include "AES128.h"

#define KEY_SIZE 16
#define ROUNDKEY_SIZE 176
#define BLOCK_SIZE 16


/*********************************************** { 구현 0 시작 } ********************************************/
BYTE S_BOX[BLOCK_SIZE*BLOCK_SIZE] =
{
	0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
	0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
	0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
	0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
	0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
	0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
	0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
	0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
	0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
	0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
	0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
	0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
	0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
	0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
	0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
	0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

BYTE RS_BOX[BLOCK_SIZE*BLOCK_SIZE] =
{
	0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
	0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
	0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
	0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
	0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
	0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
	0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
	0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
	0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
	0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
	0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
	0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
	0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
	0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
	0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

BYTE MCOL[BLOCK_SIZE] =
{
	2, 3, 1, 1,
	1, 2, 3, 1,
	1, 1, 2, 3,
	3, 1, 1, 2
};

BYTE RMCOL[BLOCK_SIZE] =
{
	14, 11, 13, 9,
	9, 14, 11, 13,
	13, 9, 14, 11,
	11, 13, 9, 14
};

BYTE RCON[] = 
{
	0x01, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00,
	0x36, 0x00, 0x00, 0x00
};

/*********************************************** { 구현 0 종료 } ********************************************/


/*  <키스케줄링 함수>
 *   
 *  key         키스케줄링을 수행할 16바이트 키
 *  roundKey    키스케줄링의 결과인 176바이트 라운드키가 담길 공간 , expandkey의 결과
 */
void expandKey(BYTE *key, BYTE *roundKey){   //                         검산

    /*********************************************** { 구현 1 시작 } ********************************************/
	int i,j,k;
	BYTE temp;
	BYTE g[KEY_SIZE/4]; // 해당 라운드에서 사용되는 g
	BYTE tempkey[BLOCK_SIZE]; //해당 라운드에서 사용되는 key
	for(k=0;k<BLOCK_SIZE;k++) //첫 roundkey에 key 삽입
		roundkey[k]=key[k];
	for(i=0,i<(ROUNDKEY_SIZE/BLOCK_SIZE)-1,i++) //나머지 roundkey
	{
		for(k=0;k<BLOCK_SIZE;k++) 
			tempkey[k]=roundkey[(BLOCK_SIZE)*i+k];
		// G 구하기 for문 하나로 줄여봄
		for(j=0;j<KEY_SIZE/4;j++) ///Left Rotation Word ,g = [w3[1],w3[2],w3[3],w3[0]]
		{
			g[((KEY_SIZE/4)-1)-j] = tempkey[j*(KEY_SIZE/4)+((KEYSIZE/4)-1)];
			g[j]=S_BOX[g[j]];
			g[j]^=RCON[i];
		}
		//G 완성
		for(j=0;<j<(KEY_SIZE/4);j++) //첫번째 word의 g연산 
			tempkey[j]^=g[j];
		for(j=(KEY_SIZE/4);j<BLOCK_SIZE;j++)
			tempkey[j]^=tempkey[j-(KEY_SIZE/4)];
		for(j=0;j<BLOCK_SIZE;j++)
			roundkey[BLOCK_SIZE*(i+1)+j]=tempkey[j];
	}



    /*********************************************** { 구현 1 종료 } ********************************************/

}


/*  <SubBytes 함수>
 *   
 *  block   SubBytes 수행할 16바이트 블록. 수행 결과는 해당 배열에 바로 반영
 *  mode    SubBytes 수행 모드
 */
 BYTE* subBytes(BYTE *block, int mode){
	 int i;
    /* 필요하다 생각하면 추가 선언 */

    switch(mode){
		
        case ENC:
            /*********************************************** { 구현 2 시작 } ********************************************/
			for(i=0;i<BLOCK_SIZE;i++)
				block[i] =  S_BOX[block[i]];

            /*********************************************** { 구현 2 종료 } ********************************************/            
            break;

        case DEC:
            /*********************************************** { 구현 3 시작 } ********************************************/
			for(i=0;i<BLOCK_SIZE;i++)
				block[i] = RS_BOX[block[i]];


            /*********************************************** { 구현 3 종료 } ********************************************/
            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }
    
    return block;
}


/*  <ShiftRows 함수>
 *   
 *  block   ShiftRows 수행할 16바이트 블록. 수행 결과는 해당 배열에 바로 반영
 *  mode    ShiftRows 수행 모드
 */
BYTE* shiftRows(BYTE *block, int mode){ 
	int i;
	BYTE temp;
    /* 필요하다 생각하면 추가 선언 */   

    switch(mode){

        case ENC:
            /*********************************************** { 구현 4 시작 } ********************************************/
			temp = block[KEY_SIZE/4];       // row 2
			for(i=1;i<(KEY_SIZE/4);i++)
				block[3+i]=block[4+i];
			block[3+i] = temp;
			
			for(i=0;i<KEY_SIZE/8;i++) // row 3
			{
				temp = block[8+i];
				block[8+i] = block[10+i];
				block[10+i] = temp;
			}
			
			temp = block[11+(KEY_SIZE/4)];          // row4
			for(i=(KEY_SIZE/4);i>1;i--) 
				block[11+i]=block[10+i];
			block[11+i] = temp;



            /*********************************************** { 구현 4 종료 } ********************************************/            
            break;

        case DEC:
            /*********************************************** { 구현 5 시작 } ********************************************/
			temp = block[3+KEY_SIZE/4];          // row2
			for(i=(KEY_SIZE/4);i>1;i--) 
				block[3+i]=block[2+i];
			block[3+i] = temp;
			
			for(i=0;i<KEY_SIZE/8;i++) // row 3
			{
				temp = block[8+i];
				block[8+i] = block[10+i];
				block[10+i] = temp;
			}
			
			temp = block[12];       // row 4
			for(i=1;i<(KEY_SIZE/4);i++)
				block[11+i]=block[12+i];
			block[11+i] = temp;




            /*********************************************** { 구현 5 종료 } ********************************************/
            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }
    
    return block;
}


/*  <MixColumns 함수>
 *   
 *  block   MixColumns을 수행할 16바이트 블록. 수행 결과는 해당 배열에 바로 반영
 *  mode    MixColumns의 수행 모드
 */
BYTE* mixColumns(BYTE *block, int mode){    

    /* 필요하다 생각하면 추가 선언 */   

    switch(mode){

        case ENC:
            /*********************************************** { 구현 6 시작 } ********************************************/
			f



            /*********************************************** { 구현 6 종료 } ********************************************/            
            break;

        case DEC:
            /*********************************************** { 구현 7 시작 } ********************************************/
			f



            /*********************************************** { 구현 7 종료 } ********************************************/
            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }
    
    return block;
}


/*  <AddRoundKey 함수>
 *   
 *  block   AddRoundKey를 수행할 16바이트 블록. 수행 결과는 해당 배열에 반영
 *  rKey    AddRoundKey를 수행할 16바이트 라운드키
 */
BYTE* addRoundKey(BYTE *block, BYTE *rKey){

    /*********************************************** { 구현 8 시작 } ********************************************/
	int i;
	
	for(i=0;i<BLOCK_SIZE;i++)
		block[i]^= rKey[i];



    /*********************************************** { 구현 8 종료 } ********************************************/

    return block;
}


/*  <128비트 AES 암호화 함수>
 *   
 *  plain   바이트 배열로 구성된 평문 (16바이트 고정)
 *  key     128비트 암호키 (16바이트)
 *
 *  @ret    암호화된 암호문
 */
BYTE* encrypt(BYTE *plain, BYTE *key){
    BYTE roundKey[ROUNDKEY_SIZE];

    /*********************************************** { 구현 9 시작 } ********************************************/




    /*********************************************** { 구현 9 종료 } ********************************************/
}


/*  <128비트 AES 복호화 함수>
 *   
 *  cipher  바이트 배열로 구성된 평문 (16바이트 고정)
 *  key     128비트 암호키 (16바이트)
 *
 *  @ret    복호화된 평문
 */
BYTE* decrypt(BYTE *cipher, BYTE *key){
    BYTE roundKey[ROUNDKEY_SIZE];
    
    /*********************************************** { 구현 10 시작 } ********************************************/




    /*********************************************** { 구현 10 종료 } ********************************************/
}


/*  <128비트 AES 암복호화 함수>
 *  
 *  mode가 ENC일 경우 평문을 암호화하고, DEC일 경우 암호문을 복호화하는 함수
 *
 *  [ENC 모드]
 *  plain   평문 바이트 배열
 *  cipher  결과(암호문)이 담길 바이트 배열. 호출하는 사용자가 사전에 메모리를 할당하여 파라미터로 넘김
 *  key     128비트 암호키 (16바이트)
 *
 *  [DEC 모드]
 *  plain   결과(평문)가 담길 바이트 배열. 호출하는 사용자가 사전에 메모리를 할당하여 파라미터로 넘김
 *  cipher  암호문 바이트 배열
 *  key     128비트 암호키 (16바이트)
 */
void AES128(BYTE *plain, BYTE *cipher, BYTE *key, int mode){
    BYTE *tmp;  

    if(mode == ENC){
        tmp = encrypt(plain, key);

        /*********************************************** { 구현 11 시작 } ********************************************/



        /*********************************************** { 구현 11 종료 } ********************************************/ 

    }else if(mode == DEC){
        tmp = decrypt(cipher, key);

        /*********************************************** { 구현 12 시작 } ********************************************/



        /*********************************************** { 구현 12 종료 } ********************************************/ 

    }else{
        fprintf(stderr, "Invalid mode!\n");
        exit(1);
    }
}
