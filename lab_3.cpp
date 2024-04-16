#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <cstdio>
#include <fcntl.h>


using namespace std;
int filedes[2];
bool is_end1 = false, is_end2 = false;

void sig_handler(int signo)
{
    printf("\nget SIGINT; %d\n", signo);

    if (signo == SIGINT) {
        is_end1 = true;
        is_end2 = true;
    }

    close(filedes[0]);
    close(filedes[1]);

    printf("Неименованный канал закрыт\n");

    printf("Программа закончила работу\n");
    exit(0);
}

void * proc1(void * arg){
    printf("Поток 1 начал работу\n");
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

        int nw = write(filedes[1], buf, strlen(buf));

        if (nw == -1){
            perror("Ошибка записи");
			sleep(1);
        }
        else{
            sleep(1);
        }
    }
    return NULL;
}

void * proc2(void * arg){
    printf("Поток 2 начал работу\n");
    bool *args = (bool*) arg;
    char buf1[1024];
    char *buf = buf1;
    while(!(*args)){
        memset(buf, 0, sizeof(buf));
        
        ssize_t nr = read(filedes[0], buf, sizeof(buf));
        if (nr == -1){
            perror("Ошибка чтения");
            sleep(1);
        }
        else if (nr == 0){
            printf("Конец файла\n");
            sleep(1);
        }
        else{
            printf("%s\n", buf);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]){
    setlocale(0, "");

    signal(SIGINT, sig_handler);

    if (argc != 2){
        printf("Добавте аргумент 1 или 2 или 3\n");
        exit(-1);
    }
    
    printf("Программа начала работу\n");
    pthread_t id1, id2;
    int variation = atoi(argv[1]);
    int flags; 

    switch(variation){
        case 1:
            pipe(filedes);
            break;
        case 2:
            pipe2(filedes, O_NONBLOCK);
            break;
        case 3:
            pipe(filedes);
            fcntl(filedes[0], F_SETFL, O_NONBLOCK);
            fcntl(filedes[1], F_SETFL, O_NONBLOCK);
            break;
        default:
            printf("Число не из диапозона от 1 до 3\n");
            exit(-1);
    }

    printf("Неименованный канал создан\n");

    pthread_create(&id1, NULL, proc1, &is_end1);
    pthread_create(&id2, NULL, proc2, &is_end2);
    
    printf("\nПрограмма ждет нажатия клавиши\n");
    getchar();
    printf("Клавиша нажата\n");

    is_end1 = true;
    is_end2 = true;
    
    pthread_join(id1, NULL);
    printf("\nПоток 1 закончил работу\n");

    pthread_join(id2, NULL);
    printf("Поток 2 закончил работу\n");

    close(filedes[0]);
    close(filedes[1]);

    printf("Неименованный канал закрыт\n");

    printf("Программа закончила работу\n");

    return 0;
}