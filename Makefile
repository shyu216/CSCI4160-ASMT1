INC = -I/usr/local/include -I/usr/share/CUnit/include
LIB = -L/usr/local/lib -L/usr/share/CUnit/lib
SIMD = -msse -mmmx -msse2

all: runtest.c util.c asgn1a.c WjCryptLib/WjCryptLib_Rc4.c
	gcc $^ -o runtest $(INC) $(LIB) $(SIMD) -lcunit -pthread -fopenmp -std=c99 -O3 
clean:
	rm -rf runtest

pp: runtest.c util.c pp.c WjCryptLib/WjCryptLib_Rc4.c
	gcc $^ -o runcmp $(INC) $(LIB) $(SIMD) -lcunit -pthread -fopenmp -std=c99 -O3

