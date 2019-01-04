SIMULATOR 	= simulator
SRC_DIR 	= src
SRC_OBJS 	= src/Common.o\
			src/HmiEvent.o\
			src/HmiMain.o\
			src/Simulator.o\
			src/GDC_Common.o\
			src/GDC_Sim.o\
			src/BMPDump.o\
			src/GDC_Driver.o\
			src/HMIControl.o\
			src/DrawCommand.o\
			src/FontInfo.o\
			src/HMI_Resource.o\
			src/HMI_Alloc.o\
			src/sclDrawCom.o\
			src/sclUtil.o\
			src/sclBase.o\
			src/sclCore.o\
			src/sclBoard.o\
			src/sclParts.o\
			src/sclCallback.o\
			src/sclLabel.o

TEST_OBJS	= src/test_Simulator.o

GPP 		= g++
CMNINC 		= -I inc/ -I src/ -I $(BOOST_INCLUDE)
GCCFLAGS 	= -g -Wall `pkg-config --cflags --libs gtk+-3.0`
GPPFLAGS	= -g -Wall -std=c++11 `pkg-config --cflags --libs gtk+-3.0` $(CMNINC)
LDFLAGS		=
LIBS 		+= -L $(BOOST_LIB)\
				-lboost_regex \
				-lboost_system \
				-lboost_filesystem \
				-lboost_chrono

all: sim
	@bash --norc -c "date '+%Y%m%d %H:%M %S'"
	@echo finished of making Extutable file.

sim: $(SIMULATOR) Makefile-g++.dep
	@echo "$(SIMULATOR) build finished."

Makefile-g++.dep: src/*.cpp Makefile
	@g++  $(CMNINC) src/*.cpp    | sed -e 's/^\([^ ]\)/src\/\1/' > $@

$(SIMULATOR): main.cpp src/*.h $(SRC_OBJS) $(TEST_OBJS)
	@echo "making..."
	$(GPP) -o $(SIMULATOR) main.cpp $(SRC_OBJS) $(TEST_OBJS) $(GPPFLAGS) $(LDFLAGS) $(LIBS)

src/%.o : src/%.c
	$(GPP) -c $(GPPFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

depend: Makefile-g++.dep
	ls -l *.dep

clean:
	rm -f *.o $(SIMULATOR) $(SRC_DIR)/*.o
