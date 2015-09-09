.SUFFIXES:.cpp .o

CC = g++
SRCS = myQQServer.cpp\
		pub.cpp\
		work.cpp
OBJS = $(SRCS:.cpp=.o)

EXE = myQQServer

all:$(OBJS)
	$(CC) -o $(EXE) $(OBJS)	-lmysqlclient
	@echo 'makefile ok'

.cpp.o:
	$(CC) -Wall -g -o $@ -c $<

clean:	
	-rm -f $(OBJS)
	-rm -f core*