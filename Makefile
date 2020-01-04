CC=g++
WC=x86_64-w64-mingw32-g++
DIR := ${CURDIR}
CFLAGS= -Wall -Wpedantic --std=c++11 -O3 -I$(DIR) -I$(DIR)/lib -L$(DIR)/lib #
DFLAGS = -g -ggdb
LFLAGS= -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
WFLAGS= -lmingw32 -mwindows
DDIR = debugging
XDIR = bin
BDIR = build
SDIR = src
WBDIR = build/wbuild
WDDIR= win_demo
WEXE= qdbp.exe

SPECIAL_LFLAGS = #-I$(DIR)/lib -L$(DIR)/lib -Wl,-R,$(DIR)/lib -Wl,--verbose


DEPS = engine.h render.h movable.h xmlparse.h xmlnode.h vec2d.h drawable.h debug_drawable.h viewport.h image.h timeframe.h chunk.h map.h map_handler.h message.h image_handler.h background.h rect2d.h hittable.h debug_hittable.h hitbox.h hitline.h debug_follower.h entity_handler.h

OBJS:= engine.o render.o xmlparse.o xmlnode.o vec2d.o debug_drawable.o viewport.o image.o timeframe.o chunk.o map.o message.o image_handler.o background.o rect2d.o debug_hittable.o drawable.o hittable.o hitbox.o hitline.o map_handler.o debug_follower.o entity_handler.o
WOBJS:= $(addprefix $(WBDIR)/,$(OBJS))
OBJS:= $(addprefix $(BDIR)/,$(OBJS))

SRCS = engine.cpp render.cpp xmlparse.cpp xmlnode.cpp vec2d.cpp debug_drawable.cpp viewport.cpp image.cpp timeframe.cpp chunk.cpp map.cpp message.cpp image_handler.cpp background.cpp rect2d.cpp debug_hittable.cpp drawable.cpp hittable.cpp hitbox.cpp hitline.cpp map_handler.cpp debug_follower.cpp entity_handler.cpp

PATHS = . movers renders vec2d xml_parser movers/drawable viewport image timeframe environment/chunk environment/map utils environment/background rect2d movers/drawable/hittable rect2d/hitbox entity_handler
VPATH = $(addprefix src/,$(PATHS))


#for all .o files: use the .c files and the deps
#compile them with
#				-c 			for an obeject file
#				-o $@ 	for giving it a name of the object file before the :
#				$< 			for the first prepreq to make this file, i think a .h
$(BDIR)/%.o: %.cpp %.h
	@mkdir -p $(BDIR)
	@printf "building %s\n" $@
	@$(CC) $(CFLAGS) -c -o $@ $<

$(WBDIR)/%.o: %.cpp %.h
	@mkdir -p $(BDIR)
	@mkdir -p $(WBDIR)
	@printf "building winobj %s\n" $@
	@$(WC) $(CFLAGS) $(WFLAGS) -c -o $@ $<

run: $(OBJS) main_driver.cpp
	@mkdir -p $(XDIR)
	@printf "final compilation... "
	@$(CC) $(CFLAGS) -o $(XDIR)/$@ $^ $(LFLAGS) $(SPECIAL_LFLAGS)
	@printf "compiled\ndone\n"

win: $(WOBJS) main_driver_w.cpp
	@mkdir -p $(WDDIR)
	@printf "final win compilation... "
	@$(WC) $(CFLAGS) $^ $(WFLAGS) $(LFLAGS) $(SPECIAL_LFLAGS) -o $(WDDIR)/$(WEXE)
	@printf "compiled\nadding resources... "
	@cp -r resources/ $(WDDIR)/
	@cp win_dll/*.dll $(WDDIR)/
	@printf "added\nzipping... "
	@zip -q -r win_demo.zip $(WDDIR)
	@paplay /usr/share/sounds/ubuntu/notifications/Positive.ogg
	@printf "zipped\ndone\n"

win2: $(OBJS) main_driver_w.cpp
	@mkdir -p $(WDDIR)
	$(WC) $(CFLAGS) $^ $(WFLAGS) $(LFLAGS) -o $(WDDIR)/$(WEXE)
	@cp -r resources/ $(WDDIR)/
	@cp win_dll/*.dll $(WDDIR)/
	zip -q -r win_demo.zip $(WDDIR)
	@paplay /usr/share/sounds/ubuntu/notifications/Positive.ogg

wintest:
	wine $(WDDIR)/$(WEXE)

winzip:
	zip -r win_demo.zip $(WDDIR)

dir:
	@printf "%s\n" $(DIR)

debug: $(SRCS) $(DEPS) main_driver.cpp
	@mkdir -p $(DDIR);
	$(CC) $(CFLAGS) $(DFLAGS) -o $(DDIR)/$@ $^ $(LFLAGS);
	@paplay /usr/share/sounds/ubuntu/notifications/Positive.ogg

clean:
	@$(RM) *.o *.gch run $(DDIR)/debug $(XDIR)/run $(BDIR)/*.o

winclean:
	@$(RM) -r win_demo/ win_demo.zip $(WBDIR)/*.o

mem:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=$(DDIR)/.v.out $(DDIR)/debug && \
	cat $(DDIR)/.v.out | awk '/HEAP SUMMARY/{p=1}p' > $(DDIR)/.v2.out && \
	sed 's/==.*== //' $(DDIR)/".v2.out" > $(DDIR)/"full-valgrind-out.txt" && \
	cat $(DDIR)/full-valgrind-out.txt > $(DDIR)/valgrind-out.txt && \
	perl -i -ne 'BEGIN{$$/=""} print unless (/SDL_.*Init/ or /X11_ShowCursor/ or  /dlopen\@\@GLIBC_2.2.5/ or /XSetLocaleModifiers/ or /_dl_catch_exception/ or /_XlcCurrentLC/ or /libpulsecommon/ or /SDL_CreateWindow_REAL/ or /lib\/x86_64-linux-gnu\/dri\/i965_dri.so/)' $(DDIR)/valgrind-out.txt;
	@rm $(DDIR)/.v.out $(DDIR)/.v2.out; less $(DDIR)/valgrind-out.txt;

#ffmpeg -ss 4.3 -t 6 -i sample3.mp4 -vf "fps=30,scale=1280:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 out5.gif
#use this to make gifs? simplescreenrecorder > ffmpeg > gifcompress.com
