CPP = g++ 
CPPFLAGS = -g --std=c++11
#ARLIB = ar -t $^
COMPILEBIN = $(CPP) -o $@ $^ $(LDFLAGS)

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)

all : main

.PHONY: all clean

%.d : %.cpp
	$(CPP) -MM %.cpp

%.o : %.cc %.d 
	$(CPP) $(CPPFLAGS) -o $@ -c $<

main : $(OBJS)
	$(COMPILEBIN)

clean :
	rm *.o main
