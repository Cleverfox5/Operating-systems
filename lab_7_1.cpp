#include <iostream>
#include <pthread.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <pwd.h>

using namespace std;

bool is_end = false, is_press = false;
mqd_t mq;
int result;

void * proc(void * arg){
    bool *args = (bool*) arg;
    char buf1[1024];
    char *buf = buf1;
    while(!(*args)){
        char *user_name = getlogin();
        struct passwd *str_user_name = getpwnam(user_name);

        if (str_user_name == NULL){
            printf("Пользователь %s не найден", user_name);
            exit(-1);
        }
        
        strcpy(buf, str_user_name->pw_dir);

        result = mq_send(mq, buf, strlen(buf) + 1, 0);
        if (result == -1){
            perror("mq_send");
        }
        else{
            printf("Программа 1 записала: %s\n", buf);
        }

        sleep(1);
    }

    return NULL;
}

int main(){
    setlocale(0, "");
    
    struct mq_attr attr;
    attr.mq_maxmsg = 20;
    attr.mq_msgsize = 1024;
    
    pthread_t id;
    mq = mq_open("/myqueue", O_CREAT|O_RDWR|O_NONBLOCK, 0644, &attr);
    
    if (mq == (mqd_t)-1) {
        perror("Ошибка при открытии 1");
        exit(0);
    }

    pthread_create(&id, NULL, proc, &is_press);

    printf("\nПрограмма ждет нажатия клавиши\n");
    getchar();
    printf("Клавиша нажата\n");

    is_press = true;

    pthread_join(id, NULL);
    printf("\nПоток закончил работу 1\n");

    if (mq_close(mq) == 0){
        printf("Очередь закрылась 1\n");
    }
    else{
        perror("Не удалось закрыть очередь 1");
    }


    if (mq_unlink("/myqueue") == 0){
        printf("Очередь была закрыта\n");
    }
    else{
        perror("Не удалось удалить очередь 1");
    }

    printf("Программа закончила работу\n");

    return 0;
}