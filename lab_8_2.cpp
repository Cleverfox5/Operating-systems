#include <iostream>
#include <pthread.h>
#include <string>
#include <mqueue.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <mqueue.h>
#include <unistd.h>
#include <vector>
#include <pwd.h>
#include <csignal>

using namespace std;

int sockfd;
pthread_t id1;
pthread_t id2;
pthread_t id3;
bool is_end_reception = false;
bool is_end_processing = false;
bool is_end_conection = false;
int counter = 1;

void sig_handler(int signo)
{
    printf("\nget SIGINT; %d\n", signo);

    if (signo == SIGINT) {
        is_end_reception = false;
        is_end_processing = false;
        is_end_conection = false;
    }

    if (close(sockfd) == -1){
        printf("Не удалось закрыть соединение %d\n", 2);
    }
    else{
        printf("Соединение успешно закрто %d\n", 2);
    }

    printf("Программа %d закончила работу\n", 2);
    exit(0);
}

void * sendMessage(void * arg){
    bool *args = (bool *)arg;
    char buffer[1024];

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "srvsock.soc", sizeof(server_addr.sun_path) - 1);
    socklen_t server_len = sizeof(server_addr);

    while (!(*args)){
        memset(buffer, 0, sizeof(buffer));

        strcpy(buffer, "Запрос ");
        strcat(buffer, (to_string(counter++)).c_str());

        int sentMessage = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, server_len);
        if (sentMessage == -1){
            perror("Error with sendto");
            if (close(sockfd) == -1){
                printf("Не удалось закрыть сокет\n");
            }
            else{
                printf("Соединение успешно закрыто\n");
            }

            exit(-1);
        }
        printf("%s\n", buffer);

        sleep(1);
    }

    pthread_exit((void*)0);
}

void * getMessage(void * arg){
    bool *args = (bool*) arg;    
    char buffer[1024];

    struct sockaddr_un server_re_addr;
    socklen_t server_re_len = sizeof(server_re_addr);
    while (!(*args)){
        memset(buffer, 0, sizeof(buffer));

        int receivedMessage = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_re_addr, &server_re_len);
        if (receivedMessage == -1){
            perror("Error with recvfrom");
            close(sockfd);
            exit(-1);
        }

        printf("%s\n", buffer);

    }

    pthread_exit((void*)0);
}

void * isConection(void * arg){
    bool *args = (bool *)arg;
    pthread_create(&id2, NULL, sendMessage, &is_end_processing);
    pthread_create(&id1, NULL, getMessage, &is_end_reception);
    while (!(*args)){
        sleep(1);
    }

    pthread_exit((void*)0);
}

int main(){
    setlocale(0, "");
    signal(SIGINT, sig_handler);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1){
        perror("Ошибка сокета");
        return -1;
    }
    
    struct sockaddr_un server_here_addr;
    server_here_addr.sun_family = AF_UNIX;
    strncpy(server_here_addr.sun_path, "сlisock.soc", sizeof(server_here_addr.sun_path) - 1);

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("Error with setsockopt");
        exit(-1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
        perror("Error with setsockopt");
        exit(-1);
    }


    if (bind(sockfd, (struct sockaddr*)&server_here_addr, sizeof(server_here_addr)) == -1) {
        perror("Error with bind");
        close(sockfd);
        return -1;
    }
    
    printf("Клиент запущен\n");

    struct sockaddr_un client_addr;
    socklen_t addr_c_len = sizeof(client_addr);
    if (getsockname(sockfd, (struct sockaddr*)&client_addr, &addr_c_len) == -1) {
        perror("Error with getsockname");
        close(sockfd);
        return -1;
    }
    
    cout << "Адрес: " << client_addr.sun_path << endl;
    
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


    if (close(sockfd) == -1){
        printf("Не удалось закрыть соединение %d\n", 2);
    }
    else{
        printf("Соединение успешно закрто %d\n", 2);
    }

    printf("Программа %d закончила работу\n", 2);

    return 0;

}
