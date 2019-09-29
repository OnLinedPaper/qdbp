CC=g++
WC=x86_64-w64-mingw32-g++
DIR := ${CURDIR}
CFLAGS= -Wall -Wpedantic --std=c++11 -O3 -I $(DIR)
DFLAGS = -g -ggdb
LFLAGS= -lSDL2main -lSDL2 -lSDL2_image
WFLAGS= -lmingw32 -mwindows
DDIR = debugging
XDIR = bin
BDIR = build
SDIR = src
WDDIR= win_demo
SDLLIBS=`sdl-config --cflags --libs`
WEXE= qdbp.exe


DEPS = engine.h render.h movable.h xmlparse.h xmlnode.h vec2d.h drawable.h debug_drawable.h viewport.h image.h timeframe.h chunk.h map.h map_handler.h message.h image_handler.h background.h rect2d.h hittable.h debug_hittable.h hitbox.h hitline.h

OBJS:= engine.o render.o xmlparse.o xmlnode.o vec2d.o debug_drawable.o viewport.o image.o timeframe.o chunk.o map.o message.o image_handler.o background.o rect2d.o debug_hittable.o drawable.o hittable.o hitbox.o hitline.o
OBJS:= $(addprefix $(BDIR)/,$(OBJS))

SRCS = engine.cpp render.cpp xmlparse.cpp xmlnode.cpp vec2d.cpp debug_drawable.cpp viewport.cpp image.cpp timeframe.cpp chunk.cpp map.cpp message.cpp image_handler.cpp background.cpp rect2d.cpp debug_hittable.cpp drawable.cpp hittable.cpp hitbox.cpp hitline.cpp

PATHS = . movers renders vec2d xml_parser movers/drawable viewport image timeframe environment/chunk environment/map utils environment/background rect2d movers/drawable/hittable rect2d/hitbox
VPATH = $(addprefix src/,$(PATHS))


#for all .o files: use the .c files and the deps
#compile them with
#				-c 			for an obeject file
#				-o $@ 	for giving it a name of the object file before the :
#				$< 			for the first prepreq to make this file, i think a .h
$(BDIR)/%.o: %.cpp %.h
	@mkdir -p $(BDIR)
	@echo 'building' $@
	@$(CC) $(CFLAGS) -c -o $@ $<

run: $(OBJS) main_driver.cpp
	@mkdir -p $(XDIR);
	@echo -n 'final compilation... '
	@$(CC) $(CFLAGS) -o $(XDIR)/$@ $^ $(LFLAGS) $(SDLLIBS)
	@echo 'done'

win: $(SRCS) $(DEPS) main_driver_w.cpp
	@mkdir -p $(WDDIR);
	$(WC) $(CFLAGS) $^ $(WFLAGS) $(LFLAGS) -o $(WDDIR)/$(WEXE)
	@cp -r resources/ $(WDDIR)/
	@cp win_dll/*.dll $(WDDIR)/
	zip -r win_demo.zip $(WDDIR)
	@paplay /usr/share/sounds/ubuntu/notifications/Positive.ogg

wintest:
	wine $(WDDIR)/$(WEXE)

winzip:
	zip -r win_demo.zip $(WDDIR)

dir:
	@echo $(DIR)

debug: $(SRCS) $(DEPS) main_driver.cpp
	@mkdir -p $(DDIR);
	$(CC) $(CFLAGS) $(DFLAGS) -o $(DDIR)/$@ $^ $(LFLAGS) $(SDLLIBS);
	@paplay /usr/share/sounds/ubuntu/notifications/Positive.ogg

clean:
	@$(RM) *.o *.gch run $(DDIR)/debug $(XDIR)/run $(BDIR)/*.o

winclean:
	@$(RM) -r win_demo/ win_demo.zip

mem:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=$(DDIR)/.v.out $(DDIR)/debug && \
	cat $(DDIR)/.v.out | awk '/HEAP SUMMARY/{p=1}p' > $(DDIR)/.v2.out && \
	sed 's/==.*== //' $(DDIR)/".v2.out" > $(DDIR)/"full-valgrind-out.txt" && \
	cat $(DDIR)/full-valgrind-out.txt > $(DDIR)/valgrind-out.txt && \
	perl -i -ne 'BEGIN{$$/=""} print unless (/SDL_.*Init/ or /X11_ShowCursor/ or  /dlopen\@\@GLIBC_2.2.5/ or /XSetLocaleModifiers/ or /_dl_catch_exception/ or /_XlcCurrentLC/ or /libpulsecommon/)' $(DDIR)/valgrind-out.txt;
	@rm $(DDIR)/.v.out $(DDIR)/.v2.out; less $(DDIR)/valgrind-out.txt;

#ffmpeg -ss 4.3 -t 6 -i sample3.mp4 -vf "fps=30,scale=1280:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 out5.gif
#use this to make gifs? simplescreenrecorder > ffmpeg > gifcompress.com
