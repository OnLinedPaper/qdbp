no-sanitized:
	g++ -I ../.. test.cpp xmlparse.cpp xmlnode.cpp ../utils/message.cpp -lstdc++fs  

sanitized:
	g++ -I ../.. test.cpp xmlparse.cpp xmlnode.cpp ../utils/message.cpp -lstdc++fs -fsanitize=address

valg:
	valgrind ./a.out

gdb:
	gdb -tui ./a.out
