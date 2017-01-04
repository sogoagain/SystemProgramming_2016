#include <stdio.h>
#include <stdlib.h> // setenv(), 외부 변수 environ
#include <string.h>

extern char **environ;  // 외부 참조 변수로 선언

int main() {

    // environ을 참조
    char **env;

    // setenv()는 애플리케이션 내에서만 유효
    // $TEST_ENV 환경변수 값을 "1234"로 설정한다.
    // 만일 이미 같은 이름의 변수가 있다면 값을 변경한다.
    // setenv()는 성공하면 0, 실패하면 -1을 반환한다.
    if (setenv( "TEST_ENV", "1234", 1) == -1) {
        perror("환경변수 설정에 실패했습니다.");
        return -1;
    }

    env = environ;
    
    // environ 변수가 가리키는 배열을 훑으면서
    // 배열의 각 원소를 출력한다.
    while(*env) {
        printf("%s\n",*env);
        env++;
    }

    exit(0);    
}
