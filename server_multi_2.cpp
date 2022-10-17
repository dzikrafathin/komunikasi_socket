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
#define FALSE 0
//#define port 8888

int main(int argc, char *argv[]) {

    int port = 8888;
    int max_clients = 30;

    printf("Masukan nomor port yang diinginkan untuk server = ");
    scanf("%d", &port);

    printf("Jumlah maksimal client yang dapat terhubung = ");
    scanf("%d", &max_clients);

    int opt = TRUE;

    int 
        master_socket,
        addrlen,
        new_socket,
        client_socket[max_clients],
        activity,
        i,
        valread,
        sd;
    
    int max_sd;

    struct sockaddr_in address;

    char buffer[1025];

    fd_set readfds;

    char *pesan = "Pesan selamat datang \n";

    for (i=0;i<max_clients;i++) {
        client_socket[i] = 0;
    }

    if (
        (master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0
    ) {
        perror("Socket gagal");
        exit(EXIT_FAILURE);
    }

    if (
        setsockopt(
            master_socket, 
            SOL_SOCKET, 
            SO_REUSEADDR, 
            (char *)&opt, 
            sizeof(opt)
        ) < 0) {
            perror("Setsockopt");
            exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (
        bind(
            master_socket,
            (struct sockaddr *)&address,
            sizeof(address)
        ) < 0
    ) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server berjalan pada port %d \n", port);  

    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (TRUE) {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);

        max_sd = master_socket;

        for (i=0;i<max_clients;i++) {
            sd = client_socket[i];

            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        activity = select(
            max_sd+1, 
            &readfds, 
            NULL, NULL, NULL
        );

        if ((activity < 0) && (errno != EINTR)) {
            printf("Select error");
        }

        if (FD_ISSET(master_socket, &readfds)) {

            if (
                (new_socket = accept(
                    master_socket,
                    (struct sockaddr *)&address,
                    (socklen_t*)&addrlen
                )) < 0
            ) {
                perror("Accept");
                exit(EXIT_FAILURE);
            }

            printf(
                "Ada koneksi baru , File Descriptor socket adalah %d , IP Address : %s , port : %d \n",
                new_socket,
                inet_ntoa(address.sin_addr),
                ntohs(address.sin_port)
            );

            if( send(new_socket, pesan, strlen(pesan), 0) != strlen(pesan) )  
            {  
                perror("send");  
            }  

            puts("Pesan selamat datang sudah dikirimkan");

            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Memasukan kedalam daftar socket sebagai : %d\n" , i);  
                         
                    break;  
                }  
            } 

        }

        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i]; 

            if (FD_ISSET( sd , &readfds))  
            {  
                
                char buff[1024] = {0};

                if ((valread = read(sd, buff, 1024)) == 0) {
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);  
                    printf("Koneksi terputus , ip %s , port %d \n" , 
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                         
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0; 
                } else {
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen); 
                    
                    char* pesan_balasan = "Pesan sudah diterima\n";

                    if (send(sd,pesan_balasan,strlen(pesan_balasan),0) != strlen(pesan_balasan)) {
                        perror("PESAN BALASAN GAGAL DIKIRIM");
                    } else {
                        printf("Pesan masuk dari IP Address %s , port %d = %s \n" , 
                            inet_ntoa(address.sin_addr) , ntohs(address.sin_port), buff);
                    }
                    

                }
            } 
        }

    }

    return 0;

}