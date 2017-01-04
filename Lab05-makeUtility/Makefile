# Makefile의 첫 대상의 이름을 all로 하고
# 그 다음에 all이 의존하는 대상들을 나열하는 것이다.
# make 명령에 대상을 지정하지 않았을 때
# 어떤 것이 만들어지는지 알 수 있다.
# all이라는 유사대상은 여러 개의 최종 파일들을
# 생성해야 되는 경우에 관례적으로 사용한다.
all: mtest

# 컴파일러 지정
CC = gcc

# 설치할 디렉터리
INSTDIR = ./fooboobar

# 헤더 파일 위치
INCLUDE = .

# 라이브러리 파일 위치
LIBPATH = .

# 개발용 옵션
CFLAGS = -g -Wall -ansi

# 릴리스용 옵션
# CFLAGS = -O -Wall -ansi

# 로컬 라이브러리
MYLIB = libfooboobar.a

# 실행 파일 mtest는 mtest.o와 libfooboobar.a에 의존한다.
mtest: mtest.o $(MYLIB)
# gcc -I. -g -Wall -ansi -o mtest mtest.o -L. libfooboobar.a
	$(CC) -I$(INCLUDE) $(CFLAGS) -o mtest mtest.o -L$(LIBPATH) $(MYLIB)

# libfooboobar.a는 foo.o, boo.o, bar.o에 의존적이며
# foo.o, boo.o, bar.o는 libfooboobar.a에 들어 있는 오브젝트 파일이다.
$(MYLIB): $(MYLIB)(foo.o) $(MYLIB)(boo.o) $(MYLIB)(bar.o)

# 각 오브젝트 파일의 의존성을 명시하였다.
mtest.o: mtest.c libfooboobar.h
foo.o: foo.c libfooboobar.h
boo.o: boo.c libfooboobar.h
bar.o: bar.c libfooboobar.h

# .c.o:라는 의존성 정의는 접미어가 .c인 파일을 그 아래의 규칙을 이용해서
# .o 파일로 변환하라고 make에게 알려주는 역할을 한다.
# 소스 파일을 오브젝트 파일로 컴파일 할 때 해당 명령줄을 거치게된다.
# $<는 현재 의존물의 이름을 나타내는 매크로다.
.c.o:
	$(CC) -I$(INCLUDE) $(CFLAGS) -c $<

# 오브젝트 파일들을 정적 라이브러리로 묶을 때 해당 며영줄을 거치게 된다.
# ar의 U옵션은 오브젝트 파일의 타임 스탬프를 체크해
# 새로운 파일일 경우에만 치환한다는 옵션이다.
.o.a:
	ar rvU $(MYLIB) $<

# clean 댜성운 rm 명령을 이용해서 목적 파일들을 삭제한다.
# rm앞에 -를 붙였기 때문에 make는 rm이 보고한 오류를 무시한다.
clean:
	-rm mtest.o foo.o boo.o bar.o $(MYLIB)

# install 대상의 규칙은 응용프로그램이 최종적인 장소에
# 설치될 때까지 여러 개의 명령들을 차례로 수행한다.
# 스크립트 명령들 전부가 하나의 논리적인 스크립트 문장으로 실행되도록
# 각 줄 끝에 \를 붙였다.
install: mtest
	@if [ -d $(INSTDIR) ]; \
		then \
		cp mtest $(INSTDIR); \
		chmod a+x $(INSTDIR)/mtest; \
		chmod og-w $(INSTDIR)/mtest; \
		echo "Installed in $(INSTDIR)"; \
	else \
		echo "Sorry, $(INSTDIR) does not exist"; \
	fi
