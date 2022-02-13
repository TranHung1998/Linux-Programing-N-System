
static:
	mkdir -p lib/static/
	ar rcs lib/static/libhello.a obj/static/helloHung.o obj/static/helloLinux.o

link_static:
	mkdir -p bin/statically-linked/
	gcc obj/main.o -Llib/static -lhello -o bin/statically-linked/exam

shared:
	mkdir -p lib/shared/
	gcc -shared obj/shared/helloHung.o obj/shared/helloLinux.o -o lib/shared/libhello.so

link_shared:
	mkdir -p bin/use-shared-library/
	gcc  obj/main.o -Llib/shared -lhello -o bin/use-shared-library/exam
	
all:
	mkdir -p obj/static obj/shared  
	gcc -c main.c -I./inc -o obj/main.o
	gcc -c src/helloHung.c -I./inc -o obj/static/helloHung.o
	gcc -c src/helloLinux.c -I./inc -o obj/static/helloLinux.o
	gcc -c -fPIC src/helloHung.c -I./inc -o obj/shared/helloHung.o
	gcc -c -fPIC src/helloLinux.c -I./inc -o obj/shared/helloLinux.o

clean:
	rm -rf bin/* obj/*
	
.PHONY: static link_static shared link_shared all clean

