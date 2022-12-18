#include "tcpCommon.h"
#include "db.hpp"
#include "queries.hpp"

pthread_mutex_t new_access;
pthread_mutex_t write_access;
pthread_mutex_t reader_counter; //Ce mutex ne sert qu'à s'assurer que readers_c ne soit modifié que par UN SEUL thread à la fois
int readers_c = 0;

int newSocket;
int const max_client = 10;
int socket_list[max_client];
int socket_index = 0;

typedef struct thread_info {
		//Struct permettant de transmettre plusieurs types d'informations aux threads
	pthread_t *thread_id;
	int* socket;
	database_t *db;
}my_data;

void reader_access_handling(database_t* db, const char* const query, char* buffer_to_change){
	pthread_mutex_lock(&new_access);
	pthread_mutex_lock(&reader_counter); //Interaction avec la variable readers_c donc on bloque
	if (readers_c == 0){
		pthread_mutex_lock(&write_access);
		readers_c++;
	}
	pthread_mutex_unlock(&reader_counter); //Interaction terminée avec la variable readers_c donc on relache
	pthread_mutex_unlock(&new_access);
	
	//Reading...
	parse_and_execute_select(db, query,buffer_to_change);
	//Hey just finished to read!
	
	pthread_mutex_lock(&reader_counter);
	readers_c--;
	if (readers_c == 0){
		//On ne libère les mutex ecriture que ssi il n'y a plus aucun accès en lecture
		pthread_mutex_unlock(&write_access);
	}
	pthread_mutex_unlock(&reader_counter);
}
void write_access_handling(database_t* db, const char* const query, char* buffer_to_change){
	pthread_mutex_lock(&new_access);
	pthread_mutex_lock(&write_access);
	pthread_mutex_unlock(&new_access);

	if (strncmp("update", query, sizeof("update")-1) == 0) {
		parse_and_execute_update(db, query,buffer_to_change);
	} 
	else if (strncmp("insert", query, sizeof("insert")-1) == 0) {
		parse_and_execute_insert(db, query, buffer_to_change);

	} 
	else if (strncmp("delete", query, sizeof("delete")-1) == 0) {
		parse_and_execute_delete(db, query,buffer_to_change);
	} 
	else {
		query_fail_bad_query_type(buffer_to_change);
	}

	pthread_mutex_unlock(&write_access);
	}
void parse_and_execute(database_t* db, const char* const query, char* buffer_to_change) {
	if (strncmp("select", query, sizeof("select")-1) == 0) {
		//Accès en LECTURE
		reader_access_handling(db,query,buffer_to_change);
	}
	else{
		//Accès en ECRITURE
		write_access_handling(db,query,buffer_to_change);
	}
  
}

void sig_handler(int signum){
	char buffer[1024];
	printf("\nAdieux camarades, j'ai été heureux de vous avoir connu! : %d\n",signum);
	strcpy(buffer,":exit\n");
	for (int i=0; i<socket_index;i++){
		printf("Je viens d'envoyer %s au socket %d \n",buffer, socket_list[i]);
		send(socket_list[i], buffer, strlen(buffer), 0);
		}
	//send(newSocket, buffer, strlen(buffer), 0);
	sleep(0.5);
	exit(0);
}
void *myThreadFun(void *vargp){
	
	
	char buffer[1024];
	char buffer_to_change[4096];
    my_data *current_data = (my_data *) vargp;
    int newSocket = *(current_data->socket);
    bool running = true;

	
	while(running){
		recv(newSocket, buffer, 1024, 0);
		printf("Thread ID : %ld; Socket : %d; msg : %s \n", *(current_data->thread_id), *(current_data->socket), buffer);
		//printf("Client socket : %d; msg : %s \n",newSocket, buffer);
		if(strcmp(buffer, ":exit\n") == 0){
			printf("Ah cioasu my furendo! \n");
			running = false;
			
		}else{
			//mutex lock 
			//send(newSocket, buffer, strlen(buffer), 0);
			parse_and_execute(current_data->db, buffer, buffer_to_change);
			//mutex unlock 
			printf("%s \n",buffer_to_change);
			send(newSocket, buffer_to_change, strlen(buffer_to_change), 0);
			bzero(buffer_to_change, 4096);
		}
	}
	
	return NULL;
}


void setup(database_t &db,int argc,char *argv[]){
	
	signal(SIGINT,sig_handler);
	signal(SIGUSR1,sig_handler);
	
	pid_t my_PID = getpid(); printf("Hehehe voici mon PID : %d \n", my_PID);
	pthread_mutex_init(&new_access, NULL);
	pthread_mutex_init(&write_access, NULL);
	pthread_mutex_init(&reader_counter, NULL);
	
	const char *database_file;
	
	if (argc > 1) {
		database_file = argv[1];
	} else {
		//fprintf(stderr, "Mandatory parameter (db file) missing.\n");
		perror("Mandatory parameter (db file) missing.\n");
		exit(1);
	}
	db_load(&db, database_file);
}

int main(int argc,char *argv[]){
	database_t db;
	setup(db,argc,argv);
	
	
	
	
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr)); //Ajout du caractere fin de chaine. Sinon "Segmentation Fault"
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Permet aux autres processus de bind avec les memes options
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

	while(1){
		newSocket = accept(sockfd, (struct sockaddr *) &newAddr, &addr_size);
		if (newSocket < 0) {
			if (errno == EINTR) {
				continue;
			 }else{
			  perror("[-]ERROR in accepting. \n");
			  exit(1);
			  }
		}
		printf("Connection accepted from %s:%d \n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		pthread_t client_thread;
		int *client_sock = (int*) malloc(sizeof(int));
		*client_sock = newSocket;
		if (socket_index<max_client){
			socket_list[socket_index] = newSocket;
			socket_index++;
		}
		my_data data = {&client_thread, client_sock, &db};
		pthread_create(&client_thread, NULL, myThreadFun, &data);
	}


	close(newSocket);

	pthread_mutex_destroy(&new_access);
	pthread_mutex_destroy(&write_access);
	pthread_mutex_destroy(&reader_counter);
	
	pthread_exit(NULL);
	return 0;
}
