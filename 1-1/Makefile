all: refresh_files compile_sources distribute_compiled_files

refresh_files:
	rm -rf ./exe/
	mkdir ./exe/

compile_sources:
	gcc -g -Wall -I"./src/" src/a.c -lpthread -o a.out
	gcc -g -Wall -I"./src/" src/b.c -lpthread -o b.out
	gcc -g -Wall -I"./src/" src/c.c -lpthread -o c.out
	gcc -g -Wall -I"./src/" src/d.c -lpthread -o d.out
	gcc -g -Wall -I"./src/" src/e.c -lpthread -o e.out

distribute_compiled_files:
	mv a.out ./exe/
	mv b.out ./exe/
	mv c.out ./exe/
	mv d.out ./exe/
	mv e.out ./exe/