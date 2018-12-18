SIMULATOR 	= test
DIR 		= test/
OBJS 		= test_GdkPixbuf.o

GPP 		= g++
CMNINC 		= -I inc/ -I src/ -I $(BOOST_INCLUDE)
GCCFLAGS 	= -g -Wall `pkg-config --cflags --libs gtk+-3.0`
GPPFLAGS	:= -g -Wall -std=c++11 `pkg-config --cflags gtk+-3.0` $(CMNINC)
LDFLAGS		:= `pkg-config --libs gtk+-3.0`
LIBS 		+= -L $(BOOST_LIB)\
				-lboost_regex \
				-lboost_system \
				-lboost_filesystem \
				-lboost_chrono

all: $(SIMULATOR) $(OBJS)
	@bash --norc -c "date '+%Y%m%d %H:%M %S'"
	@echo finished of making Extutable file.

$(SIMULATOR): $(OBJS)
	@echo "testing..."
	$(GPP) -o $@ $(OBJS) $(GPPFLAGS) $(LIBS) $(LDFLAGS) $(LIBS)

src/%.o : src/%.c
	$(GPP) -c $(GPPFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(SIMULATOR)
