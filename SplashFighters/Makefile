#
#Makefile
#
#このファイルを書き換えて,cygwinでmakeコマンドが使えるようになる

#最終目的のファイル
TARGET = splashfighters.exe

#ソースファイル(*.c)の一覧
SRCS = splashfighters.c


#オブジェクトファイル(*.o)の一覧
OBJS = ${SRCS:.c=.o}

#ヘッダーファイルの一覧(今は何もなし)
HEADERS =

#コンパイラ・リンカの指定
CC = gcc
CCFLAGS = -Wall -I/usr/include/opengl
LD = gcc
LDFLAGS =
LIBS = -lglpng -lglut32 -lglu32 -lopengl32 myicon.o

#OBJSからTARGETを作る方法
$(TARGET) : $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET) $(LIBS)

#*.cから*.oを作る方法
.c.o:
	$(CC) $(CCFLAGS) -c $<

#*.oはHEADERSとMakefileに存在(これらが書き換わったときにも*.oを再構築)
$(OBJS) : $(HEADERS) Makefile

#make cleanとしたときに実行されるコマンド
clean :
	rm -f $(TARGET) $(OBJS) core *~