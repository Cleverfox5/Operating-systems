#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>


using namespace std;

int main(int argc, char * argv[]){

    if (argc == 1){
        printf("Вы забыли ввести аргументы\n");
        exit(-1);
    }

    printf("Работает родительский поток\n");
    for (int i = 0; i < argc; i++){
        printf("%s\n", argv[i]);
    }

    pid_t	pid	=	fork();
    int wstatus;
    
    if (pid == -1) {
        perror("fork");
    }else if (pid == 0) {
        execl("lab_4_1", "first", "second", "third", "fourth", NULL);
    }
    else{
        while (waitpid(pid, &wstatus, WNOHANG) == 0) {
            printf("Ждёт\n");
            usleep(500000);
        }
        int exit_status = WEXITSTATUS(wstatus);
        printf("exit cod: %d\n", exit_status);
    };
}