NC_DIR:= ./ 
SRC_DIR:= ./ 
SRCS:=$(wildcard *.cpp) $(wildcard *.cc) $(wildcard *.c)
OBJS:= $(patsubst %.cpp,  %.o, $(SRCS))

CXX:=g++	
CXXFLAGS:= -w -g -Wall -std=c++11  $(addprefix -I, $(INC_DIR))
LIBS := -lpthread

EXE:=./service.exe

$(EXE):$(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS) $(LIBS)

clean:
	rm -rf $(EXE)
	rm -rf $(OBJS)
