.PHONY:clean

test:string.c main.c
	gcc -o test  string.c main.c
	./test

testdl:testdl.c libstr.so
	gcc -o testdl testdl.c -ldl
	./testdl

libstr.so:string.c
	gcc -fpic -shared string.c -o libstr.so

clean:
	-rm test testdl 
