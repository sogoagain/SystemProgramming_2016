/*
 * 파일명: bar.c
 * 내용: InBar 함수를 정의한다.
 *       InBar 함수는 두 정수를 매개변수로 받아 곱셈 결과를 반환한다.
 */
#include "libfooboobar.h"

int InBar(int num1, int num2) {
    printf("You are in Bar!!!\n");
    printf("Foo returns the results of multiplication.\n");

    return num1 * num2;
}
