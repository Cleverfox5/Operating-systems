#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

using namespace std;

void * proc1(void * arg){
    cout << "Поток 1 начал работу\n";
    bool *args = (bool*) arg;
    while(!(*args)){
        putchar('1');
        fflush(stdout);
        sleep(1);
    }
    pthread_exit((void*)1);
}

void * proc2(void * arg){
    cout << "Поток 2 начал работу\n";
    bool *args = (bool*) arg;
    while(!(*args)){
        putchar('2');
        fflush(stdout);
        sleep(1);
    }
    pthread_exit((void*)2);
}

int main(){
    cout << "Программа начала работу\n";
    setlocale(0, "");
    pthread_t id1, id2;
    bool is_end1 = false, is_end2 = false;

    pthread_create(&id1, NULL, proc1, &is_end1);
    pthread_create(&id2, NULL, proc2, &is_end2);
    
    cout << "\nПрограмма ждет нажатия клавиши\n";
    getchar();
    cout << "Клавиша нажата\n";

    char name1[16];
    char name2[16];
    pthread_getname_np(id1, name1, 16);
    pthread_getname_np(id2, name2, 16);
    string first_name = string(name1);
    string second_name = string(name2);
    cout << "Имя первого потока - " << first_name << endl;
    cout << "Имя второго потока - " << second_name << endl;

    char new_name1[16];
    char new_name2[16];
    strcpy(new_name1, "first_thread\0");
    strcpy(new_name2, "second_thread\0");
    pthread_setname_np(id1, new_name1);
    pthread_setname_np(id2, new_name2);

    pthread_getname_np(id1, new_name1, 16);
    pthread_getname_np(id2, new_name2, 16);
    first_name = string(new_name1);
    second_name = string(new_name2);
    cout << "Обновлённое имя первого потока - " << first_name << endl;
    cout << "Обновлённое имя второго потока - " << second_name << endl;

    is_end1 = true;
    is_end2 = true;

    int *exitcode1, *exitcode2;
    
    pthread_join(id1, (void**)&exitcode1);
    cout << "Exit code of proc1 is ";
    cout << exitcode1 << endl;

    pthread_join(id2, (void**)&exitcode2);
    cout << "Exit code of proc2 is ";
    cout << exitcode2 << endl;

    cout << "Программа закончила работу\n";
    return 0;
}