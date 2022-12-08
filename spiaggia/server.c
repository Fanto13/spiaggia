#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	int sockd = socket(AF_INET,SOCK_STREAM,0);
	int port = 6969;//porta utilizzata per comunicare con il client
	char buffer[128];
	printf("Creazione socket\n");
	struct sockaddr_in server;
	//azzera i dati di &server
	bzero((char *) &server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	// associa dominio,protocollo, tipo all'indirizzo e numero di porta
	bind(sockd,(struct sockaddr *) &server, sizeof(server));
	//numero massimo di connessioni
	int limit = 1;
	int n;
	//mette il socket in ascolto su sockd
	listen(sockd,limit);

	struct sockaddr_in client;
	socklen_t cli;
	cli = sizeof(client);

	int newsocket;
	newsocket = accept(sockd, (struct sockaddr*) &client, &cli); 
	n = read(newsocket, buffer, 128);
	//printf("%s\n",buffer );

	if (strcmp(buffer, "BOOK") == 0){
		

	}
	int err = shutdown(sockd, 2);
    fprintf(stderr, "%d\n",err );
	//instaura la connessione tra client e server, sul socket(indirizzo ip, porta e protocollo), specificando (e salvando)
	//il client connesso e la lunghezza della struttura associata


	//la chiusura del socket non mi permette immediatamente 
	//di creare un altra connessione (lato server), dato che durante la 
	//chiusura si ha una chiamata ad una wait per evitare che pacchetti 
	//ritardati nella prima connessione vengano consegnati alla seconda connessione  

	return 0;
}