#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

using namespace std;

void * proc1(void * arg){
    cout << "Поток 1 начал работу\n";
    bool *args = (bool*) arg;
    while(!(*args)){
        for (int i =0; i < 10; i++){
            putchar('1');
            fflush(stdout);
            sleep(1);
        }
        sleep(1);
    }
    pthread_exit((void*)1);
}

void * proc2(void * arg){
    cout << "Поток 2 начал работу\n";
    bool *args = (bool*) arg;
    while(!(*args)){
        for (int i =0; i < 10; i++){
            putchar('2');
            fflush(stdout);
            sleep(1);
        }
        sleep(1);
    }
    pthread_exit((void*)2);
}

int main(){
    setlocale(0, "");
    cout << "Программа начала работу\n";
    pthread_t id1, id2;
    bool is_end1 = false, is_end2 = false;

    pthread_create(&id1, NULL, proc1, &is_end1);
    pthread_create(&id2, NULL, proc2, &is_end2);
    
    cout << "\nПрограмма ждет нажатия клавиши\n";
    getchar();
    cout << "Клавиша нажата\n";

    is_end1 = true;
    is_end2 = true;
    
    pthread_join(id1, NULL);
    cout << "\nПоток 1 закончил работу\n";

    pthread_join(id2, NULL);
    cout << "Поток 2 закончил работу\n";

    cout << "Программа закончила работу\n";
    return 0;
}