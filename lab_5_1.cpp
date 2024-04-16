#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <fstream>

using namespace std;

int main(){

    sem_t *sem = sem_open("/named_semaphore", O_CREAT, 0644, 1);
    bool is_press = false;
    ofstream File;
    File.open("text.txt", ios::app);
    
    if (!File.is_open()) {
        perror("Не удалось открыть файл\n");
        sem_close(sem);
        sem_unlink("/named_semaphore");
        return -1;
    }

    sem_post(sem);

    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    while(!is_press){
        sem_wait(sem);
        for (int i = 0; i < 10; i++){
            File << "1" << flush;
            cout << "1" << flush;
            sleep(1);
        }
        sem_post(sem);
        sleep(1);

        int ch = getchar();
        if (ch == '\n'){
            is_press = true;
        }
    }
    
    File.close();
    printf("Файл закрыт 1\n");
    sem_close(sem);
    sem_unlink("/named_semaphore");
    printf("Семафор удалён 1\n");

    return 0;
}
