CC      = cc
CXX     = c++

CFLAGS  = -g -O3 -Wall
LIBS    = 
LDFLAGS = -g

OBJECTS = Connection.o StopWatch.o Tester.o UARTController.o UDPSocket.o

all:		Tester

Tester:		$(OBJECTS)
		$(CXX) $(OBJECTS) $(CFLAGS) $(LIBS) -o Tester

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

clean:
		$(RM) Tester *.o *.d *.bak *~

