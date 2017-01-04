/****** C 표준 라이브러리 *******/
#include <string.h>     // 문자열 처리
#include <stdio.h>      // 표준 I/O 라이브러리, 스트림 매개변수들이나 표준 스트림을 사용하는 함수들
#include <stdlib.h>
/****************************/
/***** C POSIX 라이브러리 *****/
#include <unistd.h>     // 다양한 필수 POSIX 함수와 상수
#include <dirent.h>     // 디렉터리 함수들
#include <sys/stat.h>   // 파일 정보, 시스템 호출들을 사용하는 경우 포함시키는 것을 권장.
/***************************/

void directoryScan(char* dir, int depth) {
    DIR* dp;                // 디렉터리 스트림(DIR 구조체의 포인터)
    /* 디렉터리에 담긴 개별 항목에 해당하는 구조체 dirent
     DIR 구조체의 필드들을 직접 변경할 수 없기 때문에 dirent 구조체를 사용한다.
     dirent 구조체 필드 (ino_t d_int: 파일의: inode 값, char d_name[]: 파일의 이름) */
    struct dirent* entry;
    struct stat statbuf;    // stat 구조체, 파일의 상태 정보를 담는 구조체
    
    /* dir 디렉터리를 열고 그에 대한 디렉터리 스트림을 반환한다.
     만약 디렉터리를 여는데 실패할 경우 널 포인트를 돌려준다. */
    if((dp = opendir(dir)) == NULL) {
        // dir 디렉터리를 여는데 실패할 경우
        // stderr 스트림에 다음과 같은 내용을 기록한다.
        // stderr는 <unistd.h> 에 의해 파일 디스크립터 2로 표현되기도 한다.
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return;     // directoryScan 함수 종료
    }
    
    chdir(dir); // 프로그램 내에서 현재 디렉토리를 dir로 변경한다.
    
    /* readdir 함수는 매개변수로 들어온 디렉터리 스트림 안의
     다음 디렉터리 항목에 대한 구조체의 포인터를 돌려준다.
     오류가 있었거나 디렉터리의 끝에 도달했다면 readdir는 NULL을 돌려준다. */
    while((entry = readdir(dp)) != NULL) {
        /* lstat는 entry->d_name(파일 이름)을 받아
         해당 파일에 연관된 파일의 상태 정보를 statbuf에 돌려준다.*/
        lstat(entry->d_name, &statbuf);
        /* 현재 탐색하고 있는 파일이 dot(.)으로 시작하면 무시한다. */
        if('.' == *(entry->d_name))
            continue;
        // 현재 탐색하고 있는 파일이 히든 파일이 아니라면 ...
        /* stat 구조체의 멤버 st_mode: 파일 권한들과 파일 종류 정보
         st_mode에 설정될 수 있는 여러 플래그 값들이 매크로 상수 형태로 sys/stat.h에 정의되어 있다.
         S_ISDIR 디렉터리에 대한 판정을 하는 마스크다.*/
        // 지금 탐색하고 있는 파일이 디렉토리라면 ...
        if(S_ISDIR(statbuf.st_mode)) {
            /* 디렉터리를 찾았음.
             디렉토리 이름을 출력한다.
             printf의 %*s 서식은 (정수, 문자열)의 인수를 갖는다.
             정수만큼 문자열이 띄어쓰기 되어 출력된다. */
            printf("%*s%s/\n", depth, "", entry->d_name);
            /* 새로운 들여쓰기 수준을 지정해서 재귀 호출 */
            directoryScan(entry->d_name,depth+4);
        }
        // 지금 탐색하고 있는 파일이 디렉토리가 아니라면 ...
        else
            // 파일 이름을 출력한다.
            printf("%*s%s\n",depth,"",entry->d_name);
    }
    // 프로그램 내에서 현재 디렉토리를 상위 디렉토리로 변경한다.
    // 재귀가 무한히 반복되는 일을 피하기 위해, 상위 디렉터리로 돌아간다.
    chdir("..");
    // dp 디렉터리 스트림을 닫고 그에 연관된 자원들을 해제한다.
    // 필요 이상으로 많은 디렉터리 스트림이 열려 있지 않도록 호출한다.
    closedir(dp);
}

int main(int argc, char** argv) {
    /* 탐색할 디렉터리의 최상위 디렉터리.
    초기값은 프로그램이 실행되는 현재 디렉터리다. */
    char *topdir = ".";      
    
    // 실행인자가 2개 이상일 경우
    if (argc >= 2)
        // 1번째 실행인자를 탐색할 디렉터리의 최상위 디렉터리로 한다.
        topdir=argv[1];
    
    // topdir에서 디렉터리 탐색을 시작한다.
    printf("Directory scan of %s:\n",topdir);
    directoryScan(topdir, 0);
    printf("done.\n");
    
    exit(0);
}
