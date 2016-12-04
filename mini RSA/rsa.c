/**
	@file		rsa.c
	@author		장원우 / 2014038286
	@date		20161120
	@brief		mini RSA implementation code
	@details	세부 설명
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rsa.h"

uint p, q, e, d, n;

uint root(uint m)
{
	uint result;
	for(result = 0; m != 1; result++)
		m = m >> 1;
	return result;
}

/**
	@brief		모듈러 덧셈 연산을 하는 함수.
	@param		uint a		: 피연산자1.
	@param		uint b		: 피연산자2.
	@param		byte op		: +, - 연산자.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 덧셈에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/
uint ModAdd(uint a, uint b, byte op, uint mod) {
	uint result,i,quotient;
	if(op == '-')
	{
		b = b * -1;
		while(b <= 0)
			b = b + mod;
	}
	result = a + b;
	quotient = result >> root(mod);
	for(i = 0; i < quotient; i++)
		result = result - mod;
	return result;
}

/**
	@brief		모듈러 곱셈 연산을 하는 함수.
	@param		uint x		: 피연산자1.
	@param		uint y		: 피연산자2.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 곱셈에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/

uint two_exp(uint a)
{
	if(a == 0)
		return 1;
	return 2 * two_exp(a-1);
}

uint ModMul(uint x, uint y, uint mod){
	uint result,result0,temp,k,j;
	temp = y;
	result = 0;
	for(k = 0; temp != 0 ;temp = temp >> 1, k++)
	{
		if((temp & 1) == 0)
		{
			continue;
		}
		else
		{
			if(k == 0)
			{
				result = x;
				continue;
			}
			printf("k : %d\n",k);
			printf("two_exp(k-1) : %d\n",two_exp(k-1));
			for(j = 0; j < two_exp(k-1); j++)
			{
				if((x+x) <= x)
					result0 = x - (mod - x);
				else if((x+x) <= mod)
					result0 = x + x;
				else
					result0 = x + x - mod;
				if(result < result0 && (result + result0) < result0)
				{
					result = result0 - (mod - result);
//					printf("if\n");
				}
				else if(result > result0 && (result + result0) < result)
				{
					result = result - (mod - result0);
//					printf("else if 1\n");
				}
				else if((result + result0) <= mod)
				{
					result = result0 + result;
//					printf("else if 2\n");
				}
				else
				{
					result = result + result0 - mod;
//					printf("else\n");
				}
//				printf("result : %u\tresult0 : %u\n",result,result0);
			}
		}
	}
	
	return result;
}

/**
	@brief		모듈러 거듭제곱 연산을 하는 함수.
	@param		uint base	: 피연산자1.
	@param		uint exp	: 피연산자2.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 연산에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
				'square and multiply' 알고리즘을 사용하여 작성한다.
*/

uint square_and_multiply(uint base, uint exp)
{
	uint result;
	if(exp == 0)
		return 1;
	result = 1;
	while(exp > 1)
	{
		if((exp & 1) == 0)
		{
			base = base * base;
			exp = exp >> 1;
		}
		else
		{
			result = base * result;
			base = base * base;
			exp = (exp - 1) >> 1;
		}
	}
	return result * base;
}
uint square_and_multiply2(uint base, uint exp,uint mod)
{
	uint result;
	if(exp == 0)
		return 1;
	result = 1;
	while(exp > 1)
	{
		if((exp & 1) == 0)
		{
//			printf("base = ModMul(%u,%u,",base,base);
			base = ModMul(base, base,mod);
//			printf("%u) = %u\n",mod,base);
			exp = exp >> 1;
		}
		else
		{
			result = ModMul(base, result,mod);
			base = ModMul(base, base, mod);
			exp = (exp - 1) >> 1;
		}
//		printf("result : %u\tbase : %u\n",result,base);
	}
	return ModMul(result, base,mod);
}

uint ModPow(uint base, uint exp, uint mod) {
//	printf("ModPow : ((%d)*(%d))mod%d",base,exp,mod);
	uint result,quotient,i;
	result = square_and_multiply2(base,exp,mod);
	return result;
}

