CXXFLAGS+=-std=c++17 -g -Wall -Wextra -Wpedantic -D_GNU_SOURCE -Werror=all
#CXX=g++
LDLIBS+=-lpthread
SOURCES = $(wildcard *.cpp */*.cpp)
HEADERS = $(wildcard *.hpp */*.hpp)

OBJ=db.o student.o queries.o 

.PHONY: main
main: Server.o client check run

Server.o: tcpServer.c ${OBJ}
	$(CXX) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)
client:
	g++ -o Client.o tcpClient.c 
%.o: %.cpp %.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^

.PHONY: clean
clean:
	-rm *.o
	-rm *.gch

