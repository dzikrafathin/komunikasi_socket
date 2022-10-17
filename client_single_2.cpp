#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
//#define port 9200

int main(int argument, char const *argv[])
{
    int obj_socket = 0, reader;
    struct sockaddr_in serv_addr;
    char message[1024] = {0};
    char buffer[1024] = {0};

    char alamatip[] = "192.168.39.14";
    int port = 8888;

    printf("Masukan alamat IP Server = ");
    scanf("%s", alamatip);

    printf("Masukan port server = ");
    scanf("%d", &port);

    if ((obj_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error !");
        return -1;
    }


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    

    if (inet_pton(AF_INET, alamatip, &serv_addr.sin_addr) <= 0)
    {
        printf("\n IP ADDRESS TIDAK VALID !!\n");
        return -1;
    }

    if (connect(obj_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Koneksi gagal !");
        return -1;
    }

    reader = read(obj_socket, buffer, 1024);
    printf("Pesan dari server = %s\n", buffer);

    while (1) {

        printf("Masukan pesan yang ingin dikirim = ");
        scanf("%s", &message);

        send(obj_socket, message, strlen(message), 0);

        reader = read(obj_socket, buffer, 1024);
        printf("Balasan dari server = %s\n", buffer);

    }

    
    // printf("\nClient : Message has been sent !\n");
    // reader = read(obj_socket, buffer, 1024);
    // printf("%s\n", buffer);
    return 0;
}