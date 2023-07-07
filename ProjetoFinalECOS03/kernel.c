#include "kernel.h"

//declaração das variáveis e structs utilizados no projeto

FILE *inputFile;

Scheduler escalonador;
Process processos[MAX_PROCESSES];
Queue filas[MAX_PROCESSES];

int start;
int end;
int quantum;
int linha;

char kernelInit(){ //inicialização das variaveis, teste do arquivo de entrada
    start = 0;
    end = 0;
    quantum = 5;
    linha = 0;
    escalonador.num_queues = 0;
    escalonador.p_disponiveis = processos;
    escalonador.queues = filas;
    inputFile = fopen("inputFile.txt", "r");
    fclose(inputFile);
    if (inputFile == NULL) { //verificação de erro
        printf("Erro ao abrir o arquivo.");
        return FAIL;
    }
    return SUCCESS;
}

char kernelAddProc(){ //adiciona o processo ao buffer do sistema
    fscanf(inputFile, "%d %d %d", &processos[end].id, &processos[end].priority, &processos[end].time_left);
    if(processos[end].id >= 0)
        return SUCCESS;
    return FAIL;
}

char kernelRemoveProc(){ //remove o processo do buffer do sistema
    processos[start].priority = 0;
    if(end != start){
    start = (start+1)%MAX_PROCESSES;
    return SUCCESS;
  }
  return FAIL;
}

void kernelLoop(void){ //função principal do kernel, gerencia os escalonadores batch e interativo e o buffer de processos

        linha = 0;
        inputFile = fopen("inputFile.txt", "r");
        if (inputFile == NULL) {
            printf("Erro ao abrir o arquivo.");
        }
        escalonador.exec = multilevelQueuePriority;
        kernelAddQueue();
        kernelAddQueue();   
        kernelAddQueue();

        while(!feof(inputFile)){
        if (((end + 1) % MAX_PROCESSES) != start)
        {
            kernelAddProc();
            if(processos[end].time_left <= 5)
                kernelEnqueue(escalonador.queues, 2);
            else if(processos[end].time_left > 5 && processos[end].time_left <= 10)
                kernelEnqueue(escalonador.queues, 1);
            else if(processos[end].time_left > 10)
                kernelEnqueue(escalonador.queues, 0);
            
            end = (end + 1) % (MAX_PROCESSES);
        }else{
            kernelRemoveProc();
        }
        }
        while(escalonador.exec() != SUCCESS);
        fclose(inputFile);
        

}

char kernelAddQueue() { //função para adicionar mais filas ao kernel
    if (escalonador.num_queues >= MAX_NUM_QUEUES) {
        
        return FAIL;
    }
    filas[escalonador.num_queues].head = 0;
    filas[escalonador.num_queues].tail = 0;
    escalonador.num_queues++;
    return SUCCESS;
}

char kernelEnqueue(Queue *q, int index) { //função para enfileirar um processo em uma fila
    if ((q[index].tail + 1) % MAX_QUEUE_SIZE == q[index].head) {
        return FAIL; 
    }
    q[index].buffer[q[index].tail] = processos[end];
    q[index].tail = (q[index].tail + 1) % MAX_QUEUE_SIZE;
    return SUCCESS;
}

char kernelDequeue(Queue *q, int index) { //função para tirar um processo de uma fila
    q[index].head = (q[index].head + 1) % MAX_QUEUE_SIZE;
    if (q[index].head == q[index].tail) {
        return FAIL; 
    }
    return SUCCESS;
}


char multilevelQueuePriority(void) { //função dos escalonadores interativo e batch, onde são organizados os processos para execução
    int head_aux, tail_aux, index, i, j;
    char repeticao;
    Process aux;
    Process aux2;
    int smallest = 0;
    
    
    while (escalonador.num_queues>=1) {
        index = escalonador.num_queues-1;
        head_aux = filas[index].head;
        tail_aux = filas[index].tail - 1;
        repeticao = REPEAT;

        while (repeticao != SUCCESS) {

            for (i = head_aux; i <= tail_aux; i++) {
                
                SJF();
                
                
                filas[index].buffer[i].running = 1;
                
                switch(index){
                    
                    case 0:
                        filas[index].buffer[i].time_left -= (quantum*3);
                        break;
                    case 1:
                        filas[index].buffer[i].time_left -= (quantum*2);
                        break;
                    case 2:
                        filas[index].buffer[i].time_left -= (quantum*1);
                        break;
                    default:
                        break;
                    
                }

                if (filas[index].buffer[i].time_left <= 0 && filas[index].buffer[i].done != 1) {
                    filas[index].buffer[i].done = 1;
                    aux = filas[index].buffer[i];
                    filas[index].buffer[i] = filas[index].buffer[head_aux];
                    filas[index].buffer[head_aux] = aux;
                    printf("%d. ID do Processo: %d \tNumero da Fila: %d\tT. Restante: %d\tFinalizado ?: %d\n", linha++, aux.id, index, aux.time_left, aux.done);
                    
                    if (kernelDequeue(escalonador.queues, index) == FAIL) {
                        escalonador.num_queues--;
                        repeticao = SUCCESS;
                    } else {
                        head_aux = filas[index].head;
                    }
                } else if (filas[index].buffer[i].time_left > 0 && filas[index].buffer[i].done != 1) {
                    aux = filas[index].buffer[i];
                    filas[index].buffer[i] = filas[index].buffer[head_aux];
                    filas[index].buffer[head_aux] = aux;
                    aux2 = filas[index].buffer[i];
                    kernelDequeue(escalonador.queues, index);
                    aux2.running = 0;
                    
                    if (kernelEnqueue(escalonador.queues, (index -1)) == FAIL && index==2) {
                        if (kernelEnqueue(escalonador.queues, (index -2)) == FAIL) {
                            kernelEnqueue(escalonador.queues, index);
                            if (index > 2){printf("\nerro\n");}
                            
                        }
                    }
                    
                    printf("%d. ID do Processo: %d \tNumero da Fila: %d\tT. Restante: %d\tFinalizado ?: %d\n", linha++, aux.id, index, aux.time_left, aux.done);
                }
            }
        }
    }
    
    return SUCCESS;
}

char SJF(void){ //função que faz o trabalho do SJF, onde o processo com menor T Left fica na head da fila
    
    int index = escalonador.num_queues-1;
    int head_aux = filas[index].head;
    int tail_aux = filas[index].tail - 1;
    int smallest = 0;
    int i, j;
    
    for (j = head_aux; j <= tail_aux; j++) {
                    if (filas[index].buffer[j].time_left < filas[index].buffer[smallest].time_left) {
                        smallest = j;
                    }
                Process temp = filas[index].buffer[filas[index].head];
                filas[index].buffer[filas[index].head] = filas[index].buffer[smallest];
                filas[index].buffer[smallest] = temp;
                }
    
}