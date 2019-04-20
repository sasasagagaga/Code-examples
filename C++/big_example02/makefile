CC = g++
CFLAGS += -O2 -std=c++14
CALL = $(CC) $(CFLAGS) -c $<
MAIN = $(CC) $(CFLAGS) $^ -o $@

all : main
	@echo main has been compiled

main : main.o tester.o tests.o ODE_solvers.o types.o
	$(MAIN)

main.o : main.cpp tester.h tests.h ODE_solvers.h types.h
	$(CALL)

tester.o : tester.cpp tester.h
	$(CALL)

tests.o : tests.cpp tests.h tester.h types.h
	$(CALL)

ODE_solvers.o : ODE_solvers.cpp ODE_solvers.h types.h
	$(CALL)

types.o : types.cpp types.h
	$(CALL)

clean :
	rm -f main *.o
