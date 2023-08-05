CC=g++
WC=x86_64-w64-mingw32-g++
DIR := ${CURDIR}
CFLAGS= -Wall -Wextra -Wpedantic --std=c++17 -I$(DIR) -fstack-protector-all #-fsanitize=address
OFLAGS = -O3 -g
DFLAGS = -g -ggdb -O0 #-fstack-protector-all #-fsanitize=address #-fno-stack-protector
LFLAGS= -lstdc++fs -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lncurses
MFLAGS= -DRENDER_SDL
WFLAGS= -lmingw32 -mwindows -I$(DIR)/lib -L$(DIR)/lib
DDIR = debugging
XDIR = bin
BDIR = build
SDIR = src
DBDIR = build/dbuild
WBDIR = build/wbuild
WDDIR= win_demo
WEXE= qdbp.exe

SPECIAL_LFLAGS = #-I$(DIR)/lib -L$(DIR)/lib -Wl,-R,$(DIR)/lib -Wl,--verbose


DEPS = background.h chunk.h debug_follower.h debug_hittable.h debug_killable.h debug_player.o drawable.h engine.h entity_handler.h gunner.h hitbox.h hitline.h hud.h hybrid_box.h image.h image_handler.h iamge_handler_nc.h map.h map_handler.h message.h mortal.h movable.h pather.h player.h rect2d.h render.h render_nc.h rng.h text.h text_handler.h timeframe.h vec2d.h viewport.h weapon.h xmlnode.h xmlparse.h 

OBJS:= image_handler_nc.o background.o chunk.o debug_follower.o debug_hittable.o debug_killable.o debug_player.o drawable.o engine.o entity_handler.o gunner.o hitbox.o hitline.o hud.o hybrid_box.o image.o image_handler.o image_handler_nc.o map.o map_handler.o message.o mortal.o movable.o pather.o player.o rect2d.o render.o rng.o text.o text_handler.o timeframe.o vec2d.o viewport.o weapon.o xmlnode.o xmlparse.o
#OBJS:= engine.o render.o movable.o xmlparse.o xmlnode.o vec2d.o viewport.o image.o timeframe.o chunk.o map.o message.o image_handler.o background.o rect2d.o debug_hittable.o drawable.o hittable.o hitbox.o hitline.o hybrid_box.o map_handler.o debug_follower.o entity_handler.o text_handler.o text.o heatable.o debug_heatable.o weapon.o killable.o debug_killable.o mortal.o
WOBJS:= $(addprefix $(WBDIR)/,$(OBJS))
DOBJS:= $(addprefix $(DBDIR)/,$(OBJS))
OBJS:= $(addprefix $(BDIR)/,$(OBJS))

SRCS = engine.cpp render.cpp movable.cpp xmlparse.cpp xmlnode.cpp vec2d.cpp viewport.cpp image.cpp timeframe.cpp chunk.cpp map.cpp message.cpp image_handler.cpp background.cpp rect2d.cpp debug_hittable.cpp drawable.cpp hitbox.cpp hitline.cpp hud.cpp hybrid_box.cpp map_handler.cpp debug_follower.cpp entity_handler.cpp text_handler.cpp text.cpp weapon.cpp debug_killable.cpp mortal.cpp debug_player.cpp player.cpp gunner.cpp rng.cpp pather.cpp engine_nc.cpp rect2d_nc.cpp render_nc.cpp background_nc.cpp chunk_nc.cpp debug_follower_nc.cpp debug_hittable_nc.cpp debug_killable_nc.cpp debug_player_nc.cpp hud_nc.cpp weapon_nc.cpp viewport_nc.cpp render_nc.cpp image_handler_nc.cpp

PATHS = . entity_handler environment/background environment/chunk environment/map hud image movers movers/drawable movers/drawable/mortal movers/drawable/mortal/debug_entities rect2d rect2d/hitbox renders text timeframe utils vec2d viewport xml_parser
VPATH = $(addprefix src/,$(PATHS))


#for all .o files: use the .c files and the deps
#compile them with
#				-c 			for an obeject file
#				-o $@ 	for giving it a name of the object file before the :
#				$< 			for the first prepreq to make this file, i think a .h
$(BDIR)/%.o: %.cpp %.h
	@mkdir -p $(BDIR)
	@printf "building %s\n" $@
	@$(CC) $(CFLAGS) $(OFLAGS) $(MFLAGS) -c -o $@ $<

$(WBDIR)/%.o: %.cpp %.h
	@mkdir -p $(BDIR)
	@mkdir -p $(WBDIR)
	@printf "building winobj %s\n" $@
	@$(WC) $(CFLAGS) $(OFLAGS) $(MFLAGS) $(WFLAGS) -c -o $@ $<

