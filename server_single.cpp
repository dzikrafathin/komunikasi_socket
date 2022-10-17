#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>

#define PORT 9200

int main(int argc, char *argv[])
{
    int obj_server, sock, reader;

    struct sockaddr_in address;

    int opted = 1;

    int address_length = sizeof(address);

    char buffer[1024] = {0};

    char *message = "Pesan dari server";

    if ((obj_server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Gagal socket\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(obj_server, SOL_SOCKET, SO_REUSEADDR,
                   &opted, sizeof(opted)))
    {
        perror("Can't set the socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(obj_server, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("Binding of socket failed !");
        exit(EXIT_FAILURE);
    }

    if (listen(obj_server, 3) < 0)
    {
        perror("Gagal listen");
        exit(EXIT_FAILURE);
    }

    if (
        (
            sock =
                accept(
                    obj_server,
                    (struct sockaddr *)&address,
                    (socklen_t *)&address_length)) < 0)
    {
        perror("Error accept");
        exit(EXIT_FAILURE);
    };

    printf("%d", sock);

    // reader = read(sock, buffer, 1024);
    // printf("%s\n", buffer);

    // send(sock, message, strlen(message), 0);
    // printf("Server : Message has been sent ! \n");

    while (1) {
        reader = read(sock, buffer, 1024);
        printf("Pesan masuk = %s\n", buffer);
    }


    return 0;
}