/**
	@brief		입력된 수가 소수인지 입력된 횟수만큼 반복하여 검증하는 함수.
	@param		uint testNum	: 임의 생성된 홀수.
	@param		uint repeat		: 판단함수의 반복횟수.
	@return		uint result		: 판단 결과에 따른 TRUE, FALSE 값.
	@todo		Miller-Rabin 소수 판별법과 같은 확률적인 방법을 사용하여,
				이론적으로 4N(99.99%) 이상 되는 값을 선택하도록 한다. 
*/
bool IsPrime(uint testNum, uint repeat) {
	if(testNum == 1)
		return TRUE;
	if(testNum == 2)
		return FALSE;
	if(testNum == 3)
		return TRUE;
	uint a,k,q,j,i;
	i = 0;
	while(i < repeat)
	{
		k = 0;
		q = testNum - 1;
		while((q & 1) == 0)
		{
			q = q >> 1;
			k++;
		}
		if((square_and_multiply(2,k)*q) != testNum-1)
			return FALSE;
		if(k <= 0)
			return FALSE;
			
		a = (int)(WELLRNG512a()*(testNum-1));
		while(a <= 1 || a >= testNum-1)
			a = (int)(WELLRNG512a()*(testNum-1));
				
		if(ModPow(a,q,testNum) == 1)
			return FALSE;
		for(j = 0; j < k-1; j++)
			if(ModPow(a,q*(square_and_multiply(2,j)),testNum) == testNum - 1)	
				return FALSE;
 		i++;
	}
	return TRUE;
}

uint GCD(uint a, uint b) {
	uint prev_a;

	while(b != 0) {
		printf("GCD(%u, %u)\n", a, b);
		prev_a = a;
		a = b;
		while(prev_a >= b) prev_a -= b;
		b = prev_a;
	}
	printf("GCD(%u, %u)\n\n", a, b);
	return a;
}

/**
	@brief		모듈러 역 값을 계산하는 함수.
	@param		uint a	: 피연산자1.
	@param		uint m	: 모듈러 값.
	@return		uint result	: 피연산자의 모듈러 역수 값.
	@todo		확장 유클리드 알고리즘을 사용하여 작성하도록 한다.
*/
uint ModInv(uint a, uint m) {
	uint inv, u1, u3, v1, v3, t1, t3, q;
	int iter;

	u1 = 1;
	u3 = a;
	v1 = 0;
	v3 = m;

	iter = 1;

	while (v3 != 0) {
		q = u3 / v3;
		t3 = u3 % v3;
		t1 = u1 + q * v1;
		u1 = v1;
		v1 = t1;
		u3 = v3;
		v3 = t3;
		iter *= -1;
	}

	if (u3 != 1)
		return 0;

	if (iter < 0)
		inv = m - u1;
	else
		inv = u1;

	return inv;
}
/**
	@brief		RSA 키를 생성하는 함수.
	@param		uint *p	: 소수 p.
	@param		uint *q	: 소수 q.
	@param		uint *e	: 공개키 값.
	@param		uint *d	: 개인키 값.
	@param		uint *n	: 모듈러 n 값.
	@return		void
	@todo		과제 안내 문서의 제한사항을 참고하여 작성한다.
*/
void MRSAKeygen(uint *p, uint *q, uint *e, uint *d, uint *n) {
	uint pi_n,ans,diff,pi_n2;
	
	printf("mRSA key generator start\n\n");
	
	
	*p = (uint)(WELLRNG512a()*(32768)) + 32768;
	printf("random-number1 %u selected.\n",*p);
	while(!IsPrime(*p,4))
	{
		printf("%u is not Prime.\n\n",*p);
		*p = (uint)(WELLRNG512a()*32768) + 32768;
		printf("random-number1 %u selected.\n",*p);
	}
	printf("%u may be Prime.\n\n",*p);
	
	*q = (uint)(WELLRNG512a()*(32768)) + 32768;
	printf("random-number2 %u selected.\n",*q);
	while(!IsPrime(*p,4))
	{
		printf("%u is not Prime.\n\n",*q);
		*p = (uint)(WELLRNG512a()*(32768)) + 32768;
		printf("random-number2 %u selected.\n",*q);
	}
	printf("%u may be Prime.\n\n",*q);
	
	while(root(ModMul(*p, *q,*n)) != 31)
	{
		*p = (uint)(WELLRNG512a()*(32768)) + 32768;
		printf("random-number1 %u selected.\n",*p);
		while(!IsPrime(*p,4))
		{
			printf("%u is not Prime.\n\n",*p);
			*p = (uint)(WELLRNG512a()*(32768)) + 32768;
			printf("random-number1 %u selected.\n",*p);
		}
		printf("%u may be Prime.\n\n",*p);
	
		*q = (uint)(WELLRNG512a()*32768) + 32768;
		printf("random-number2 %u selected.\n",*q);
		while(!IsPrime(*p,4))
		{
			printf("%u is not Prime.\n\n",*q);
			*p = (uint)(WELLRNG512a()*(32768)) + 32768;
			printf("random-number2 %u selected.\n",*q);
		}
		printf("%u may be Prime.\n\n",*q);
	}

	printf("finally selected prime p, q = %u, %u\n",*p, *q);


	*n = ModMul((*p), (*q), *n);
	printf("n : %u\n",*n);
	
	pi_n = ModMul((*p - 1), (*q - 1), *n);
	
	*e = (uint)(WELLRNG512a()*(pi_n - 1)) + 1;
	while(GCD(*e, pi_n) != 1)
		*e = (uint)(WELLRNG512a()*(pi_n - 1)) + 1;
	printf("e : %u selected.\n",*e);
	
	*d = ModInv(*e, pi_n);
	printf("d : %u selected.\n\n",*d);
	
	printf("e, d, n, pi_n : %u\t%u\t%u\t%u\n",*e,*d,*n,pi_n);
	
	printf("root(n) : %u(%u)\nroot(p) : %u(%u)\nroot(q) : %u(%u)\n",root(*n),*n,root(*p),*p,root(*q),*q);
	
	pi_n2 = root(pi_n);
	ans = (*e * *d) >> pi_n2;
	
	printf("e*d mod pi_n : %u\n\n",ans);
}

