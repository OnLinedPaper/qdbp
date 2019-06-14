CC = g++
CFLAGS = -Wall --std=c++11
DFLAGS = -g -ggdb

DEPS = xmlnode.h xmlparse.h
OBJS = xmlnode.o xmlparse.o main.o
SRCS = xmlnode.cpp xmlparse.cpp main.cpp

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(OBJS)
	$(CC) $(CLFAGS) -o $@ $^

debug: $(SRCS) $(DEPS)
	$(CC) $(CFLAGS) $(DFLAGS) -o $@ $^

clean:
	$(RM) *.o *.gch run debug

mem:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./run; less valgrind-out.txt
