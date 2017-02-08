#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS     5

int contadorGlobal;

void *PrintHello(void *threadid)
{
   long tid;
   long *contador;

   tid = *(long *)threadid;
   printf("Thread #%ld alive\n", tid);
   contador = (long *) threadid;
   printf("Thread #%ld: contador inicializado\n", tid);
   printf("Thread #%ld: contador[0]=%ld\n", tid, contador[0]);
   printf("Thread #%ld: contador[1]=%ld\n", tid, contador[1]);
   contadorGlobal++;
   contador[1]++;
   printf("Thread #%ld: contadorGlobal=%d, contadorLocal=%ld\n", tid, contadorGlobal, contador[1]);
   pthread_exit(NULL);
}

int main (/*int argc, char *argv[]*/)
{
   /* int contadorLocal; */

   contadorGlobal = 2000;
   pthread_t threads[NUM_THREADS];
   int rc;
   long t[2];

   t[1] = 100;
   for(t[0]=0; t[0]<NUM_THREADS; t[0]++){
      printf("In main: creating thread %ld\n", t[0]);
      rc = pthread_create(&threads[t[0]], NULL, PrintHello, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
      printf("In main: thread %ld created OK\n\n", t[0]);
   }
   printf("Main: Contador local antes de pthread_exit = %ld\n\n", t[1]);
   /* Last thing that main() should do */
   pthread_exit(NULL);
}
