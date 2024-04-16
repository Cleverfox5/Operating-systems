#include <iostream>
#include <pthread.h>
#include <string>
#include <mqueue.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <mqueue.h>
#include <unistd.h>
#include <queue>
#include <pwd.h>
#include <csignal>
#include <arpa/inet.h>


using namespace std;

int sockfd;
pthread_t id1;
pthread_t id2;
pthread_t id3;
queue<string> msglist;
pthread_mutex_t mutex;
bool is_end_reception = false;
bool is_end_processing = false;
bool is_end_conection = false;
int counter = 1;
bool is_first = true;
bool is_second = false;
struct sockaddr_un client_addr;
socklen_t client_len;

void sig_handler(int signo)
{
    printf("\nget SIGINT; %d\n", signo);

    if (signo == SIGINT) {
        is_end_reception = false;
        is_end_processing = false;
        is_end_conection = false;
    }

    if (pthread_mutex_destroy(&mutex) == -1){
        perror("Не удалось удалить мьютекс 1\n");
    }
    else{
        printf("Мьютекс успшно удалён %d\n", 1);
    }

    if (close(sockfd) == -1){
        printf("Не удалось закрыть соединение %d\n", 1);
    }
    else{
        printf("Соединение успешно закрто %d\n", 1);
    }

    printf("Программа %d закончила работу\n", 1);
    exit(0);
}

void * getMessage(void * arg){
    bool *args = (bool*) arg;    
    char buffer[1024];

    client_len = sizeof(client_addr);
    while (!(*args)){
        memset(buffer, 0, sizeof(buffer));

        int receivedMessage = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_len);
        if (receivedMessage == -1){
            perror("Error with recvfrom");
            close(sockfd);
            exit(-1);
        }
        if(is_first){
            cout << "Адрес Клиента: " << client_addr.sun_path << endl;
            is_second = true;
            is_first = false;
        }

        pthread_mutex_lock(&mutex);
        msglist.push(buffer);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit((void*)0);
}

void * sendMessage(void * arg){
    bool *args = (bool *)arg;
    char buffer[1024];
    char buf1[1024];
    char *buf = buf1;

    client_addr.sun_family = AF_UNIX;
    
    while (!(*args)){
        pthread_mutex_lock(&mutex);
        if (!msglist.empty()){
            memset(buffer, 0, sizeof(buffer));
            memset(buf1, 0, sizeof(buf1));

            printf("%s\n", msglist.front().c_str());
            msglist.pop();
            pthread_mutex_unlock(&mutex);
            
            char *user_name = getlogin();
            struct passwd *str_user_name = getpwnam(user_name);

            if (str_user_name == NULL){
                printf("Пользователь %s не найден", user_name);
                exit(-1);
            }
            
            strcpy(buf, str_user_name->pw_dir);
            strcpy(buffer, "Ответ ");
            strcat(buffer, (to_string(counter++)).c_str());
            strcat(buffer, ":");
            strcat(buffer, buf);

            int sentMessage = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, client_len);
            if (sentMessage == -1){
                perror("Error with sendto");
                close(sockfd);
                exit(-1);
            }
            printf("%s\n", buffer);
        }
        else{
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }

    pthread_exit((void*)0);
}

void * isConection(void * arg){
    bool *args = (bool *)arg;
    pthread_create(&id1, NULL, getMessage, &is_end_reception);
    pthread_create(&id2, NULL, sendMessage, &is_end_processing);
    while (!(*args)){
        sleep(1);
    }

    pthread_exit((void*)0);
}

int main(){
    setlocale(0, "");
    signal(SIGINT, sig_handler);

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Error with pthread_mutex_init");
        return -1;
    }
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1){
        perror("Ошибка сокета");
        return -1;
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "srvsock.soc", sizeof(server_addr.sun_path) - 1);

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("Error with setsockopt");
        exit(-1);
    }

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error with bind");
        close(sockfd);
        return -1;
    }

    cout << "Сервер запущен, ожидает сообщения от клиента\n";
    
    pthread_create(&id3, NULL, isConection, &is_end_conection);

    printf("\nПрограмма ждет нажатия клавиши\n");
    getchar();
    printf("Клавиша нажата\n");

    is_end_conection = true;
    is_end_processing = true;
    is_end_reception = true;

    int *exitcode1, *exitcode2, *exitcode3;

    pthread_join(id1, (void**)&exitcode1);
    pthread_join(id2, (void**)&exitcode2);
    pthread_join(id3, (void**)&exitcode3);

    if (pthread_mutex_destroy(&mutex) == -1){
        perror("Не удалось удалить мьютекс 1\n");
    }
    else{
        printf("Мьютекс успшно удалён %d\n", 1);
    }

    if (close(sockfd) == -1){
        printf("Не удалось закрыть соединение %d\n", 1);
    }
    else{
        printf("Соединение успешно закрто %d\n", 1);
    }

    printf("Программа %d закончила работу\n", 1);

    return 0;
}