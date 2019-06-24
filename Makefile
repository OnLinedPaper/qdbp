CC=g++
CFLAGS= -Wall --std=c++11
DFLAGS = -g -ggdb
LFLAGS= -lSDL2 -lSDL2_image

DEPS = engine.h renders/texture.h renders/render.h movers/movable.h xml_parser/xmlparse.h xml_parser/xmlnode.h vec2d/vec2d.h movers/debug_draw.h
OBJS = main_driver.o engine.o renders/texture.o renders/render.o xml_parser/xmlparse.o xml_parser/xmlnode.o vec2d/vec2d.o movers/debug_draw.o
SRCS = main_driver.cpp engine.cpp renders/texture.cpp renders/render.cpp xml_parser/xmlparse.cpp xml_parser/xmlnode.cpp vec2d/vec2d.cpp movers/debug_draw.cpp

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
	$(CC) $(CFLAGS) $(DFLAGS) -o debugging/$@ $^ $(LFLAGS)


clean:
	$(RM) *.o *.gch run debugging/debug vec2d/*.o xml_parser/*.o movers/*.o renders/*.o

mem:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=debugging/.v.out ./debugging/debug && \
	cd debugging && \
	cat .v.out | awk '/HEAP SUMMARY/{p=1}p' > .v2.out && \
	sed 's/==.*== //' ".v2.out" > "full-valgrind-out.txt" && \
	cat full-valgrind-out.txt > valgrind-out.txt && \
	perl -i -ne 'BEGIN{$$/=""} print unless (/SDL_/ or /dlopen\@\@GLIBC_2.2.5/ or /XSetLocaleModifiers/ or /_dl_catch_exception/ or /_XlcCurrentLC/ or /libpulsecommon/)' valgrind-out.txt;
	rm debugging/.v.out debugging/.v2.out; less debugging/valgrind-out.txt; cd ..
