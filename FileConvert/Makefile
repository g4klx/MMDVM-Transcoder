CC      = cc
CXX     = c++

CFLAGS  = -g -O3 -Wall
LIBS    = -lsndfile
LDFLAGS = -g

OBJECTS = Connecton.o DVFileReader.o DVFileWriter.o FileConvert.o StopWatch.o Thread.o UARTController.o UDPSoocket.o WAVFileReader.o WAVFileWriter.o

all:		FileConvert

FileConvert:	$(OBJECTS)
		$(CXX) $(OBJECTS) $(CFLAGS) $(LIBS) -o FileConvert

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

install:
		install -m 755 FileConvert /usr/local/bin/

clean:
		$(RM) FileConvert *.o *.d *.bak *~

