/*
 * 파일명: boo.c
 * 내용: InBoo 함수를 정의한다.
 *       InBoo 함수는 두 정수를 매개변수로 받아 뺄셈 결과를 반환한다.
 */
#include "libfooboobar.h"

int InBoo(int num1, int num2) {
    printf("You are in Boo!!!\n");
    printf("Foo returns the results of subtraction.\n");

    return num1 - num2;
}
