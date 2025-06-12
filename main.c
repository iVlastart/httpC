#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    char header[] = "HTTP/1.0 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-Type: text/html\r\n"
                "\r\n";

    FILE *pFile;
    size_t bytesRead;
    
    const char* PORT_ENV = getenv("PORT");
    const char* BUFFER_ENV = getenv("BUFFER");

    int PORT = PORT_ENV ? atoi(PORT_ENV) : 8080;       // výchozí hodnota 8080, pokud není nastavena proměnná
    int BUFFER = BUFFER_ENV ? atoi(BUFFER_ENV) : 1024; // výchozí hodnota 1024
    char buff[BUFFER];
    char body[BUFFER];
    char resp[BUFFER];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(sockfd==-1)
    {
        perror("Webserver (socket)");
        return 1;
    }
    printf("Socket created!");


    if(bind(sockfd, (struct sockaddr*)&host_addr, host_addrlen)!=0)
    {
        perror("Webserver (bind)");
        return 1;
    }
    printf("Socket was binded");

    if(listen(sockfd, SOMAXCONN)!=0)
    {
        perror("Webserver (listen)");
        return 1;
    }
    printf("Socket is listening");

    for(;;)
    {
        int newsockfd = accept(sockfd, (struct sockaddr*)&host_addr, (socklen_t *)&host_addrlen);
        if(newsockfd<0)
        {
            perror("Webserver (accept)");
            continue;
        }
        printf("Connection accepted");
        int valRead = read(newsockfd, buff, BUFFER);
        if(valRead<0)
        {
            perror("Webserver (read)");
            continue;
        }

        pFile = fopen("./client/index.html", "r");
        bytesRead = fread(body, 1, BUFFER - 1, pFile);
        body[bytesRead] = '\0';  // ukonči řetězec
        fclose(pFile);    // Připoj tělo

        strcpy(resp, header);   // Zkopíruj hlavičku
        strcat(resp, body);     // Připoj tělo
        
        ssize_t valWrite = write(newsockfd, resp, strlen(resp));
        if(valWrite<0)
        {
            perror("Webserver (write)");
            continue;
        }
        close(newsockfd);
    }
    return 0;
}