#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 16

int main(int argc, char* argv[]) {
    // 출력할 환경 변수들
    char var[][MAX_LENGTH] = {"HOME","PS1","PATH","LD_LIBRARY_PATH"};
    char* value;    // 환경 변수의 값
    int i = 0;
    int number_of_var = 0;  // 출력할 환경 변수들의 갯수

    number_of_var = sizeof(var)/sizeof(var[0]);

    // getenv()를 이용하여 각 환경변수들의 값을 출력
    for(i = 0; i < number_of_var; i++) {
        value = getenv(var[i]);
        printf("Variable %s has value %s\n", var[i], value);
    }

    exit(0);
}
