all: refresh_files compile_sources distribute_compiled_files

refresh_files:
	rm -rf ./exe/
	mkdir ./exe/

compile_sources:
	gcc -g -Wall -I"./src/" src/mythread_create.c src/main.c -lpthread -o mythread_test.out

distribute_compiled_files:
	mv mythread_test.out ./exe/