SIMULATOR = simulator
SRC_DIR = src
SRC_OBJS = src/Common.o\
			src/HmiEvent.o\
			src/HmiMain.o 

CMNINC	= -I inc/ -I src/ -lpthread
GPPFLAGS = -g -Wall -std=gnu++11 $(CMNINC)
GPP = g++


all: sim
	@bash --norc -c "date '+%Y%m%d %H:%M %S'"
	@echo finished of making Extutable file.

sim: $(SIMULATOR) Makefile-g++.dep
	@echo "$(SIMULATOR) build finished."

Makefile-g++.dep: src/*.cpp Makefile
	@g++  $(CMNINC) src/*.cpp    | sed -e 's/^\([^ ]\)/src\/\1/' > $@

$(SIMULATOR): main.cpp src/*.h $(SRC_OBJS)
	@echo "testing..."
	$(GPP) -o $(SIMULATOR) main.cpp $(SRC_OBJS) $(GPPFLAGS) $(LIBS)

src/%.o : src/%.c
	$(GPP) -c           $(GPPFLAGS) -o $@ $<

depend: Makefile-g++.dep
	ls -l *.dep

clean:
	rm -f *.o $(SIMULATOR) $(SRC_DIR)/*.o  
