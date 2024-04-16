#include <iostream>
#include <pthread.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <pwd.h>

using namespace std;

bool is_end = false, is_press = false;
mqd_t mq;
struct mq_attr attr;

void * proc(void * arg){
    bool *args = (bool*) arg;
    unsigned int priority;
    ssize_t is_here;
    mq_getattr(mq, &attr);
    while(!(*args)){
        char buffer[attr.mq_msgsize];
        memset(buffer, 0, sizeof(buffer));
        is_here = mq_receive(mq, buffer, attr.mq_msgsize, &priority);
        if (is_here != (ssize_t) -1){
            printf("Сообещние прочитано: %s\n", buffer);
        }
    }

    return NULL;
}

int main(){
    setlocale(0, "");
    struct mq_attr attr;
    attr.mq_maxmsg = 100;
    attr.mq_msgsize = 1024;
    mq = mq_open("/myqueue", O_CREAT|O_RDWR|O_NONBLOCK, 0644, &attr);

    pthread_t id;
    pthread_create(&id, NULL, proc, &is_press);


    printf("\nПрограмма ждет нажатия клавиши\n");
    getchar();
    printf("Клавиша нажата\n");

    is_press = true;

    pthread_join(id, NULL);
    printf("\nПоток закончил работу 2\n");

    if (mq_close(mq) == 0){
        printf("Очередь закрылась 2\n");
    }
    else{
        perror("Не удалось закрыть очередь 2");
    }


    if (mq_unlink("/myqueue") == 0){
        printf("Очередь была закрыта 2\n");
    }
    else{
        perror("Не удалось удалить очередь 2");
    }

    printf("Программа закончила работу 2\n");

    return 0;
}