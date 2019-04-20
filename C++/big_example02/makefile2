
CC = g++
CFLAGS += -O2 -std=c++14
BOOST_FLAGS = -lboost_system -lboost_filesystem
CALL = $(CC) $(CFLAGS) -c $<
MAIN = $(CC) $(CFLAGS) $^ -o $@ $(BOOST_FLAGS)

all : main
	@echo main has been compiled

main : main.o matrix.o gaussian_method.o tester.o tests.o matrix_functions.o SLE_solvers.o
	$(MAIN)

main.o : main.cpp matrix.h gaussian_method.h tester.h tests.h matrix_functions.h SLE_solvers.h
	$(CALL)

matrix.o : matrix.cpp matrix.h
	$(CALL)

gaussian_method.o : gaussian_method.cpp gaussian_method.h matrix.h
	$(CALL)

tester.o : tester.cpp tester.h
	$(CALL)

tests.o : tests.cpp tests.h matrix.h tester.h matrix_functions.h
	$(CALL)

matrix_functions.o : matrix_functions.cpp matrix_functions.h matrix.h gaussian_method.h
	$(CALL)

SLE_solvers.o : SLE_solvers.cpp SLE_solvers.h matrix.h gaussian_method.h matrix_functions.h tester.h tests.h
	$(CALL)

clean :
	rm -f main *.o
