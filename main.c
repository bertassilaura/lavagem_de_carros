// compilar com gcc main.c -o main
// executar com /.main

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

// Defines

#define QTD_CLIENTES 4
#define LAVAGEM 5
#define ENTRANDO 5 
#define POSICIONANDO 5

// Variáveis globais

int vagas = 3;
int clientes = 0;
pthread_mutex_t mutex;
sem_t cliente;
sem_t funcionario;
sem_t cliente_pronto;
sem_t funcionario_pronto;

// Cliente

void *cliente_funcao(void *arg){
  int *n;
  n = (int)arg;
  entra_loja(n);
  sem_wait(&funcionario);
  carro_em_lavagem(n);
  sai_loja(n);
}

void entra_loja(int n){
  bool entrou = false;
  sleep(rand() % ENTRANDO);
  pthread_mutex_lock(&mutex);
  if(clientes < vagas){
    clientes++;
    entrou = true;
  }
  pthread_mutex_unlock(&mutex);
  if(entrou == false){
    desiste(n);
  }
  printf("Cliente %d entrou na loja!\n", n);
  sem_post(&cliente);
}

void carro_em_lavagem(int n){
  sleep(rand() % POSICIONANDO);
  printf("Cliente %d posicionou seu carro para lavagem\n", n);
  sem_post(&cliente_pronto);
  sem_wait(&funcionario_pronto);
}

void desiste(int n){
  printf("Cliente %d desistiu e saiu da loja\n", n);
  pthread_exit(NULL);
}

void sai_loja(int n){
  pthread_mutex_lock(&mutex);
  clientes--;
  pthread_mutex_unlock(&mutex);
  printf("Cliente %d teve seu carro lavado e saiu da loja\n", n);
  pthread_exit(NULL);
}

// Funcionário

void *funcionario_funcao(void *arg){
  printf("Funcionário chegou!\n");
  while(true){
    sem_post(&funcionario);
    printf("Funcionário está pronto para atender!\n");
    if (sem_trywait(&cliente) == -1) {
        descansar();
	      sem_wait(&cliente);
        printf("Funcionário saiu do descanso!\n");
      }
    lavando_carro();
    }
}

void descansar(){
  printf("Funcionário está descansando!\n");
}

void lavando_carro(){
  sem_wait(&cliente_pronto);
  sleep(rand() % LAVAGEM);
  printf("Funcionário lavou um carro\n");
  sem_post(&funcionario_pronto);
};

// Main

int main(int argc, char *argv[]){

  // Variáveis locais
  pthread_t thread_clientes[QTD_CLIENTES];
  pthread_t thread_funcionario;
  int i;

  // Inicia mutex e semáforos

  pthread_mutex_init(&mutex, NULL);
  sem_init(&cliente, 0, 0);
  sem_init(&funcionario, 0, 0);
  sem_init(&cliente_pronto, 0, 0);
  sem_init(&funcionario_pronto, 0, 0);

  // Cria Threads
  
  pthread_create(&thread_funcionario, NULL, funcionario_funcao,NULL); 
 
  for(i = 0; i < QTD_CLIENTES; i++){
    pthread_create(&thread_clientes[i], NULL, cliente_funcao, (void *)i); 
  }
  
  // Espera finalização de todos clientes

  for(i=0; i < QTD_CLIENTES; i++){
    pthread_join(thread_clientes[i], NULL);
  }

  // Destruição de mutex e semáforos

  pthread_mutex_destroy(&mutex); 
  sem_destroy(&cliente);
  sem_destroy(&funcionario);
  sem_destroy(&cliente_pronto);
  sem_destroy(&funcionario_pronto);

  // Finalização

  printf("Não há mais clientes\n");
  return 0;
}
