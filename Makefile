# Makefile for the Bank program
# FOR C++
CC=g++
CFLAGS= -Wall -Werror -pedantic-errors -DNDEBUG -pthread *.cpp
CXXFLAGS=-std=c++11 -pthread
CCLINK=$(CC)
OBJS=main.o bank.o atm.o account.o
#LIBS=-pthread
RM=rm -f
# Creating the  executable
Bank: $(OBJS)
	$(CCLINK) $(CXXFLAGS) -o Bank $(OBJS)
# Creating the object files
main.o: main.cpp atm.h account.h bank.h
bank.o: bank.cpp bank.h
atm.o: atm.cpp atm.h bank.h
account.o: account.cpp account.h
# Cleaning old files before new make
#clean:
#	rm -f $(PROG) $(OBJS) $(TARGET) *.o *~ "#"* core.*
.PHONY: clean
clean:
	rm -f Bank $(OBJS)
	$(RM) $(TARGET) *.o *~ "#"* core.*