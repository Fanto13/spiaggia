#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <unistd.h>
#include <netdb.h>

void send_receive(int);
int main(int argc, char const *argv[])
{
	int sockd;
	struct sockaddr_in server;
	struct hostent* pserv = gethostbyname("localhost");
	char buffer[128];
	int n,i;

	int port = 6969;

	sockd  = socket(AF_INET,SOCK_STREAM,0);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (connect(sockd, (struct sockaddr*)&server ,sizeof(server)) < 0)
	{
		fprintf(stdout, "IMPOSSIBILE CONNETTERSI\n");
		exit(1);
	}
	printf("Connesso\n\nInserisci BOOK per prenotare\nAVAILABLE [numero riga] per la disponibilita\nCANCEL $id [$data_inizio $data_fine] per eliminare prenotazioni\n");
	fgets(buffer, 128, stdin);
	n = write(sockd, buffer, strlen(buffer));

	char tmp[10]=""; 
	for (i = 0; i < 9 && buffer[i]!=' ' && buffer[i]!='\0' &&  buffer[i]!='\n'; ++i)
	{
		tmp[i] = buffer[i];
	}
	printf("%s\n",tmp);
	if(strcmp(tmp,"BOOK")==0)
	{
		n = read(sockd,buffer,128);//risposta del server
		if(strcmp(buffer,"OK"))
		{
			fprintf(stdout, "INSERISCI BOOK $id\n");
			fgets(buffer, 128, stdin);
			n = write(sockd,buffer,strlen(buffer));
			n = read(sockd,buffer,128);
			if (strcmp(buffer,"AVAILABLE"))
			{	
				fprintf(stdout, "INSERISCI BOOK $id $datafine\nINSERISCI BOOK $id $datainizio $datafine\nINSERISCI CANCEL\n");
				send_receive(sockd);
			}
			else
			{	
				fprintf(stdout,"PRENOTAZIONI ATTUALI NON DISPONIBILI\nINSERISCI BOOK $id $datainizio $datafine");
				send_receive(sockd);
			}


		}
		else if(strcmp(buffer,"NAVAILABLE") == 0)
		{
			fprintf(stdout, "PRENOTAZIONI ATTUALI NON DISPONIBILI\nINSERISCI BOOK $id $datainizio $datafine");
			send_receive(sockd);
		}
		else //esco
		{
			shutdown(sockd,2);
			exit(1);
		}

	}
	else if (strcmp(tmp,"AVAILABLE")==0)
	{
		if (strlen(buffer) == 12)
		{
			n = read(sockd,buffer,128);
			printf("%s\n",buffer );
		}
		else
		{
			int b; 
			n = read(sockd,&b,sizeof(b));
			printf("%d\n",b );
		}
		shutdown(sockd,2);
	}
	else if(strcmp(tmp,"CANCEL") == 0)
	{
		n = read(sockd,buffer,128);
		fprintf(stdout, "%s",buffer );
		shutdown(sockd,2);
		exit(0);
	}

	return 0;
}
void send_receive(int sockd)
{
	int n,i;
	char buffer[128] = "", tmp[7] = "";
	fgets(buffer,128,stdin);
	for (i = 0; i < 7 && buffer[i]!='\n' && buffer[i]!=' '; i++)
	{
		tmp[i] = buffer[i];
	}
	n = write(sockd,buffer,strlen(buffer));
	if (strcmp(tmp,"CANCEL")!=0)
	{
		
		memset(buffer,0,128);
		n = read(sockd,buffer,128);
		fprintf(stdout, "%s\n",buffer );

		int a =  strcmp(buffer,"OK\n");
		fprintf(stderr, "%d\n",a );

		if (strcmp(buffer,"OK\n")==0)
		{
			fprintf(stdout, "PRENOTAZIONE EFFETTUATA\n");
		}
		else
		{
			fprintf(stdout, "PRENOTAZIONE NON EFFETTUATA\n");
		}
	}
	shutdown(sockd,2);
	exit(0);
}