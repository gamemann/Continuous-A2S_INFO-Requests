#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

const char request[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x54, 0x53, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x20, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79, 0x00};

struct a2s_info
{
    int *sockfd;
    struct sockaddr_in *socket;
    int *timeout;
};

void* sendRequest(void *arg)
{
    struct a2s_info *info = arg;
    int size = sizeof(*info->socket);
    int err = 0;

    // Let's get the time.
    time_t now;
    time(&now);

    if ((err = sendto(*info->sockfd, request, strlen(request) + 1, 0, (struct sockaddr *)info->socket, size)) < 0)
    {
        printf("[%s] Could not send A2S_INFO request (Error %d)...\n", ctime(&now), err);
        pthread_exit(NULL);

        return 0;
    }

    char reply[256];

    // Set timeout.
    struct timeval tv;
    tv.tv_sec = *info->timeout;
    tv.tv_usec = 0;

    setsockopt(*info->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    if ((err = recvfrom(*info->sockfd, reply, sizeof(reply) + 1, 0, (struct sockaddr *)info->socket, &size)) < 0)
    {
        printf("[%s] Could not receive A2S_INFO request (Error %d)...\n", ctime(&now), err);
        pthread_exit(NULL);

        return 0;
    }

    pthread_exit(NULL);

    return 0;
}

int main(int argc, char **argv)
{
    printf("The request is - %s\n", request);

    if (argc < 3)
    {
        printf("IP and/or Port argument not specified.\n\n Usage: a2s_info <IP> <Port> [<Time out> <Interval>]");

        return 0;
    }

    char* dstaddr = argv[1];
    int dstport = atoi(argv[2]);
    int timeout = 1;
    int interval = 1;

    // Check for timeout argument ;)
    if (argc >= 4)
    {
        timeout = atoi(argv[3]);
    }

    // Check for interval argument.
    if (argc >= 5)
    {
        interval = atoi(argv[4]);
    }

    printf("Sending A2S_INFO requests to %s:%d (timeout - %d, interval - %d)\n\n", dstaddr, dstport, timeout, interval);

    // Create socket.
    int sockfd;
    struct sockaddr_in server;

    // Configure socket.
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Check if socket is valid.
    if (sockfd < 0)
    {
        printf("Error creating socket...\n");

        return EXIT_FAILURE;
    }

    // Configure settings within the socket.
    bzero((char *)&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(dstaddr);
    server.sin_port = htons(dstport);

    // Now input everything into the a2s_info structure to pass along to the function.
    struct a2s_info info = { 0 };

    info.socket = &server;
    info.sockfd = &sockfd;
    info.timeout = &timeout;

    while (1)
    {
        // Create a new thread for each request.
        pthread_t pthread_id;
        pthread_create(&pthread_id, NULL, &sendRequest, (void *)&info);

        sleep(interval);
    }

    return EXIT_SUCCESS;
}