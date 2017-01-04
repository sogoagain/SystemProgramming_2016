#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

/* 터미널 출력부분의 초기 행, 열 값 */
#define ROW_INIT 12
#define COL_INIT 0

#define TIME_INTERVAL 5

/* 메뉴 문자열 */
char *menu[] = {
    "a - Send alarm to ptest",
    "k - Force quit ptest",
    "s - Start ptest",
    "r - Restart ptest",
    "q - exit pmon",
    NULL,
};

/*
 get_pid_of_ptest: ptest의 pid 값을 구한다.
 check_ptest: 현재 ptest가 실행중인지 아닌지를 판단한다.
 */
int get_pid_of_ptest(void);
void check_ptest(void);

/*
 send_alarm: ptest에 alarm signal을 전송
 kill_ptest: ptest가 실행중이라면 프로세스를 죽인다.
 start_ptest: ptest를 새로운 터미널에서 실행시킨다.
 restart_ptest: ptest를 새로 실행시킨다.
 */
void send_alarm(void);
int kill_ptest(void);
int start_ptest(char* path);
int restart_ptest(char* path);

/*
 get_choice: pmon에서 사용자의 목적을 입력받는다.
 print_menu: menu를 출력한다.
 */
int get_choice(char* choices[]);
void print_menu(char *menu[]);

/* SIGTERM 핸들러 */
void hand_sigterm(int sig) {
    exit(0);
}

int main(int argc, char* argv[]) {
    pid_t pid;      /* fork()시 사용할 프로세스 아이디 */
    char buf[80];   /* ptest 경로 */
    FILE* fp;       /* config.cfg 파일 스트림 */
    int choice = 0; /* 사용자 입력 값 */
    int stat_val;   /* 자식 프로세스의 종료 상태 */
    
    /* curses 라이브러리 초기화 */
    initscr();
    
    signal(SIGTERM, hand_sigterm);
    /* config.cfg에 저장된 ptest 경로를 buf로 받아온다. */
    fp = fopen("config.cfg", "r");
    
    while(fgets(buf, 80, fp) != NULL) {
        buf[strlen(buf) - 1] =0x00;
    }
    fclose(fp);
    
    /* 메뉴를 출력한다 */
    print_menu(menu);
    
    /* 프로세스를 복제한다 */
    pid = fork();
    switch(pid) {
            /* fork 실패시 */
        case -1:
            perror("fork failed.");
            exit(1);
            
            /* fork 성공시 자식 프로세스의 행동
             자식 프로세스는 5초 단위로 ptest를 검사한다. */
        case 0:
            while(1) {
                check_ptest();
                sleep(5);
            }
            exit(0);
            
            /* fork는 부모 프로세스에서는 새 자식 프로세스의 pid를 돌려준다. */
        default:
            break;
    }
    /* 여기서 부터 부모 프로세스
     부모 프로세스는 사용자로 부터 명령을 받아 그것을 시행한다. */
    do {
        choice = get_choice(menu);
        
        switch(choice) {
            case 'a':
                send_alarm();
                break;
            case 'k':
                kill_ptest();
                break;
            case 's':
                start_ptest(buf);
                break;
            case 'r':
                restart_ptest(buf);
                break;
                
                /* q입력시 무한루프에 빠져있는 자식 프로세스를 시그널을 이용해 종료시킨다. */
            case 'q':
                if (kill(pid, SIGTERM) == -1)
                    kill(pid, SIGKILL);
                break;
            default:
                break;
        }
    } while(choice != 'q');
    
    move(ROW_INIT + 2, 0);
    clrtoeol();
    printw("exit pmon.");
    refresh();
    
    /* 자식 프로세스의 종료를 기다린다. */
    wait(&stat_val);

    move(ROW_INIT + 3.0, 0);
    clrtoeol();
    if(WIFEXITED(stat_val))
        printw("Child exited with code %d",WEXITSTATUS(stat_val));
    else
        printw("Child terminated abnormally\n");
    refresh();
    
    sleep(2);
    /* 윈도우를 정리하고 pmon을 종료한다. */
    move(LINES - 1, COLS -1);
    refresh();
    endwin();
    
    exit(0);
}

/* 
 get_pid_of_ptest: ptest의 pid 값을 구한다.
 성공하면(ptest가 실행중이면 -1이 아닌 값을
 실패하면(ptest가 실행중이 아니면) -1을 반환한다.
 */
int get_pid_of_ptest(void)
{
    pid_t pid = -1;
    FILE *read_fp;
    char buffer[BUFSIZ + 1];
    int chars_read;
    
    memset(buffer, '\0', sizeof(buffer));
    
    /*
     popen과 pclose를 이용해서 외부 프로그램의 출력을 읽을 수 있다.
     다음은 pidof 명령어를 이용하여 ptest의 pid를 알아낸 후
     popen을 이용해 buffer로 가져온다.
     */
    read_fp = popen("pidof ptest", "r");
    
    if(read_fp != NULL) {
        chars_read = fread(buffer, sizeof(char), BUFSIZ, read_fp);
        if(chars_read > 0) {
            pid = atoi(buffer);
        }
        pclose(read_fp);
        return pid;
    }
    return pid;
}

