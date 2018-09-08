all: shell
shell: main.o parse.o exec.o
	gcc main.o parse.o -o shell
main: main.c
	gcc -c main.c
parse.o: parse.h parse.c
	gcc -c parse.c
exec.o: exec.c exec.h
	gcc -c exec.c
clean:
	rm *.o shell

