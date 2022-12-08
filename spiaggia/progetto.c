#include "prototipazioni.h"
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <unistd.h>
#define M 20
void future_pren(int, char*,int);
prenotazioni spiaggia[N];
pthread_mutex_t mutexarray;
pthread_t workers[M];
void *worker ()
{
	int sockd = socket(AF_INET,SOCK_STREAM,0);
	int port = 6969;
	char buffer[128]="", tmp[10] = "", idombrello[3] = "";
	printf("Creazione socket\n");
	struct sockaddr_in server;

	bzero((char *) &server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	// associa dominio,protocollo, tipo all'indirizzo e numero di porta
	bind(sockd,(struct sockaddr *) &server, sizeof(server));
	//numero massimo di connessioni
	int limit = 1;
	int n,i;
	//è in ascolto sul socket, qualora ci sia una connessione pendente la accetta
	listen(sockd,limit);

	struct sockaddr_in client;
	socklen_t cli;
	cli = sizeof(client);

	//accetta la connessione
	int newsocket;
	newsocket = accept(sockd, (struct sockaddr*) &client, &cli); 
	n = read(newsocket, buffer, 128);
	fprintf(stdout, "%s\n",buffer);
	for(i = 0; i < 10 && buffer[i] != ' ' && buffer[i]!='\n' && buffer[i]!='\0'; i++)
	{
		tmp[i] = buffer[i];
	}
////////////////////////////////////////////////////////// potrebbe dare problemi qua
	fprintf(stdout,"%s\n",tmp);
	//qualora ricevo BOOK
	//int as = strcmp(tmp,"BOOK");
	//printf("%d\n",as );
	int base;
	if(strcmp(tmp,"BOOK") == 0)
	{
		bool v = false;
		//cerco se ci sono ombrelloni con stato 0
		pthread_mutex_lock(&mutexarray);
		for(i = 0; i < N && v==false; i++)
		{
			if(spiaggia[i].ombrello.stato == 0)
			{
				v = true;
			}
		}
		//printf("%d\n",v );
		//se ci sono ombrelloni con stato 0 allora mi permette qualsiasi tipo di prenotazione
		//attuale e non
		if(v)
		{	
			n = write(newsocket,"OK\n",3);
			//non so se sia necessario quando implementiamo i thread, dato che ho variabili globali e locali al thread
			//leggo l'invio del tipo BOOK $id
			n = read(newsocket, buffer, 128);

			tmp[0] = buffer[5];
			tmp[1] = buffer[6];
			//tmp contiene ora il numeo dell'ombrello prenotato(o in considerazione)
			
			switch(tmp[0])
			{
				case '1': base = 0;break;
				case '2': base = 26;break;
				case '3': base = 52;break;
				case '4': base = 78;break;
				case '5': base = 104;break;
				case '6': base = 130;break;
				case '7': base = 156;break;
				case '8': base = 182;break;
				case '9': base = 208;break;
			}
			base += tmp[1] - 97;
			//base contiene ora l'indice della cella dell'ombrellone con l'id ricevuto
			if(spiaggia[base].ombrello.stato == 0)
			{
				
				spiaggia[base].ombrello.stato = 2;//setto l'ombrellone come temporaneamente prenotato(forse posso già rilasciare il mutex)
				pthread_mutex_unlock(&mutexarray);
				n = write(newsocket,"AVAILABLE\n",10);
				memset(buffer,0,128);
				n = read(newsocket, buffer, 128);
				fprintf(stdout, "%s\n",buffer);
				int len = strlen(buffer);
				//printf("%d\n",len );
				//se risponde CANCEL cancello la prenotazione
				if(strcmp(buffer,"CANCEL\n") == 0)
				{
					spiaggia[base].ombrello.stato = 0;
				}
				else if(strlen(buffer)==19)//se risponde con una cosa come BOOK $id $datafine
				{
					spiaggia[base].ombrello.stato = 1;
					for (i = 0; i < 10; i++)
					{
						tmp[i] = buffer[i+8];
					}
					spiaggia[base].ombrello.data_scadenza = string_to_date(tmp);
					n = write(newsocket,"OK\n",3);
				}
				else //se risponde con BOOK $id $datainizio $datafine
				{
					spiaggia[base].ombrello.stato = 0;//setto a zero dato che il giorno corrente non è occupato
					future_pren(newsocket,buffer,base);
				}
			}
			else
			{
				pthread_mutex_unlock(&mutexarray);
				n = write(newsocket,"NAVAILABLE\n",11);
				n = read(newsocket, buffer, 128);
				future_pren(newsocket,buffer,base);	
			}

		}
		else
		{
			pthread_mutex_unlock(&mutexarray);
			n = write(newsocket,"NAVAILABLE\n",11);
			n = read(newsocket, buffer, 128);
			future_pren(newsocket,buffer,base);				
		}
		 //shutdown(sockd,2);
	}
	else if(strcmp(tmp,"CANCEL") == 0)
	{
		idombrello[0] = buffer[7];
		idombrello[1] = buffer[8];
		switch(idombrello[0])
		{
			case '1': base = 0;break;
			case '2': base = 26;break;
			case '3': base = 52;break;
			case '4': base = 78;break;
			case '5': base = 104;break;
			case '6': base = 130;break;
			case '7': base = 156;break;
			case '8': base = 182;break;
			case '9': base = 208;break;
		}
		base += idombrello[1] - 97;
		printf("%d\n",base);
		if (strlen(buffer) == 32)//CANCELLAZIONI FUTURE CANCEL $id $datainizio $datafine
		{
			//printf("FUTURE CANCEL\n");
			char tmp1[10]= "";
			for (i = 0; i < 10; i++)
			{
				tmp[i] = buffer[i+10];
				tmp1[i] = buffer[i+21];
			}
			list_data *d = (list_data*)malloc(sizeof(list_data));
			list_data *h = (list_data*)malloc(sizeof(list_data));
			d->data_inizio = string_to_date(tmp);
			d->data_fine = string_to_date(tmp1);
			//visualizza_date(spiaggia[base].data_prenotaz);
			pthread_mutex_lock(&spiaggia[base].semaforo);
			h =eliminazione_lista(&spiaggia[base].data_prenotaz, d->data_inizio, d->data_fine);
			if (h == NULL)
			{
				write(newsocket, "NOK\n",4);
			}
			else
			{
	//			sem_wait(&spiaggia[base].semaforo);
				if (h->data_inizio.giorno == 0 && h->data_inizio.mese == 0 && h->data_inizio.anno == 0)
				{
					spiaggia[base].data_prenotaz = NULL;
				}
				else
				{
					spiaggia[base].data_prenotaz = h;
				}
	//			sem_post(&spiaggia[base].semaforo);
				write(newsocket, "CANCEL OK\n",10);	
			}
			pthread_mutex_unlock(&spiaggia[base].semaforo);

			//shutdown(newsocket,2);
		}
		else// CANCELLAZIONI PRENOTAZIONI ATTUALI CANCEL $id
		{
			pthread_mutex_lock(&mutexarray);
			if (spiaggia[base].ombrello.stato == 0)
			{
				n = write(newsocket,"ALREADY FREE\n",13);
			}
			else
			{
				spiaggia[base].ombrello.stato = 0;
				n = write(newsocket,"CANCEL OK\n",10);
			}
			pthread_mutex_unlock(&mutexarray);
			shutdown(newsocket,2);
		}
	}
	else if(strcmp(tmp,"AVAILABLE") == 0)
	{
		int num = 0;
		if(strlen(buffer) == 10)//chiedo il numero dei disponibili
		{
			pthread_mutex_lock(&mutexarray);
			for (i = 0; i < N; i++)
			{
				if (spiaggia[i].ombrello.stato == 0)
				{
					num++;
				}
			}
			if(num > 0)
			{
				printf("%d\n",num );
				write(newsocket,&num,sizeof(num));

			}
			else
			{
				write(newsocket,"NAVAILABLE\n",11);
			}
			pthread_mutex_unlock(&mutexarray);
		}
		else // chiedo quali disponibili
		{
			int a = (int)buffer[10] - 49;
			char string1[128] = "";
			pthread_mutex_lock(&mutexarray);
			for (i = 0; i < 26; i++)
			 {
			 	if(spiaggia[a*26 + i].ombrello.stato == 0)
			 	{
			 		strcat(string1, spiaggia[a*26 + i].ombrello.numero_ombrello);
			 		strcat(string1, " ");
			 	}
			 	
			 } 
			 n = write(newsocket,string1,strlen(string1));
			 pthread_mutex_unlock(&mutexarray);
		}
		//shutdown(newsocket,2);
	}

}
int main(int argc, char const *argv[])
{
	int i;
	int base;
	//sem_init(&mutexarray,0,1);
	//operazioni del thread master master
	FILE *f = fopen("file_salvataggio", "r");
	lettura_file(f,spiaggia);
	fclose(f);
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	for(i = 0; i < N; i++)
	{
		pthread_mutex_init(&spiaggia[i].semaforo,&attr);
	}
	pthread_mutex_init(&mutexarray, &attr);
	for (i = 0; i < M; ++i)
	{
		pthread_create(&workers[i],NULL, worker, NULL);
	}
	

	//
	//operazioni threads worker
	
	//visualizza_date(spiaggia[0].data_prenotaz);
	//int err = shutdown(newsocket, 2);
   // fprintf(stdout, "%d\n",err );
    scrittura_file(spiaggia);
	//instaura la connessione tra client e server, sul socket(indirizzo ip, porta e protocollo), specificando (e salvando)
	//il client connesso e la lunghezza della struttura associata


	//la chiusura del socket non mi permette immediatamente 
	//di crearne un altro(lato server), dato che durante la 
	//chiusura si ha una chiamata ad una wait per evitare che pacchetti 
	//ritardati nella prima connessione vengano consegnati alla seconda connessione  



	/* SETTO UNA VARIABILE DA CONSIDERARSI ODIERNA.
	data def; 
	def.anno = 2017;
	def.mese = 5;
	def.giorno = 31;	
	*/
	
	return 0;
}
void future_pren(int sockfd, char *buffer, int base)
{
	int n,i;
	data datai, dataf;
	char tmp[11] = "";
	char tmp1[11] = "";
	for (i = 0; i < 10; i++)
	{
		tmp[i] = buffer[i+8];
		tmp1[i] = buffer[i+19];
	}
	datai = string_to_date(tmp);
//	printf("%s\n",tmp1 );
	dataf = string_to_date(tmp1);
	list_data *d = (list_data*) malloc(sizeof(list_data));
	list_data *h = (list_data*) malloc(sizeof(list_data));
	d->data_inizio = datai;
	d->data_fine = dataf;
	pthread_mutex_lock(&spiaggia[base].semaforo);
	h = inserimento_lista1(&spiaggia[base].data_prenotaz,&d);
	if (h == NULL)//inserisco in una data non possibile(mi manca ancora il controllo con la data attuale)
	{
		n = write(sockfd,"NOK\n",4);	
	}
	else
	{
		spiaggia[base].data_prenotaz = h;
		n = write(sockfd,"OK\n",3);
	}
	pthread_mutex_unlock(&spiaggia[base].semaforo);
}
bool inserimento_lista(list_data **head, list_data **ins)
{	
	printf("controllo inserimento\n");
	list_data *tmp = *head;
	list_data *prec;
	if((*head)->data_inizio.giorno == 0)/* confronto con 0, dato che per comodità non esiste e di conseguenza tali valori sono sostituiti*/
	{
		(*head)->data_inizio.giorno = (*ins)->data_inizio.giorno;
		(*head)->data_fine.giorno = (*ins)->data_fine.giorno;
		(*head)->data_inizio.mese = (*ins)->data_inizio.mese;
		(*head)->data_fine.mese = (*ins)->data_fine.mese;
		(*head)->data_inizio.anno = (*ins)->data_inizio.anno;
		(*head)->data_fine.anno = (*ins)->data_fine.anno;
		(*head)->next = NULL;
		return true;
	}
	else if( confronto_date((*ins)->data_inizio,(*head)->data_inizio)==0 )
	{
		bool v = controllo_intervallo((*head)->data_inizio,(*head)->data_fine, (*ins)->data_inizio, (*ins)->data_fine);	
		if(!v)   		
		{					
			return false;	
		}
		(*ins)->next = (*head)->next;
		(*head)->next = (*ins);
		data temp_i,temp_f;
		temp_i = (*head)->data_inizio;
		temp_f = (*head)->data_fine;
		(*head)->data_inizio = (*ins)->data_inizio;
		(*head)->data_fine = (*ins)->data_fine;
		(*ins)->data_inizio = temp_i;
		(*ins)->data_fine = temp_f;					

		return true;
	}

	while(confronto_date(tmp->data_inizio,(*ins)->data_inizio)==0)
	{
		printf("controllo while\n");
		prec = tmp;
		tmp = tmp->next;
	}

	bool v = controllo_intervallo(prec->data_inizio,prec->data_fine, (*ins)->data_inizio, (*ins)->data_fine);
	bool v1 = controllo_intervallo((*ins)->data_inizio, (*ins)->data_fine,tmp->data_inizio, tmp->data_fine);
	if( v == false || v1== false )
		{ return false;}

	prec->next = *ins;
	(*ins)->next = tmp;
	return true;

}

list_data *inserimento_lista1(list_data **head, list_data **ins)
{	
	//printf("controllo inserimento1\n");
	if((*head)== NULL)/* confronto con 0, dato che per comodità non esiste e di conseguenza tali valori sono sostituiti*/
	{
		//printf("head = NULL\n");
		(*ins)->next = NULL;
		return *ins;
	}
	else if( confronto_date((*ins)->data_inizio,(*head)->data_inizio)==0 )
	{
		bool v = controllo_intervallo((*head)->data_inizio,(*head)->data_fine, (*ins)->data_inizio, (*ins)->data_fine);	
		if(!v)   		
		{					
			return NULL;	
		}
		(*ins)->next = (*head);
		return *ins;
	}
	list_data *tmp = *head;
	list_data *prec;
	while(tmp->next!=NULL && confronto_date(tmp->data_inizio,(*ins)->data_inizio)==0)
	{
		//printf("controllo while\n");
		prec = tmp;
		tmp = tmp->next;
	}

	bool v = controllo_intervallo(prec->data_inizio,prec->data_fine, (*ins)->data_inizio, (*ins)->data_fine);
	bool v1 = controllo_intervallo((*ins)->data_inizio, (*ins)->data_fine,tmp->data_inizio, tmp->data_fine);
	if( v == false || v1== false )
		{ return NULL;}

	prec->next = *ins;
	(*ins)->next = tmp;
	return *head;

}
list_data *eliminazione_lista(list_data **head, data datai, data dataf)
{
//	printf("LLLLL\n");
	//visualizza_date((*head));
	list_data *tmp = (list_data*)malloc (sizeof(list_data));
	tmp = (*head);
	//printf("LLLLL\n");
	list_data *prec =(list_data*)malloc (sizeof(list_data));
	prec = *head;
	printf("%d\n", confronto_date(tmp->data_inizio,datai));
	printf("%d\n", confronto_date(tmp->data_fine,dataf));
	if(confronto_date((*head)->data_inizio,datai) == 2 && confronto_date((*head)->data_fine,dataf) == 2)
	{
	//	printf("ENtro qua\n");
		(*head) = (*head)->next;
		if(head == NULL)
		{
			list_data *ret = (list_data*)malloc(sizeof(list_data));
			ret->data_inizio.giorno = 0;
			ret->data_inizio.mese = 0;
			ret->data_inizio.anno = 0;
			return ret;
		}
		free(tmp);
		return *head;
	}
	//printf("NON ENTRA\n");
	tmp = (*head)->next;
	while(tmp != NULL)
	{	
		if( confronto_date(tmp->data_inizio,datai) == 2 && confronto_date(tmp->data_fine,dataf) == 2)
		{
			prec->next = tmp->next;
			free(tmp);////verificare quando ho un solo elemento che verrà  cancellato
			return *head;
		}
		prec = tmp;
		tmp = tmp->next;
	}	
}
void lettura_file(FILE *fp, prenotazioni *spiaggia){
	int i = 0;
	int j;
	int tmpday, tmpmonth, tmpyear, tmp;
	char string[11]= "";
	for(i = 0; i < N; i++)
	{
		//printf("ombrello %d\n",i );
		fscanf(fp, "%s", spiaggia[i].ombrello.numero_ombrello);
		fscanf(fp, "%hd", &spiaggia[i].ombrello.stato);
		/*printf("%s\n", spiaggia[i].ombrello.numero_ombrello);
		printf("%hd\n", spiaggia[i].ombrello.stato);*/
		if (spiaggia[i].ombrello.stato==1)
		{
			fscanf(fp, "%s", string);
			spiaggia[i].ombrello.data_scadenza = string_to_date(string);
			
		}

		fscanf(fp, "%s", string);
		//printf("%s\n",string );
		bool first_time = true;
		list_data *head = NULL;
		list_data *tmp = NULL; 
		list_data *a = NULL;
		while(strcmp(string,"#") != 0)
		{
			tmp = (list_data*) malloc(sizeof(list_data));
			tmp->data_inizio = string_to_date(string);

			fscanf(fp, "%s", string);

			tmp->data_fine = string_to_date(string);
			tmp->next = NULL;

			if(first_time)
			{
				a = inserimento_lista1(&head,&tmp);
				head = a;
			}
			else
			{
				inserimento_lista1(&a,&tmp);
				a = tmp;
			}
			fscanf(fp, "%s", string);
		}
		spiaggia[i].data_prenotaz = head;
	}
}
void scrittura_file(prenotazioni *spiaggia)
{
	FILE *fp;
	fp = fopen("file_salvataggio", "w");
	list_data *tmp = (list_data*) malloc(sizeof(list_data));
	if (fp == NULL)
	{
		printf("ERROR\n");
	}
	else
	{
		int i= 0;
		for (i = 0; i < N; i++ )
		{
			//printf("A\n");
			fprintf(fp,"%c%c\n",spiaggia[i].ombrello.numero_ombrello[0], spiaggia[i].ombrello.numero_ombrello[1]); 
			fprintf(fp,"%d\n",spiaggia[i].ombrello.stato);

			if (spiaggia[i].ombrello.stato)
			{
				fprintf(fp,"%s\n",addzeros(spiaggia[i].ombrello.data_scadenza));			
			}
			
			tmp = spiaggia[i].data_prenotaz;
			while(tmp != NULL)
			{
				printf("B\n");
				fprintf(fp,"%s\n",addzeros(tmp->data_inizio));
				fprintf(fp,"%s\n",addzeros(tmp->data_fine));
				tmp = tmp->next;

			}
			//fprintf(fp,"%s\n",addzeros(spiaggia[i].data_prenotaz->data_inizio));
			//fprintf(fp,"%s\n",addzeros(spiaggia[i].data_prenotaz->data_fine));
			fprintf(fp, "#\n");
		}
		fprintf(fp, "###");

		fclose(fp);
	}
	free(tmp);
	
} 
void inizialize()
{
	FILE *fp;
	int i;
	char c,d;
	fp = fopen("start.txt","w");

	for (i = 0; i < N; i++)
	{
		d = i%26 + 97;
		c = i/26 + 49;
		//printf("%c%c\n",c,d );
		fprintf(fp, "%c%c\n",c,d );
		fprintf(fp, "0\n#\n" );
	}
	fprintf(fp, "###" );
	fclose(fp);
}
char* addzeros(data d)
{
	char *string = malloc(sizeof (char)*11);
	char year[5];
	sprintf(year,"%d",d.anno);
	if (d.giorno/10 == 0)
	{
		string[0] = '0';
	}
	else
	{
		string[0] = (d.giorno/10+48);
	}
	string[1] = d.giorno%10+48;
	string[2] = '-';
	string[5] = '-';
	if (d.mese/10 == 0)
	{
		string[3] = '0';
	}
	else
	{
		string[3] = d.mese/10+48; 
	}
	string[4] = d.mese%10+48;
	strcat(string,year);
	return string;
}