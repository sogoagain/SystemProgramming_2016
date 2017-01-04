/*
 * 파일명: mtest.c
 * 내용: 실행인자로 들어온 두개의 정수에 대해 덧셈, 뺄셈, 곱셈 결과를 출력한다. 연산에는 libfooboobar에 정의된 InFoo, InBoo, InBar 함수를 사용한다.
 */

#include "libfooboobar.h"
#include <stdlib.h>

int main(int argc, char* argv[]) {

    int num[2];

    if (argc != 3) {
        printf("Usage: %s num1 num2\n", argv[0]);
        return 1;
    }

    num[0] = atoi(argv[1]);
    num[1] = atoi(argv[2]);

    printf("%d + %d = %d\n", num[0], num[1], InFoo(num[0], num[1])); 
    printf("%d - %d = %d\n", num[0], num[1], InBoo(num[0], num[1]));
    printf("%d * %d = %d\n", num[0], num[1], InBar(num[0], num[1]));

    return 0;
}
