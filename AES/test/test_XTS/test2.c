#include <stdio.h>
#include<string.h>
#define is_mul_16(x) (x) % 16 == 0
int main()
{
	int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	printf("%d\n%d\n",*(a+4),a[4]);
	int size = 15;
	int block_num = is_mul_16(size) ? 1 : 0;
	int loop;
	printf("block_num : %d %d\n",block_num,size);
	size = 18;
	block_num = is_mul_16(size) ? 1 : 0;
	printf("block_num : %d %d\n", block_num,size);
	loop = is_mul_16(size) ? block_num : block_num - 2;
	printf("loop : %d\n",loop);
	int x,y;
	for(x = y = 0; x < 3; x++)
		printf("%d %d\n",x,y);
	for(x = 0, y = 0; x < 3; x++)
		printf("%d %d\n", x ,y);
	return 0;
}
