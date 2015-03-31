VERSION=1.0.0

LIBRARY = libRFtransmitter.so
STATIC_LIBRARY = libRFtransmitter.a
TEST_NAME = runTests

SOURCES = src/snmpconnector.cpp \
	  src/rfcomponent.cpp \
	  src/outstage.cpp \
	  src/mtx.cpp \
	  src/transmitter.cpp \
	  src/liquidcooling.cpp \
	  src/rfsensor.cpp \
	  src/amplifiers.cpp
	
OBJS = $(SOURCES:.cpp=.o)

FLAGS = -O3 -Wall -fPIC -std=c++0x -DSTDCXX_98_HEADERS -DHAVE_NAMESPACE_STD -I./include -lsnmp++
COMPILER = g++

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(COMPILER) $(FLAGS) -shared -o $(LIBRARY) $(OBJS)

%.o:	%.cpp
	$(COMPILER) -c $(FLAGS) -o $*.o $*.cpp   

static : $(OBJS) 	
	ar -rs $(STATIC_LIBRARY) $(OBJS)

clean:
	rm -f $(LIBRARY) $(STATIC_LIBRARY) $(OBJS) $(TEST_NAME)

test:	static
	$(COMPILER) $(FLAGS) -o $(TEST_NAME) test/gTestMe.cpp -L./ -Wl,-Bstatic -lRFtransmitter -Wl,-Bdynamic -lgtest
