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
        printf("Не удалось отсоединить сегмент общей памяти 2\n");
    }
    else{
        printf("Сегмент паямти успешно отсоединён 2\n");
    }

    if (shmctl(mem_id, IPC_RMID, NULL) == -1){
        printf("Не удалось удалить сегмент общей памяти 2\n");
    }
    else{
        printf("Сегмент паямти успешно удалён 2\n");
    }
    sem_close(w_sem);
    sem_close(r_sem);
    if (sem_unlink("/write_semaphore") == 0){
        printf("Семафор записи удалён 2\n");
    }
    else{
        printf("Не удалось удалить семафор записи 2\n");
    }
    if (sem_unlink("/read_semaphore") == 0){
        printf("Семафор чтения удалён 1\n");
    }
    else{
        printf("Не удалось удалить семафор чтения 2\n");
    }
    
    printf("Программа закончила работу\n");
    exit(0);
}

void * proc1(void * arg){
    printf("Поток 2 начал работу\n");
    bool *args = (bool*) arg;
    char buf1[1024];
    char *buf = buf1;
    while(!(*args)){
        sem_wait(w_sem);
        memcpy(buf, ptr, strlen(ptr) + 1);
        printf("Программа 2 считала: %s\n", buf);
        sem_post(r_sem);
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
    printf("\nПоток 2 закончил работу\n");

    if (shmdt(ptr) == -1) {
        perror("Не удалось отсоединить сегмент общей памяти 2");
    }
    else{
        printf("Сегмент паямти успешно отсоединён 2\n");
    }

    if (shmctl(mem_id, IPC_RMID, NULL) == -1){
        perror("Не удалось удалить сегмент общей памяти 2");
    }
    else{
        printf("Сегмент паямти успешно удалён 2\n");
    }
    sem_close(w_sem);
    sem_close(r_sem);
    if (sem_unlink("/write_semaphore") == 0){
        printf("Семафор записи удалён 2\n");
    }
    else{
        printf("Не удалось удалить семафор записи 2\n");
    }
    if (sem_unlink("/read_semaphore") == 0){
        printf("Семафор чтения удалён 2\n");
    }
    else{
        printf("Не удалось удалить семафор чтения 2\n");
    }

    return 0;
}