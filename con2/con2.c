

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>


#define IFLAGS (SEMPERM | IPC_CREAT)
#define SKEY   (key_t) IPC_PRIVATE	
#define SEMPERM 0600

int NbBocalPetit;
int NbBocalGrand, temps;

struct sembuf sem_oper_P = {0,-1,0};
struct sembuf sem_oper_V = {0,1,0};

int initsem(key_t semkey)
{
	int status = 0;
	int semid_init;
	union semun {
		int val;
		short *array;

	}ctl_arg;

	if((semid_init = semget(semkey, 6, IFLAGS))>0)
	{
		short array[6]={1,0,0,0,0,0};
		ctl_arg.array = array;
		status = semctl(semid_init, 0, SETALL, ctl_arg);

	}
	if(semid_init == -1 || status == -1)
	{	
		perror("Erreur initsem");
		return (-1);
	}
	else return (semid_init);

}

int P(int SemId, int semnum)
{
	
	sem_oper_P.sem_num = semnum;

	if(semop(SemId, &sem_oper_P, 1) == -1)
	{
		fprintf(stderr, "semaphore_p failed\n");
		return 0;
	}
	return 1;
}

int V(int SemId, int semnum)
{
	
	sem_oper_V.sem_num = semnum;

        if(semop(SemId, &sem_oper_V, 1) == -1)
	{
		fprintf(stderr, "semaphore_v failed\n");
                return 0;
	}
	return 1;
}

void valve_oper01(int semId)
{
if(!fork()){
//il va reveiller le premier
	
	P(semId,1);
	printf("Ouvre la valve\n");
		
	V(semId,2);
	P(semId,3);//!		

	printf("Ferme la valve\n");
	V(semId,5);
	exit(0);
}
}

void valve_oper02(int semId)
{
if(!fork()){
//il va reveiller le premier

        P(semId,4);
        printf("Ouvre la valve\n");
                           
        V(semId,2);
        P(semId,3);        

        V(semId,2);
	P(semId,3);//deux iterations
			
        printf("Ferme la valve\n");
        V(semId,5);
        exit(0);
}
}

void Horloge(int semId)
{
if(!fork()){

	P(semId,2);
	printf("Il est en train de remplir le bocal\n");
	sleep(temps);
	printf("Le remplissage est terminé\n");
	
	V(semId,3);
	
	exit(0);
}
}


void bocal_petit(int i, int semid)
{
if(!fork()){

      	printf("(Le petit bocal %d arrive...)\n", i);

        P(semid,0);
        printf("--------------------------------------\n");

        printf("Le petit bocal %d est placé\n", i);
        V(semid,1);	

        P(semid,5);
        printf("Le petit bocal %d est enlevé\n", i);

        printf("--------------------------------------\n");
        V(semid,0);
		
	exit(0);	
}
}

void bocal_grand(int i, int semid)
{
if(!fork()){

        printf("(Le grand bocal %d arrive...)\n", i);

        P(semid,0);
        printf("--------------------------------------\n");

        printf("Le grand bocal %d est placé\n", i);
        V(semid,4);

        P(semid,5);
        printf("Le grand bocal %d est enlevé\n", i);

        printf("--------------------------------------\n");
        V(semid,0);

        exit(0);

}
}


int main()
{
	int semid;

        printf("Entrez le nombre des bocaux petits: \n");
        scanf("%d", &NbBocalPetit);

        printf("Entrez le nombre des bocaux grands: \n");
        scanf("%d", &NbBocalGrand);
 
        printf("Entrez le temps du remplissage \n");
        scanf("%d", &temps);

        int NbProcessus = NbBocalPetit*3 + NbBocalGrand*4;

	if((semid = initsem(SKEY)) < 0)
		return(1);

        printf("--------------------Le process commence!--------------------\n");

        for(int i=1; i<NbBocalPetit+1; i++)
        {
                valve_oper01(semid);
                Horloge(semid);
		bocal_petit(i, semid);
        }

        for(int i2=1; i2<NbBocalGrand+1; i2++)
        {
                valve_oper02(semid);
                Horloge(semid);
		Horloge(semid);
		bocal_grand(i2, semid);
        }

	for(int k=1; k<NbProcessus+1; k++)
        {
                wait(0);
        }
	printf("Tous les bocaux sont remplis\n");
        printf("--------------------Le process se termine!--------------------\n");

	semctl(semid, 0, IPC_RMID);//faire attention!!!
	
	return 0;

}

