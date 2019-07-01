CC=g++
DIR := ${CURDIR}
CFLAGS= -Wall --std=c++11 -I $(DIR)
DFLAGS = -g -ggdb
LFLAGS= -lSDL2 -lSDL2_image
DDIR = debugging
XDIR = bin
BDIR = build

DEPS = engine.h texture.h render.h movable.h xmlparse.h xmlnode.h vec2d.h debug_movable.h drawable.h debug_drawable.h
OBJS:= main_driver.o engine.o texture.o render.o xmlparse.o xmlnode.o vec2d.o debug_movable.o debug_drawable.o
OBJS:= $(addprefix $(BDIR)/,$(OBJS))
SRCS = main_driver.cpp engine.cpp texture.cpp render.cpp xmlparse.cpp xmlnode.cpp vec2d.cpp debug_movable.cpp debug_drawable.cpp
PATHS = . movers renders vec2d xml_parser movers/drawable
VPATH = $(addprefix src/,$(PATHS))

#for all .o files: use the .c files and the deps
#compile them with
#				-c 			for an obeject file
#				-o $@ 	for giving it a name of the object file before the :
#				$< 			for the first prepreq to make this file, i think a .h
$(BDIR)/%.o: %.cpp %.h
	@mkdir -p $(BDIR)
	@echo 'building ' $@
	@$(CC) $(CFLAGS) -c -o $@ $<

run: $(OBJS)
	@mkdir -p $(XDIR);
	@echo -n 'final compilation... '
	@$(CC) $(CFLAGS) -o $(XDIR)/$@ $^ $(LFLAGS)
	@echo 'done'

dir:
	@echo $(DIR)

debug: $(SRCS) $(DEPS)
	@mkdir -p $(DDIR);
	$(CC) $(CFLAGS) $(DFLAGS) -o $(DDIR)/$@ $^ $(LFLAGS)


clean:
	@$(RM) *.o *.gch run $(DDIR)/debug vec2d/*.o xml_parser/*.o xml_parser/run movers/*.o renders/*.o $(XDIR)/run $(BDIR)/*.o

mem:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=$(DDIR)/.v.out $(DDIR)/debug && \
	cat $(DDIR)/.v.out | awk '/HEAP SUMMARY/{p=1}p' > $(DDIR)/.v2.out && \
	sed 's/==.*== //' $(DDIR)/".v2.out" > $(DDIR)/"full-valgrind-out.txt" && \
	cat $(DDIR)/full-valgrind-out.txt > $(DDIR)/valgrind-out.txt && \
	perl -i -ne 'BEGIN{$$/=""} print unless (/SDL_.*Init/ or /X11_ShowCursor/ or  /dlopen\@\@GLIBC_2.2.5/ or /XSetLocaleModifiers/ or /_dl_catch_exception/ or /_XlcCurrentLC/ or /libpulsecommon/)' $(DDIR)/valgrind-out.txt;
	@rm $(DDIR)/.v.out $(DDIR)/.v2.out; less $(DDIR)/valgrind-out.txt;
