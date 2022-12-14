#include "tcpCommon.h"


typedef struct thread_info {
  pthread_t *thread_id;
  int* socket;
}my_data;

void *myThreadFun(void *vargp)
{
	char buffer[1024];
    my_data *current_data = (my_data *) vargp;
    int newSocket = *(current_data->socket);
    
	printf("Thread ID : %ld; Socket : %d \n\n", *(current_data->thread_id), *(current_data->socket));

		recv(newSocket, buffer, 1024, 0);
		
		if(strcmp(buffer, ":exit") == 0){
			//printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
			printf("Ah cioasu my furendo! \n");
			exit(1);
		}else{
			send(newSocket, buffer, strlen(buffer), 0);
			bzero(buffer, sizeof(buffer));
		}

	printf("Hehehe Bye bye! \n");
}

int main(){
	pthread_t thread_id;
	
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer_bis[1024];
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr)); //Ajout du caractère fin de chaine. Sinon "Segmentation Fault"
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", PORT);
	printf("[]The full address %s:%d\n", inet_ntoa(serverAddr.sin_addr), PORT);
	
	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}

	while (1) {
		newSocket = accept(sockfd, (struct sockaddr *) &newAddr, &addr_size);
		if (newSocket < 0) {
		  printf("ERROR in accepting. \n");
		  exit(1);
		}
		printf("Connection accepted from %s:%d \n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		
		pthread_t thread;
		
		int *my_newsock = malloc(sizeof(int));
		*my_newsock = newSocket;
		my_data data = {&thread, my_newsock};
		pthread_create(&thread, NULL, myThreadFun, &data);
		//pthread_detach(thread);
	}


	close(newSocket);
	pthread_exit(NULL);
	return 0;
}
