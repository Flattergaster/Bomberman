#include "../include/server.h"

player_t players[MAX_PLAYERS];

int create_socket(struct sockaddr_in *addr, uint16_t port) {
	int sd = 0, i = 0;
	socklen_t size;
	
	sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sd < 0) {
		perror("socket()");
		return EXIT_FAILURE;
	}
    
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    addr->sin_port = htons(port);
    size = sizeof(*addr);
	
	if (bind(sd, addr, size) < 0) {
		if (errno == EADDRINUSE) {
            for (i = PORT + 1; i < MAX_PORT_VALUE; i++) {
                errno = 0;
                addr->sin_port = htons(i);
                
                if (bind(sd, addr, size) == 0) {
                    break;
                } 
            }
        } else {
            perror("bind()");
            return EXIT_FAILURE;
        }
	}
    
	return sd;
}

int create_player(int sd, struct sockaddr_in client_addr) {
    int index = 0;
    struct sockaddr_in server_addr;
    
    sd = create_socket(&server_addr, PORT);
    
    for (index = 0; index < MAX_PLAYERS; index++) {
        if (players[index].p_id == 0) {
            /*TODO after add shared constants*/
            players[index].p_id = rand() + 4;
            players[index].x = rand();
            players[index].y = rand();
            players[index].bomb_str = rand();
            players[index].bomb_pwr = rand();
            players[index].sd = sd;
            players[index].end_point = client_addr;
            return index;
        }
    }
    
    return EXIT_FAILURE;
}

int listener_new_clients(int sd) {
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
	pthread_t *threads = NULL;
	int status = 0, size_message = 64, sd_client = 0, index_client = 0;
	char *buffer = NULL;
    int *args_thread = NULL;
	socklen_t size_addr;
	
	size_addr = sizeof(client_addr);
	buffer = calloc(size_message, sizeof(char));
    threads = malloc(sizeof(pthread_t) * MAX_PLAYERS);
    args_thread = malloc(sizeof(int) * MAX_PLAYERS);
	
	while (1) {
		status = recvfrom(
					sd, 
					buffer, 
					size_message, 
					0, 
					&client_addr, 
					&size_addr);
		if (status < 0) {
			perror("recvfrom()");
			return EXIT_FAILURE;
		}
        
        sd_client = create_socket(&server_addr, PORT);
        index_client = create_player(sd_client, client_addr);
        
        if (index_client < 0) {
            continue;
        }
        
        args_thread[index_client] = index_client;
        pthread_create(
                &threads[index_client],
                NULL,
                client_thread,
                &args_thread[index_client]);
	}
	
	close(sd);
	free(buffer);
    free(threads);
    
	return EXIT_SUCCESS;
}

void *client_thread(void *args) {
    int index = *((int *) args);
    printf("%d\n", index);
    pthread_exit(0);
}
