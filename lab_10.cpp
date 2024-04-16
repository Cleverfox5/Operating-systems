#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

using namespace std;

bool is_press = false;
const struct inotify_event *event;
int fd;

void * proc(void * args){
    bool * arg = (bool *) args;
    char buf[4096];
    while(!(* arg)){
        memset(buf,0,sizeof(buf));
        int len = read(fd, buf, sizeof(buf));
        if (len == -1){
            if (errno != EAGAIN){
                perror("read");
                break;
            }
            else{
                sleep(1);
            }
        }
        for (char *ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event *) ptr;
            cout << endl;
            cout << "name: " << event->name << endl;
            cout << "wd: " << event->wd << endl;
            cout << "mask: " << event->mask << endl;
            cout << "cookie: " << event->cookie << endl;
            cout << "len: " << event->len << endl;
        }
    }

    pthread_exit((void*)0);
}

int main(){
    setlocale(0, "");
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    fd = inotify_init1(IN_NONBLOCK);
    int wd = inotify_add_watch(fd, ".", IN_OPEN | IN_CLOSE);

    pthread_t id;
    pthread_create(&id, NULL, proc, &is_press); 

    printf("Введите Enter\n");
    while(true){
        if (getchar() == 10){
            break;
        }
    }
    printf("Клавиша была нажата\n");

    is_press = true;

    int *exitcode;

    pthread_join(id, (void **)&exitcode);
    printf("Поток завершён\n");
    if (fcntl(STDIN_FILENO, F_SETFL, flags) == -1)
        perror("Не удалось возвратить флаги в исходное положение");
    else
        printf("Флаги возвращены в исходное положение\n");
    
    if (inotify_rm_watch(fd, wd) == -1)
        perror("");
    else
        printf("Наблюдатель удалён\n");
    
    if (close(fd) == -1)
        perror("Не удалось закрыть экземпляр inotify");
    else
        printf("Экземпляр inotify закрыт\n");
    
    return 0;
}