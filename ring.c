#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

char header[2048];

char list[128][512];
size_t listc;

void
load_files(){
	FILE *fp = fopen("list", "r");
	while(fgets(list[listc], 512, fp) != NULL && listc < 126) {
		list[listc][strlen(list[listc])-1] = '\0'; // rem \n
		++listc;
	}
	fclose(fp);

	fp = fopen("header", "r");
	fread(header, 1, 2048, fp);
	fclose(fp);
}

void
socket_thread(int client_sock){
	unsigned n = time(NULL)/60;
	srand(n);

	size_t idx = ((unsigned)rand())%listc;

	char h[2048];
	sprintf(h, header, list[idx]);

	write(client_sock, h, strlen(h));

	shutdown(client_sock, 1);
	close(client_sock);
}

int
main(int argc, char *argv[]){
	load_files();

	int socket_desc, client_sock, c;
	struct sockaddr_in server, client;

	// Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		fputs("Socket create error\n", stderr);
		return 1;
	}

	// Prepare sockaddr_in struct
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8079);

	// Bind
	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server))<0){
		fputs("bind err\n", stderr);
		return 1;
	}
	puts("Bound.");

	listen(socket_desc, 3);

	for(;;) {
		c = sizeof(struct sockaddr_in);
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if(client_sock < 0){
			fputs("failed\n", stderr);
			fflush(stderr);
			return 1;
		}

		socket_thread(client_sock);
	}
}
