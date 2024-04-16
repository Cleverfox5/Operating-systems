#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <signal.h>


using namespace std;

void sigsys_handler(int signum, siginfo_t *info, void *context)
{
    printf("Фильтр убил дочерний процесс, и послал SIGSYS\n");
    printf("(поэтому вы выидите данное сообщение)\n");
    exit(-1);
}

int main(){
    struct sigaction act;
    act.sa_sigaction = sigsys_handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGSYS, &act, NULL);
    
    printf("Работает дочерний поток\n");

    struct sysinfo info;
    if (sysinfo(&info) == -1) {
        perror("Error with sysinfo");
        return -1;
    }
    unsigned long long totalMem = info.totalram;
    totalMem *= info.mem_unit;
    cout << "Дочерний процесс: Доступная память: " << totalMem << endl;

    return 5;
}