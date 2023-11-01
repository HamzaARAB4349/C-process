#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main (int argc, char *argv[]){
	int err,status,p;
	char *nomProgramme = argv[1];
	size_t size = strlen(nomProgramme)+1;		
        char *moduleObjet= (char *)malloc(size);
        strcpy(moduleObjet,nomProgramme);	    
        char *point = strrchr(moduleObjet,'.');
        strcpy(point,".o");
	pid_t p1 = fork();
	if(p1 == 0){/*Processus fils P1*/
	    /*Le processus fils lance la phase de compilation*/
	    err= execlp("gcc","gcc","-c",nomProgramme,NULL);
	    printf("\n Erreur = %d",err);
	    exit(1);
	}
	/*attendre la terminaison du fils P1*/
	p = waitpid(p1,&status,0);
	if((status>>8)!=0){
		/*Test*/
		printf("\nLe processus fils %d n'as pas cree le module objet\n",p1);
		return 2;
	}

	pid_t p2 = fork();
	if(p2 == 0){/*Processus fils P2*/
	    /*Le processus fils P2 lance l'edition des liens*/
	    char *point = strrchr(nomProgramme,'.');
	    strcpy(point,"");
	    err = execlp("gcc","gcc","-o",nomProgramme,moduleObjet,NULL);
     	    printf("\n Erreur = %d",err);
	    exit(1);	    
	}
	/*Attendre la terminaison du fils p2*/
	p= waitpid(p2,&status,0);
	if((status>>8)!=0){
		/*TEST*/
		printf("\nLe processus fils %d n'as pas fait l'édition de lien\n",p2);
		return 3;
	}

	pid_t p3 = fork();
	if(p3==0){/*Processus fils P3*/
		char string[256];
		char *point1 = strrchr(nomProgramme,'.');
	    	strcpy(point1,"");
		sprintf(string,"./%s",nomProgramme);
		/*processus fils P3 execute le programme */
		err = execlp(string,nomProgramme,NULL);
		printf("\n Erreur = %d",err);
		exit(1);
	}
	/*Attendre la terminaison du fils P3*/
	p = waitpid(p3,&status,0);
	if(WIFEXITED(status)){
		printf("P3 est termine normalement (sans erreurs)  avec le code de sortie : %d\n" ,WEXITSTATUS(status));
	}else if(WIFSIGNALED(status)){
		int signal_number =WTERMSIG(status);
		printf("fin anormal de  processus fils a cause de deroutement %d\n",signal_number);
	}else{
		printf("fin normal avec erreurs de precessus P3\n");
	}
	
	return 0;
}
