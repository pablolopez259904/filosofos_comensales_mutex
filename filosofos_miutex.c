#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


#define NUM_FILOSOFOS 100
int N2=0; /*numero de lineas para el nombre de filosofos */
char *NombreFilosofo2[NUM_FILOSOFOS]; //OBTENER LOS NOMBRES DE LOS FILOSOFOS

typedef enum {
	pensando = 0, esperando, comiendo
} estado;

bool palillo[NUM_FILOSOFOS] = { false };//Controlamos si he podido coger tenedores o no, si no he podido, me bloqueo hasta que nos despierte el filosofo de la izqda o dcha
pthread_cond_t espera[NUM_FILOSOFOS];
pthread_mutex_t mutex; //Mutex: Solo un filosofo puede coger o soltar tenedores;
estado filosofo_estado[NUM_FILOSOFOS] = { pensando }; // definimos en que estado esta el filosofo

void pensar(int id); //mandamos a pensar los filosofos
void comer(int id); //mandamos a comer a los filosofos
void dormir(int id); //mandamos a dormir a los filosofos
void nombre(); //obtener el nombre del filosofo
char *read_archivo(void); //leer el archivo

char *read_archivo(void){
    static char linea[1000000];
    FILE *fich; 
    fich = fopen("nombres_filosofos.txt", "r");
    while (fgets(linea, 1000000, (FILE*)fich)); 
    fclose(fich);

    return linea;
}

void termino_comer(int id){
	 /*OBTENEMOS LA HORA*/
    time_t t;
    struct tm *tm;
    char hora[100];
    char hora2[100];
    t=time(NULL);
    tm=localtime(&t);
    strftime(hora, 100, "%H:%M:%S", tm);
    strftime(hora2, 100, "%H", tm);
    int ls=atof(hora2);
    if(ls >= 12){
        stpcpy(hora2,"PM");
    }else
    {
         stpcpy(hora2,"AM");
    }
    /*--------------------------------------*/

	printf("%s %s Filosofo %s termino de comer\n",hora,hora2,NombreFilosofo2[id]);
    FILE *filePointer;
    filePointer = fopen("salida.txt", "a");
    fprintf(filePointer, "%s %s Filosofo %s termino de comer\n",hora,hora2,NombreFilosofo2[id]);
    sleep (random() % 10);
    fclose(filePointer);
}

void nombre(){
      //NombreFilosofo = igualo a la funcion de read_archivo
    //NombreFilosofo1 = aqui tomo las posiciones cuando le quito las comas
    //NombreFilosofo2 = meto la array que saque con NombreFilosofo1 y lo coloca en una sola posicion
    char *NombreFilosofo=read_archivo();

    //saco el primer valor del array que este separado por coma
    char *NombreFilosofo1 = strtok (NombreFilosofo,",");
    for(int i=0; i < N2 ; i++)
    {
        NombreFilosofo2[i]=NombreFilosofo1;
        //saco el valor del array que este separado por coma
        NombreFilosofo1 = strtok (NULL,",");
    }

}
void pensar(int id)
{
    /*OBTENEMOS LA HORA*/
    time_t t;
    struct tm *tm;
    char hora[100];
    char hora2[100];
    t=time(NULL);
    tm=localtime(&t);
    strftime(hora, 100, "%H:%M:%S", tm);
    strftime(hora2, 100, "%H", tm);
    int ls=atof(hora2);
    if(ls >= 12){
        stpcpy(hora2,"PM");
    }else
    {
         stpcpy(hora2,"AM");
    }
    /*--------------------------------------*/
    
    printf("%s %s Filosofo %s pensando\n",hora,hora2,NombreFilosofo2[id]);

	sleep (random() % 10);
    
}

void dormir(int id)
{
    /*OBTENEMOS LA HORA*/
    time_t t;
    struct tm *tm;
    char hora[100];
    char hora2[100];
    t=time(NULL);
    tm=localtime(&t);
    strftime(hora, 100, "%H:%M:%S", tm);
    strftime(hora2, 100, "%H", tm);
    int ls=atof(hora2);
    if(ls >= 12){
        stpcpy(hora2,"PM");
    }else
    {
         stpcpy(hora2,"AM");
    }
    /*--------------------------------------*/

    printf("%s %s Filosofo %s durmiendo\n",hora,hora2,NombreFilosofo2[id]);
	sleep (random() % 10);

}

void comer(int id)
{
    /*OBTENEMOS LA HORA*/
    time_t t;
    struct tm *tm;
    char hora[100];
    char hora2[100];
    t=time(NULL);
    tm=localtime(&t);
    strftime(hora, 100, "%H:%M:%S", tm);
    strftime(hora2, 100, "%H", tm);
    int ls=atof(hora2);
    if(ls >= 12){
        stpcpy(hora2,"PM");
    }else
    {
         stpcpy(hora2,"AM");
    }
    /*--------------------------------------*/

    printf("%s %s Filosofo %s esta comiendo\n",hora,hora2,NombreFilosofo2[id]);
    
    FILE *filePointer;
    filePointer = fopen("salida.txt", "a");
    fprintf(filePointer, "%s %s Filosofo %s esta comiendo\n",hora,hora2,NombreFilosofo2[id]);
    fclose(filePointer);
    sleep (random() % 10);
}


void * Filosofo(void * arg) {
	int filosofo = *((int *)arg);
		
		filosofo_estado[filosofo] = pensando;
		
		/*imprimo el mensaje que esta pensando*/
		pensar(filosofo);
		

		/* filosofo esta esperando" */
		filosofo_estado[filosofo] = esperando;

		pthread_mutex_lock(&mutex);
		while (palillo[filosofo] || palillo[(filosofo + 1) % N2])
			pthread_cond_wait(&espera[filosofo], &mutex);

		/* seccion critica: el filosofo puede tomar los palillos */
		palillo[filosofo] = true;
		palillo[(filosofo + 1) % N2] = true;
		pthread_mutex_unlock(&mutex);

		/* Filosofo ya tiene el control de los palillo*/
		filosofo_estado[filosofo] = comiendo;
		comer(filosofo);

		/* Soltar los palillos */

		/* seccion critica */
		pthread_mutex_lock(&mutex);
		palillo[filosofo] = false;
		palillo[(filosofo + 1) % N2] = false;
		
		/*Indicamos que el filosofo termino de comer y solo va poder ingresar un solo filosofo*/
		termino_comer(filosofo);
		

		/* notifica al filoso anterior y filosofo posterior */
		pthread_cond_signal(&espera[(filosofo - 1 + N2) % N2]);
		pthread_cond_signal(&espera[(filosofo + 1) % N2]);
		pthread_mutex_unlock(&mutex);

		/*Mandamos a dormir a los filosofo*/
		dormir(filosofo);
}

/*--------------------------------------------MAIN-------------------------------------------*/
int main(int argc, char *argv[])
{
	int i;
	int filosofo_ids[N2];
	N2=atoi(argv[1]);
    nombre();

	pthread_mutex_init(&mutex, NULL);
	
	while(true){
		pthread_t filosofo[N2];
		for (i = 0; i < N2; i++) {
			filosofo_ids[i] = i;
			pthread_cond_init(&espera[i], NULL);
			pthread_create(&filosofo[i], NULL, Filosofo, &filosofo_ids[i]);
		}
		for (i = 0; i < N2; i++) {
			pthread_join(filosofo[i], NULL);
			pthread_cond_destroy(&espera[i]);
		}
	}

	pthread_mutex_destroy(&mutex);
	return 0;
} 