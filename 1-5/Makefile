all: refresh_files compile_sources distribute_compiled_files

refresh_files:
	rm -rf ./exe/
	mkdir ./exe/

compile_sources:
	gcc -g -Wall -I"./src/" src/a.c -lpthread -o a.out

distribute_compiled_files:
	mv a.out ./exe/