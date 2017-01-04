/*
 * 파일명: foo.c
 * 내용: InFoo 함수를 정의한다.
 *       InFoo 함수는 두 정수를 매개변수로 받아, 덧셈 결과를 반환한다.
 */
#include "libfooboobar.h"

int InFoo(int num1, int num2) {
    printf("You are in Foo!!!\n");
    printf("Foo returns the results of addition.\n");

    return num1 + num2;
}
