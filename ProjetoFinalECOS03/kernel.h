#ifndef KERNEL_H
#define KERNEL_H

//libs utilizadas
#include <stdio.h>
#include <stdlib.h>

//variaveis de controle
#define SUCCESS 0
#define FAIL 1
#define REPEAT 2

//limites de operação
#define MAX_PROCESSES 50
#define MAX_QUEUE_SIZE 15
#define MAX_NUM_QUEUES 3

//estrutura do ponteiro de função
typedef char (*ptrFunc)(void);

//structs referentes aos processos, filas e ao escalonador
typedef struct {
  int id;
  int priority;
  int time_left;
  int running;
  int done;
} Process;

typedef struct {
    Process buffer[MAX_QUEUE_SIZE];
    int head;
    int tail;
}Queue;

typedef struct {
    Queue *queues;
    Process *p_disponiveis;
    int num_queues;
    ptrFunc exec;
} Scheduler;

//funções utilizadas
char priorityScheduling(void);
char multilevelQueuePriority(void);
char kernelInit(void);
char kernelAddProc();
char kernelRemoveProc();
void kernelLoop(void);
char kernelDequeue(Queue *q, int index);
char kernelEnqueue(Queue *q, int index);
char exec(void);
char kernelAddQueue(void);
char SJF(void);

#endif /* KERNEL_H */
