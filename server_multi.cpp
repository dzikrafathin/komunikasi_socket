#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define TRUE 1
#define PORT 5500

int main(int argc, char *argv[]) {

    int opt = TRUE;
    int master_sock, addrlen, new_sock, client_sock[30],
    maximum_clients = 30, act, i, value_read, sock_descriptor;
    int maximum_socket_descriptor;
    struct sockaddr_in adr{};

    char buff[1025];

    fd_set readfds;

    char *message = "ECHO Daemon v1.0 \r\n";

    for (i = 0 ; i < maximum_clients; i++) {
        client_sock[i] = 0;
    }

    if ((master_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("SOCKET GAGAL");
        exit(EXIT_FAILURE);
    }

    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = INADDR_ANY;
    adr.sin_port = htons(PORT);

    if (bind(master_sock, (struct sockaddr*)&adr, sizeof(adr)) < 0) {
        perror("BIND GAGAL");
        exit(EXIT_FAILURE);
    }

    printf("Sudah listen pada port: %d \n", PORT);

    if (listen(master_sock,3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(adr);

    puts("Menunggu koneksi");

    while (TRUE) {
        FD_ZERO(&readfds);
        FD_SET(master_sock, &readfds);

        maximum_socket_descriptor = master_sock;

        for (i=0; i<maximum_clients;i++) {
            sock_descriptor = client_sock[i];

            if (sock_descriptor > 0) {
                FD_SET(sock_descriptor, &readfds);

                if (sock_descriptor > maximum_socket_descriptor) {
                    maximum_socket_descriptor = sock_descriptor;
                }
            }
        }

        act = select(maximum_socket_descriptor+1, &readfds, nullptr, nullptr, nullptr);

        if ((act < 0) && (errno!=EINTR)) {
            printf("Gagal select");
        }

        if (FD_ISSET(master_sock, &readfds)) {
            if ((new_sock = accept(master_sock, (struct sockaddr *)&adr, (socklen_t*)&addrlen)) < 0) {
                perror("Accept!");
                exit(EXIT_FAILURE);
            }
        }

        printf(
            "This is a New Connection,The socket file descriptor is %d and the IP is : %s on Port : %d\n", 
            new_sock, 
            inet_ntoa(adr.sin_addr),
            ntohs(adr.sin_port)
        );

        if (send(new_sock, message, strlen(message), 0) != strlen(message)) {
            perror("Error mengirim");
        }

        puts("Selamat datang");

        for (i=0;i<maximum_clients;i++) {
            if (client_sock[i] == 0) {
                client_sock[i] = new_sock;
                printf("Adding new socket to the list of sockets as %d\n" , i);
                break;
            }
        }

        for (i = 0; i < maximum_clients; i++) {
            sock_descriptor = client_sock[i];

            if (FD_ISSET(sock_descriptor, &readfds)) {
                if ((value_read = read(sock_descriptor, buff, 1024)) == 0) {
                    getpeername(
                        sock_descriptor, 
                        (struct sockaddr*)&adr, 
                        (socklen_t*)&addrlen
                    );

                    printf("Disconnected Host. Their , IP %s and PORT %d \n" ,
                          inet_ntoa(adr.sin_addr) , ntohs(adr.sin_port));
                    close( sock_descriptor ); //Closing the socket and marking it as 0 in the list to be reused
                    client_sock[i] = 0;
                } else {
                    buff[value_read] = '\0'; //Setting the string terminating NULL byte on the end of the data that is read
                    send(sock_descriptor , buff , strlen(buff) , 0 );
                }
            }
        }

    }
    return 0;

}