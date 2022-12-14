#include "tcpCommon.h"


typedef struct thread_info {
	//Struct permettant de transmettre plusieurs types d'informations aux threads
	pthread_t *thread_id;
	int* socket;
}my_data;

void *myThreadFun(void *vargp)
{
	char buffer[1024];
    my_data *current_data = (my_data *) vargp;
    int newSocket = *(current_data->socket);
    bool running = true;
	
	while(running){
		recv(newSocket, buffer, 1024, 0);
		printf("Thread ID : %ld; Socket : %d; msg : %s \n", *(current_data->thread_id), *(current_data->socket), buffer);
		if(strcmp(buffer, ":exit\n") == 0){
			//printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
			printf("Ah cioasu my furendo! \n");
			running = false;
			//exit(1);
			
		}else{
			send(newSocket, buffer, strlen(buffer), 0);
			bzero(buffer, sizeof(buffer));
		}
	}
	printf("Hehehe Bye bye! \n");
}

int main(){
	int max_client = 10;
	pthread_t thread_list[max_client];
	int my_newsock[max_client];
	int index = 0;
	
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr)); //Ajout du caractère fin de chaine. Sinon "Segmentation Fault"
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Permet aux autres processus de bind avec les mêmes options
	int opt = 1;
    setsockopt (sockfd , SOL_SOCKET , SO_REUSEPORT , &opt , sizeof (opt ));
	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		perror("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", PORT);
	printf("[]The full address %s:%d\n", inet_ntoa(serverAddr.sin_addr), PORT);
	
	if(listen(sockfd, max_client) == 0){
		printf("[+]Listening....\n");
	}else{
		perror("[-]Error in binding.\n");
		exit(1);
	}

	while (1) {
		newSocket = accept(sockfd, (struct sockaddr *) &newAddr, &addr_size);
		if (newSocket < 0) {
		  perror("[-]ERROR in accepting. \n");
		  exit(1);
		}
		printf("Connection accepted from %s:%d \n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		my_newsock[index] = newSocket;
		my_data data = {&thread_list[index], &my_newsock[index]};
		pthread_create(&thread_list[index], NULL, myThreadFun, &data);
		//pthread_join(thread_list[index],NULL);
		
		if (index>=max_client){
			perror("[-]Error out of index thread_list or socket_list \n");
			exit(1);
		}
		index++;
	}


	close(newSocket);
	pthread_exit(NULL);
	return 0;
}
