CC=g++
CFLAGS= -Wall --std=c++11
DFLAGS = -g -ggdb
LFLAGS= -lSDL2 -lSDL2_image

DEPS = engine.h texture.h render.h movable.h xml_parser/xmlparse.h xml_parser/xmlnode.h
OBJS = main_driver.o engine.o texture.o render.o xml_parser/xmlparse.o xml_parser/xmlnode.o
SRCS = main_driver.cpp engine.cpp texture.cpp render.cpp xml_parser/xmlparse.cpp xml_parser/xmlnode.cpp

#for all .o files: use the .c files and the deps
#compile them with
#				-c 			for an obeject file
#				-o $@ 	for giving it a name of the object file before the :
#				$< 			for the first prepreq to make this file, i think a .h
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

debug: $(SRCS) $(DEPS)
	$(CC) $(CFLAGS) $(DFLAGS) -o $@ $^ $(LFLAGS)


clean:
	$(RM) *.o *.gch run debug vec2d/*.o vec2d/*.gch xml_parser/*.o xml_parser/*.gch

mem:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./debug; less valgrind-out.txt
