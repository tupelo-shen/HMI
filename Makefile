SIMULATOR 	= simulator
SRC_DIR 	= src
SRC_OBJS 	= src/Common.o\
			src/HmiEvent.o\
			src/HmiMain.o\
			src/Simulator.o\
			src/GDC_Sim.o

GPP 		= g++
CMNINC 		= -I inc/ -I src/
GCCFLAGS 	= -g -Wall `pkg-config --cflags --libs gtk+-3.0`
GPPFLAGS	:= -g -Wall -std=c++11 `pkg-config --cflags gtk+-3.0` $(CMNINC)
LDFLAGS		:= `pkg-config --libs gtk+-3.0`


all: sim
	@bash --norc -c "date '+%Y%m%d %H:%M %S'"
	@echo finished of making Extutable file.

sim: $(SIMULATOR) Makefile-g++.dep
	@echo "$(SIMULATOR) build finished."

Makefile-g++.dep: src/*.cpp Makefile
	@g++  $(CMNINC) src/*.cpp    | sed -e 's/^\([^ ]\)/src\/\1/' > $@

$(SIMULATOR): main.cpp src/*.h $(SRC_OBJS)
	@echo "testing..."
	$(GPP) -o $(SIMULATOR) main.cpp $(SRC_OBJS) $(GPPFLAGS) $(LDFLAGS)

src/%.o : src/%.c
	$(GPP) -c $(GPPFLAGS) -o $@ $< $(LDFLAGS)

depend: Makefile-g++.dep
	ls -l *.dep

clean:
	rm -f *.o $(SIMULATOR) $(SRC_DIR)/*.o
