#include "tcpCommon.h"

int clientSocket;

void sig_handler(int signum){
	char buffer[1024];
	printf("\nJe vous fais mes adieux, camarades! : %d\n",signum);
	strcpy(buffer,":exit\n");
	send(clientSocket, buffer, strlen(buffer), 0);
	sleep(0.5);
	exit(0);
}

int main(){
	signal(SIGINT,sig_handler);
	signal(SIGUSR1,sig_handler);
	
	int ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	char buffer_recu[4096];

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server Address %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	while(1){
		printf("Client: \t");
		fgets(buffer, sizeof(buffer),stdin); //Recupere du STDIN et met tout dans un seul buffer
		send(clientSocket, buffer, strlen(buffer), 0);
		
		if(strcmp(buffer, ":exit\n") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}
		bzero(buffer, 1024);

		if(recv(clientSocket, buffer_recu, 4096, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			if(strcmp(buffer_recu, ":exit\n") == 0){
				close(clientSocket);
				printf("[-]Disconnected from server.\n");
				exit(1);
			}
			printf("%s\n", buffer_recu);
			bzero(buffer_recu, 4096);
		}
	}

	return 0;
}
