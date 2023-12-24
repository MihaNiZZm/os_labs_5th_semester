# Task 2-3. Synchronization of multiple reader- and writer-threads.  

## Инструкция по сборке и запуску программ.  
1. Скомпилируйте программу, используя команду: gcc main.c storage.c utils.c -D TYPE -o main.out  
TYPE может быть двух видов: SPIN, MUTEX. Если параметр TYPE не указан явно, то в качестве примитива синхронизации будет выбран rwlock.
2. Запустите программу, используя команду: ./main.out -s SIZE -n NUM_OF_SWAPPERS -t TIME_IN_SECONDS  
SIZE - количество элементов в хранилище  
NUM_OF_SWAPPERS - количество потоков-писателей  
TIME_IN_SECONDS - время работы программы в секундах