$(DBDIR)/%.o: %.cpp %.h
	@mkdir -p $(BDIR)
	@mkdir -p $(DBDIR)
	@printf "building debug obj %s\n" $@
	@$(CC) $(CFLAGS) $(DFLAGS) $(MFLAGS) -c -o $@ $<

run: $(OBJS) main_driver.cpp
	@mkdir -p $(XDIR)
	@printf "final compilation... "
	@$(CC) $(CFLAGS) $(OFLAGS) -o $(XDIR)/$@ $^ $(LFLAGS) $(SPECIAL_LFLAGS)
	@printf "compiled\ndone\n"

win: $(WOBJS) main_driver_w.cpp
	@mkdir -p $(WDDIR)
	@printf "final win compilation... "
	@$(WC) $(CFLAGS) $(OFLAGS) $^ $(WFLAGS) $(LFLAGS) $(SPECIAL_LFLAGS) -o $(WDDIR)/$(WEXE)
	@printf "compiled\nadding resources... "
	@cp -r resources/ $(WDDIR)/
	@cp win_dll/*.dll $(WDDIR)/
	@printf "added\nzipping... "
	@zip -q -r win_demo.zip $(WDDIR)
	#@paplay /usr/share/sounds/ubuntu/notifications/Positive.ogg
	@printf "zipped\ndone\n"

wintest:
	wine $(WDDIR)/$(WEXE)

winzip:
	zip -r win_demo.zip $(WDDIR)

dir:
	@printf "%s\n" $(DIR)

debug: $(DOBJS) main_driver.cpp
	@mkdir -p $(BDIR);
	@mkdir -p $(DBDIR);
	@mkdir -p $(DDIR);
	@printf "final compilation... "
	@$(CC) $(CFLAGS) $(DFLAGS) -o $(DDIR)/$@ $^ $(LFLAGS);
	@printf "compiled\ndone\n"
	#@paplay /usr/share/sounds/ubuntu/notifications/Positive.ogg

dtest:
	gdb ./debugging/debug

clean:
	@$(RM) *.o *.gch run $(DDIR)/debug $(XDIR)/run $(BDIR)/*.o $(DBDIR)/*.o

winclean:
	@$(RM) -r win_demo/ win_demo.zip $(WBDIR)/*.o

mem:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=$(DDIR)/.v.out $(DDIR)/debug && \
	cat $(DDIR)/.v.out | awk '/HEAP SUMMARY/{p=1}p' > $(DDIR)/.v2.out && \
	sed 's/==.*== //' $(DDIR)/".v2.out" > $(DDIR)/"full-valgrind-out.txt" && \
	cat $(DDIR)/full-valgrind-out.txt > $(DDIR)/valgrind-out.txt && \
	perl -i -ne 'BEGIN{$$/=""} print unless (/SDL_.*Init/ or /X11_ShowCursor/ or  /dlopen\@\@GLIBC_2.2.5/ or /XSetLocaleModifiers/ or /_dl_catch_exception/ or /_XlcCurrentLC/ or /libpulsecommon/ or /SDL_CreateWindow_REAL/ or /lib\/x86_64-linux-gnu\/dri\/i965_dri.so/ or /TTF_Init/ or /_dl_init/ or /_dbus_strdup/ or /vgpreload/ or /wrefresh/ or /waddch/)' $(DDIR)/valgrind-out.txt;
	@rm $(DDIR)/.v.out $(DDIR)/.v2.out; less $(DDIR)/valgrind-out.txt;

#a little tool for xml modification
xml_tool:
	g++ $(CFLAGS) $(DFLAGS) -o tools/xml_mod_tool src/xml_parser/main.cpp src/xml_parser/xmlparse.cpp src/xml_parser/xmlparse.h src/utils/message.h src/utils/message.cpp src/xml_parser/xmlnode.h src/xml_parser/xmlnode.cpp -lstdc++fs 

#win2: $(OBJS) main_driver_w.cpp
#	@mkdir -p $(WDDIR)
#	$(WC) $(CFLAGS) $^ $(WFLAGS) $(LFLAGS) -o $(WDDIR)/$(WEXE)
#	@cp -r resources/ $(WDDIR)/
#	@cp win_dll/*.dll $(WDDIR)/
#	zip -q -r win_demo.zip $(WDDIR)
#	@paplay /usr/share/sounds/ubuntu/notifications/Positive.ogg

#ffmpeg -ss 4.3 -t 6 -i sample3.mp4 -vf "fps=30,scale=1280:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 out5.gif
#use this to make gifs? simplescreenrecorder > ffmpeg > gifcompress.com
