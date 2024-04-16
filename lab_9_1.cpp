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
#include <sys/sysinfo.h>
#include <seccomp.h>
#include <signal.h>


using namespace std;

scmp_filter_ctx ctx;

void sigsys_handler(int signum, siginfo_t *info, void *context)
{
    printf("Фильтр убил дочерний процесс, и послал SIGSYS\n");
    printf("(поэтому вы выидите данное сообщение)\n");
    seccomp_release(ctx);
    printf("Фильтр удалён\n");
    exit(-1);
}

int main(int argc, char *argv[]){
    struct sigaction act;
    act.sa_sigaction = sigsys_handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGSYS, &act, NULL);
    if (argc == 1){
        printf("Вы забыли ввести аргументы\n");
        printf("1 - Уничтожение процесса, вызвавшего системную функцию\n");
        printf("2 - Возвращение кода ошибки вместо результата\n");
        printf("3 - Получение и обработка сигнала SIGSYS\n");
        printf("4 - Запись события вызова функции в журнал\n");
        exit(-1);
    }
    printf("Работает родительский поток\n");

    pid_t	pid	=	fork();
    int wstatus;

    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
        perror("Error with seccomp_init");
        return -1;
    }

    switch(stoi(argv[1])){
        case 1:
            if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(sysinfo), 0) == -1) {
                perror("Error with seccomp_rule_add with sysinfo");
                seccomp_release(ctx);
                return -1;
            }
            break;
        case 2:
            if (seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(sysinfo), 0) == -1) {
                perror("Error with seccomp_rule_add with sysinfo");
                seccomp_release(ctx);
                return -1;
            }
            break;
        case 3:
            if (seccomp_rule_add(ctx, SCMP_ACT_TRAP, SCMP_SYS(sysinfo), 0) == -1) {
                perror("Error with seccomp_rule_add with sysinfo");
                seccomp_release(ctx);
                return -1;
            }
            break;
        case 4:
            if (seccomp_rule_add(ctx, SCMP_ACT_LOG, SCMP_SYS(sysinfo), 0) == -1) {
                perror("Error with seccomp_rule_add with sysinfo");
                seccomp_release(ctx);
                return -1;
            } 
            break;
        default:
            printf("Вы ввели слишком большое значение\n");
            printf("1 - Уничтожение процесса, вызвавшего системную функцию\n");
            printf("2 - Возвращение кода ошибки вместо результата\n");
            printf("3 - Получение и обработка сигнала SIGSYS\n");
            printf("4 - Запись события вызова функции в журнал\n");
            exit(-1);
            break;
    }

    int load = seccomp_load(ctx);
    if (load != 0){
        perror("Error with seccomp_load");
        seccomp_release(ctx);
        printf("Фильтр удалён\n");
        return -1;
    }
    
    if (pid == -1) {
        perror("fork");
    }else if (pid == 0) {
        execl("lab_9_2", "lab_9_2", NULL);
    }
    else{
        while (waitpid(pid, &wstatus, WNOHANG) == 0) {
            printf("Ждёт\n");
            usleep(500000);
        }
        int exit_status = WEXITSTATUS(wstatus);
        
        printf("exit cod: %d\n", exit_status);

        seccomp_release(ctx);
        printf("Фильтр удалён\n");
    };
}