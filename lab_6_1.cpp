#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <pwd.h>
#include <string.h>
#include <csignal>


using namespace std;

bool is_end = false, is_press = false;
sem_t *w_sem; 
sem_t *r_sem;
key_t mykey;
int mem_id;
char * ptr;

void sig_handler(int signo)
{
    printf("\nget SIGINT; %d\n", signo);

    if (signo == SIGINT) {
        is_end = true;
        is_press = true;
    }
    if (shmdt(ptr) == -1) {
        printf("Не удалось отсоединить сегмент общей памяти 1\n");
    }
    else{
        printf("Сегмент паямти успешно отсоединён 1\n");
    }
    if (shmctl(mem_id, IPC_RMID, NULL) == -1){
        printf("Не удалось удалить сегмент общей памяти 1\n");
    }
    else{
        printf("Сегмент паямти успешно удалён 1\n");
    }
    sem_close(w_sem);
    sem_close(r_sem);
    if (sem_unlink("/write_semaphore") == 0){
        printf("Семафор записи удалён 1\n");
    }
    else{
        printf("Не удалось удалить семафор записи 1\n");
    }
    if (sem_unlink("/read_semaphore") == 0){
        printf("Семафор чтения удалён 1\n");
    }
    else{
        printf("Не удалось удалить семафор чтения 1\n");
    }

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

        
        memcpy(ptr, buf, strlen(buf) + 1);
        printf("Программа 1 записала: %s\n", buf);

        sem_post(w_sem);
        sem_wait(r_sem);
        sleep(1);
    }
    return NULL;
}

int main(){
    setlocale(0, "");
    
    signal(SIGINT, sig_handler);

    w_sem = sem_open("/write_semaphore", O_CREAT, 0644, 0);
    r_sem = sem_open("/read_semaphore", O_CREAT, 0644, 0);
    mykey = ftok("/my_shered_memory", 5);
    mem_id = shmget(mykey,4096,IPC_CREAT|0666);
    ptr = (char*) shmat(mem_id, (void *) 0, 0);

    pthread_t id1;

    pthread_create(&id1, NULL, proc1, &is_press);

    printf("\nПрограмма ждет нажатия клавиши\n");
    getchar();
    printf("Клавиша нажата\n");

    is_press = true;

    pthread_join(id1, NULL);
    printf("\nПоток 1 закончил работу\n");

    if (shmdt(ptr) == -1) {
        printf("Не удалось отсоединить сегмент общей памяти 1\n");
    }
    else{
        printf("Сегмент паямти успешно отсоединён 1\n");
    }

    if (shmctl(mem_id, IPC_RMID, NULL) == -1){
        printf("Не удалось удалить сегмент общей памяти 1\n");
    }
    else{
        printf("Сегмент паямти успешно удалён 1\n");
    }

    sem_close(w_sem);
    sem_close(r_sem);
    if (sem_unlink("/write_semaphore") == 0){
        printf("Семафор записи удалён 1\n");
    }
    else{
        printf("Не удалось удалить семафор записи 1\n");
    }
    if (sem_unlink("/read_semaphore") == 0){
        printf("Семафор чтения удалён 1\n");
    }
    else{
        printf("Не удалось удалить семафор чтения 1\n");
    }

    return 0;
}