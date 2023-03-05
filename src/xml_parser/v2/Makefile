no-sanitized:
	g++ -I ../../.. test.cpp xml_handler.cpp xmlnode_v2.cpp ../../utils/message.cpp -lstdc++fs  

sanitized:
	g++ -I ../../.. test.cpp xml_handler.cpp xmlnode_v2.cpp ../../utils/message.cpp -lstdc++fs -fsanitize=address

valg:
	valgrind ./a.out

gdb:
	gdb -tui ./a.out
