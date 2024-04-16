#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>


using namespace std;

int main(int argc, char *argv[]){

    if (argc == 1){
        printf("Вы забыли ввести аргументы\n");
        exit(-1);
    }
    
    printf("Работает дочерний поток\n");
    for (int i = 0; i < argc; i++){
        printf("%s\n", argv[i]);
        sleep(1);
    }

    printf("child: %d\n", getpid());
    printf("parent: %d\n", getppid());

    return 5;
}