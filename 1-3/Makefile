all: refresh_files compile_sources distribute_compiled_files

refresh_files:
	rm -rf ./exe/
	mkdir ./exe/

compile_sources:
	gcc -g -Wall -I"./src/" src/a.c -lpthread -o a.out
	gcc -g -Wall -I"./src/" src/b.c -lpthread -o b.out

distribute_compiled_files:
	mv a.out ./exe/
	mv b.out ./exe/