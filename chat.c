#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h> 
#define MAX 140
#define PORT 8686

// server chat 
void serverchat(int clientfd)
{
	char buffer[MAX];
    int val;
	bool on = true;
    while(on) {
        bzero(buffer, MAX);
        read(clientfd, buffer, sizeof(buffer));
        printf("Friend: %sYou: ", buffer);
        bzero(buffer, MAX);
        val= 0;
        while ((buffer[val++] = getchar()) != '\n') {
            if (sizeof(buffer) > 140) {
				printf("Message too large, please send another.");
			}
		}
        write(clientfd, buffer, sizeof(buffer));
    }
}
// client chat 
void clientchat(int sockfd) {
	char buffer[MAX];
    int val;
	bool on = true;
    while(on) {
        bzero(buffer, sizeof(buffer));
        printf("You: ");
        val = 0;
        while ((buffer[val++] = getchar()) != '\n') {
           if (sizeof(buffer) > 140) {
				printf("Message too large, please send another.");
			}
		}
		if (sizeof(buffer) > 140) {
			printf("Message too large, please send another.");
		}
        write(sockfd, buffer, sizeof(buffer));
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));
        printf("Friend: %s", buffer);
    }
}

int main(int argc, char *argv[]) {

	if (argc == 1) {
		struct sockaddr_in serverAddress;
		struct sockaddr_in clientAddress;
		int sockfd;
		int clientfd;
		int length;
	
		printf("Welcome to Chat!\n");
		printf("Waiting for a connection on 192.82.47.232 port 8686\n");
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			printf("Failed to create socket\n");
			exit(0);
		}
		
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(PORT);
		length = sizeof(clientAddress);

		if ((bind(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) != 0) {
			printf("Couldn't bind the socket.\n");
			exit(0);
		}

		if ((listen(sockfd, 5)) != 0) {
			printf("Failed to listen.\n");
			exit(0);
		}

		clientfd = accept(sockfd, (struct sockaddr*)&clientAddress, &length);
		if (clientfd < 0) {
			printf("Failed to accept a server\n");
			exit(0);
		}
		else {
			printf("Found a friend! You receive first.\n");
		}

		serverchat(clientfd);
		close(sockfd);
	}
	else {

		struct sockaddr_in serverAddress;
		int sockfd;
		char *port = NULL; 
		char *serverip = NULL; 
		
		if (argc <= 0) {
			printf("Useage: -p %s <port> -s %s <ip> or -s %s <ip> -p %s <port>", argv[4], argv[2], argv[2], argv[4]);
		}
		printf("Connecting to server...\n");

		void printHelp() {
			printf("Usage: ./chat [OPTIONS]\n");
			printf("  -p <port>       Port to connect to on\n");
			printf("  -s <server_ip>  IP address of the server\n");
			printf("  -h              Display this message\n");
			exit(0);
		}
		bool validPort (const char *port) {
			for (size_t i = 0; i < strlen(port); i++ ) {
				if (!isdigit(port[i])) {
					return false;
				}
			}
			return true;
		}
		bool validIP (const char *ip) {
			return inet_pton(AF_INET, ip,  &serverAddress.sin_addr) != 0;
		}
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-p") == 0) {
				if (i + 1 < argc && validPort(argv[i + 1])) {
						port = argv[i+1];
				}
				else {
					printf("Not a valid port\n");
					return 1;
				}
			}
			else if (strcmp(argv[i], "-s") == 0) {
				if (i + 1 < argc && validIP(argv[i + 1])) {
					serverip = argv[i + 1];
				} else {
					printf("Not a valid server IP\n");
					return 1;
				}
			} 
			else if (strcmp(argv[i], "-h") == 0) {
				printHelp();
			}
		}
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			printf("Failed to create a socket\n");
			exit(0);
		}
		else {
			printf("Connected!\n");
		}
		bzero(&serverAddress, sizeof(serverAddress));

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
		serverAddress.sin_port = htons(PORT);
	
		if (connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress))!= 0) {
			printf("Failed to create server connection.\n");
			exit(0);
		}
		else {
			printf("Connected to a friend! You send first.\n");
		}

		clientchat(sockfd);
		close(sockfd);
	}
}