/*
 ptest의 현재 상태를 체크하고 출력한다.
 */
void check_ptest(void) {
    pid_t test_PID;
    static int i = 0;
    
    test_PID = get_pid_of_ptest();
    
    move(ROW_INIT + 0, 0);
    clrtoeol();
    if(test_PID == -1)
        printw("not existed: %ds",i*TIME_INTERVAL);
    else
        printw("running...: %ds",i*TIME_INTERVAL);
    move(ROW_INIT + 1, 0);
    printw(">>");
    move(ROW_INIT + 1, 3);
    refresh();
    i++;
}

/*
 ptest에 SIGALRM을 보낸다.
 */
void send_alarm(void) {
    pid_t test_PID;
    test_PID = get_pid_of_ptest();

    move(ROW_INIT + 2, 0);
    clrtoeol();
    if(test_PID == -1)
        printw("can't send signal.");
    else {
        printw("send alarm signal to ptest.");
        kill(test_PID, SIGALRM);
    }
    move(ROW_INIT + 1, 3);
    refresh();
}
/*
 ptest 프로세스를 kill한다.
 kill하기 전에 먼저 실행여부를 확인한다.
 kill하면 0을 그렇지 못하면 -1을 리턴한다.
 */
int kill_ptest(void) {
    pid_t test_PID;
    test_PID = get_pid_of_ptest();
    
    move(ROW_INIT + 2, 0);
    clrtoeol();
    if(test_PID != -1) {
        printw("Force exit ptest.");
        if (kill(test_PID, SIGTERM) == -1)
            kill(test_PID, SIGKILL);
        move(ROW_INIT + 1, 3);
        refresh();
        return 0;
    }
    printw("ptest is not runnig now.");
    move(ROW_INIT + 1, 3);
    refresh();
    return -1;
}

/*
 ptest 프로세스를 시작한다.
 main의 buf에 있는 경로를 받아와 새로운 터미널로 실행시킨다.
 이미 실행중이라면 실행하지 않는다.
 성공적으로 실행하면 0을
 그렇지 못하면 -1을 리턴한다.
 */
int start_ptest(char* path) {
    char cmd_str[1024];
    pid_t test_PID;
    test_PID = get_pid_of_ptest();
    
    sprintf(cmd_str,"gnome-terminal -e %s",path);
    
    move(ROW_INIT + 2, 0);
    clrtoeol();
    if(test_PID != -1) {
        printw("already running.");
        move(ROW_INIT + 1, 3);
        refresh();
        return -1;
    }
    move(ROW_INIT + 2, 0);
    clrtoeol();
    printw("ptest now starting...");
    refresh();
    move(ROW_INIT + 1, 3);
    
    system(cmd_str);
    
    return 0;
    
}

/*
 ptest 프로세스를 kill한뒤 다시 시작한다.
 kill_ptest를 호출하여 -1이 호출되면 (현재 ptest가 실행중이지 않다면) 새로 시작한다는 문구를 출력한다.
 그 이외의 경우에는 문구 출력없이 새로 시작한다.
 */
int restart_ptest(char* path) {
    char cmd_str[1024];
    sprintf(cmd_str,"gnome-terminal -e %s",path);

    move(ROW_INIT + 2, 0);
    clrtoeol();
    printw("Force restart ptest.");
    move(ROW_INIT + 1, 3);
    refresh();

    if (kill_ptest() == -1) {
        move(ROW_INIT + 2, 0);
        clrtoeol();
        printw("newly started.");
        move(ROW_INIT + 1, 3);
        refresh();
    }
    
    system(cmd_str);
    return 0;
}

/*
 사용자 입력을 받는다.
 */
int get_choice(char* choices[]) {
    int chosen = 0;
    char selected;
    char **option;
    int i;
    
    do {
        move(ROW_INIT + 1, 0);
        printw(">> ");
        move(ROW_INIT + 1, 3);
        refresh();
        do {
            selected = getch();
        } while(selected == '\n');
        
        option = choices;
        
        while(*option) {
            if(selected == *option[0]) {
                chosen = 1;
                break;
            }
            option++;
        }
        
        if(!chosen) {
            move(ROW_INIT + 2, 0);
            clrtoeol();
            printw("Incorrect choice, select again");
            refresh();
        }
    } while(!chosen);
    return selected;
}

/* 메뉴를 출력한다. */
void print_menu(char *menu[]) {
    char **option;
    int screenrow = 1;
    option = menu;
    
    move(screenrow, 20);
    attron(A_STANDOUT);
    printw("%s", "<---menu--->");
    attroff(A_STANDOUT);
    refresh();
    screenrow++;

    while(*option) {
        move(screenrow, 15);
        attron(A_BOLD);
        printw("%s", *option);
        attroff(A_BOLD);
        refresh();
        screenrow++;
        option++;
    }

    move(screenrow, 20);
    attron(A_STANDOUT);
    printw("%s","/----------/");
    attroff(A_STANDOUT);
    refresh();
    
}