/**
	@brief		RSA 암복호화를 진행하는 함수.
	@param		FILE *ifp	: 입력 파일 포인터.
	@param		uint len	: 입력 파일 길이.
	@param		FILE *ofp	: 출력 파일 포인터.
	@param		uint key	: 키 값.
	@param		uint n		: 모듈러 n 값.
	@return		uint result	: 암복호화에 성공한 byte 수.
	@todo		과제 안내 문서의 제한사항을 참고하여 작성한다.
*/
uint MRSACipher(FILE *ifp, uint len, FILE *ofp, uint key, uint n) {
	printf("key : %u\tmod : %u\n",key,n);
	char buffer1[len];
	uint buffer2[len],buffer3[len],i,j,turnoff;
	turnoff = 0;
	bzero(buffer1,len);
//	bzero(buffer2,len);
//	bzero(buffer3,len);
	fread(buffer1,sizeof(char),len,ifp);
	fseek(ifp, 0, SEEK_SET);
	fread(buffer2,sizeof(uint),len,ifp);
	
//	for(i = 0; i < len; i++)
//		printf("buffer2[%d] : %u\n",i,buffer2[i]);
	
	printf("start\n");
	for (i = 0; turnoff != 1; i++)
	{
		printf("i : %d\n",i);
		if(buffer2[i] > n)
		{
			printf("block size > modular n : encrypt failed\n");
			continue;
		}
		
		buffer3[i] = ModPow(buffer2[i], key, n);
		
		printf("len : %u\nbuf : ",(len-i));
		for(j = i*4; j < 4 + i*4; j++)
		{
			if(buffer1[j] == '\0')
				turnoff = 1;
			printf("%c",buffer1[j]);
		}
		printf("\n");
		
		
		printf("ptx : %u\nctx : %u\n\n",buffer2[i],buffer3[i]);
		
	}
	printf("buffer3 : %u\n",buffer3[0]);
	fwrite(buffer3,1,len,ofp);
	return len;
}

int main(int argc, char const *argv[]) {
	uint seed = time(NULL);
	InitWELLRNG512a(&seed);

	FILE *data_fp, *enc_fp, *dec_fp;
	uint fsize;

	if(argc != 4) {
		printf("usage : ./rsa data_file encrypt_file decrypt_file\n");
		exit(1);
	}

	data_fp = fopen(argv[1], "rb");
	enc_fp = fopen(argv[2], "wb");
	if(data_fp == NULL | enc_fp == NULL) {
		printf("file open fail\n");
		exit(1);
	}

	fseek(data_fp, 0, SEEK_END);
	fsize = ftell(data_fp);
	printf("data file size : %u\n\n", fsize);
	fseek(data_fp, 0, SEEK_SET);

	MRSAKeygen(&p, &q, &e, &d, &n);
//	e = 3769658485;
//	d = 3400071421;
//	n = 3973763749;
	fsize = MRSACipher(data_fp, fsize, enc_fp, e, n);

	fclose(data_fp);
	fclose(enc_fp);

	enc_fp = fopen(argv[2], "rb");
	dec_fp = fopen(argv[3], "wb");
	if(dec_fp == NULL | enc_fp == NULL) {
		printf("file open fail\n");
		exit(1);
	}

	printf("encrypted file size : %u\n\n", fsize);

	fsize = MRSACipher(enc_fp, fsize, dec_fp, d, n);

	fclose(enc_fp);
	fclose(dec_fp);
}