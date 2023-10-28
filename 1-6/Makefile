all: refresh_files compile_sources distribute_compiled_files

refresh_files:
	rm -rf ./exe/
	mkdir ./exe/

compile_sources:
	gcc -g -Wall -I"./src/" src/my_thread_create.c -lpthread -o my_thread_create.out

distribute_compiled_files:
	mv my_thread_create.out ./exe/