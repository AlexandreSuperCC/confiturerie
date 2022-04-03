
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int NbPetit, NbGrand, temps;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_forall = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t valve = PTHREAD_COND_INITIALIZER;
pthread_cond_t valve_2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t horloge = PTHREAD_COND_INITIALIZER;
pthread_cond_t fermer = PTHREAD_COND_INITIALIZER;
pthread_cond_t enleve = PTHREAD_COND_INITIALIZER;

void oper_valve()
{
	pthread_mutex_lock(&mutex);	

	pthread_cond_wait(&valve, &mutex);

	printf("Ouvre la valve\n");
	
	pthread_cond_signal(&horloge);

	pthread_cond_wait(&fermer, &mutex);
	printf("Ferme la valve\n");
	
	pthread_cond_signal(&enleve);
	pthread_mutex_unlock(&mutex);
}
void oper_valve_2()
{
        pthread_mutex_lock(&mutex);

	pthread_cond_wait(&valve_2, &mutex);

       	printf("Ouvre la valve\n");

        pthread_cond_signal(&horloge);
        pthread_cond_wait(&fermer, &mutex);
        pthread_cond_signal(&horloge);
        pthread_cond_wait(&fermer, &mutex);
 
        printf("Ferme la valve\n");
        
        pthread_cond_signal(&enleve);
        pthread_mutex_unlock(&mutex);
}

void oper_horloge()
{
	pthread_mutex_lock(&mutex);

	pthread_cond_wait(&horloge, &mutex);
	printf("Il est en train de remplir le bocal\n");
        sleep(temps);
        printf("Le remplissage est terminé\n");

	pthread_cond_signal(&fermer);

	pthread_mutex_unlock(&mutex);
}

void oper_bocal(int i)
{
        pthread_mutex_lock(&mutex_forall);

	pthread_mutex_lock(&mutex);
	printf("--------------------------------------\n");

	if(i<NbPetit+1){	
		
		printf("Le petit bocal %d arrive\n", i);

	        printf("Le petit bocal %d est placé\n", i);	

                pthread_cond_signal(&valve);                
		pthread_cond_wait(&enleve, &mutex);
				
		printf("Le petit bocal %d est enlevé\n", i);
      	
		printf("--------------------------------------\n");
		pthread_mutex_unlock(&mutex);
      		pthread_mutex_unlock(&mutex_forall);
	}

	else{
		
		i=i-NbPetit;

	        printf("Le grand bocal %d arrive\n", i);

		printf("Le grand bocal %d est placé\n", i);

                pthread_cond_signal(&valve_2);
                pthread_cond_wait(&enleve, &mutex);

                printf("Le grand bocal %d est enlevé\n", i);

		printf("--------------------------------------\n");
        	pthread_mutex_unlock(&mutex);
	        pthread_mutex_unlock(&mutex_forall);
		
	}
}

 
void * fonc_valve()
{
while(1){
	oper_valve();
}
}

void * fonc_valve_2()
{
while(1){
        oper_valve_2();
}
}

void * fonc_horloge()
{
while(1){
        oper_horloge();
}
}

void * fonc_bocal(void *i)
{
	oper_bocal((int) i);
}


int main()
{
	int err, num1, num;

        printf("Entrez le nombre des bocaux petits: \n");
        scanf("%d", &NbPetit);

        printf("Entrez le nombre des bocaux grands: \n");
        scanf("%d", &NbGrand);
 
        printf("Entrez le temps du remplissage \n");
        scanf("%d", &temps);

	int NbTh = NbGrand + NbPetit;
	pthread_t tid[NbTh + 1];

        err = pthread_create(tid+NbTh+2, 0, (void *)fonc_horloge, NULL);
        err = pthread_create(tid+NbTh, 0, (void *)fonc_valve, NULL);
        err = pthread_create(tid+NbTh+1, 0, (void *)fonc_valve_2, NULL);
        usleep(2000);

        printf("--------------------Le process commence!--------------------\n");

	for(num1=1;num1<NbTh+1;num1 ++)
		err = pthread_create(tid+num1-1, 0, (void *)fonc_bocal, (void *)num1);

	if(err != 0)
	{
		printf("On ne peut pas créer le thread: %s\n",strerror(err));
		return 1;
	}

	//attend la fin de toutes les threads Bocal
	for(num=0;num<NbTh;num ++)
		pthread_join(tid[num],NULL);

	printf("Tous les bocaux sont remplis\n");
        printf("--------------------Le process se termine!--------------------\n");

	/* liberation des ressources");*/
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&valve);
	pthread_cond_destroy(&valve_2);
        pthread_cond_destroy(&horloge);
        pthread_cond_destroy(&fermer);
	 
	exit(0);
}	
	

