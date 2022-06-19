#include <stdlib.h>
#include <semaphore.h>
#include "buffet.h"
#include "config.h"
#include "globals.h"
#include "worker_gate.h"

sem_t buffet_positions_semaphore; //Semáfaro de cada posição do buffet.

void *buffet_run(void *arg)
{ 
    //int all_students_entered = FALSE;
    buffet_t *self = (buffet_t*) arg;
    
    /*  O buffet funciona enquanto houver alunos na fila externa. */
    while (TRUE)
    {
        _log_buffet(self);
        msleep(100);

        if(globals_get_students() == 0)break;
    }
    
    //Destruição do mutex de cada opção disponível no buffet, tanto na esquerda quanto na direita.
    for(int k = 0; k < 5; k++){
        pthread_mutex_destroy(&self->mutexes_meals_left[k]);
        pthread_mutex_destroy(&self->mutexes_meals_right[k]);
    }

    pthread_exit(NULL);
}

void buffet_init(buffet_t *self, int number_of_buffets)
{
    globals_set_buffets_number(number_of_buffets);

    int i = 0, j = 0;

    for (i = 0; i < number_of_buffets; i++)
    {
        for(int k = 0; k < 5; k++){
            pthread_mutex_init(&self[i].mutexes_meals_left[k], NULL);
            pthread_mutex_init(&self[i].mutexes_meals_right[k], NULL);
        }
        /*A fila possui um ID*/
        self[i]._id = i;

        /* Inicia com 40 unidades de comida em cada bacia */
        for(j = 0; j < 5; j++)
            self[i]._meal[j] = 40;

        for(j= 0; j< 5; j++){
             /* A fila esquerda do buffet possui cinco posições. */
            self[i].queue_left[j] = 0;
            /* A fila esquerda do buffet possui cinco posições. */
            self[i].queue_right[j] = 0;
        }

        pthread_create(&self[i].thread, NULL, buffet_run, &self[i]);
    }
}


int buffet_queue_insert(buffet_t *self, student_t *student)
{
    /* Se o estudante vai para a fila esquerda */
    //O mutex bloqueia a primeira posição do buffet do lado que o estudante entrar.
    //A cada estudante que entra, há um decremento no valor da variável global dos estudantes.
    if (student->left_or_right == 'L') 
    {
        pthread_mutex_lock(&self[student->_id_buffet].mutexes_meals_left[0]); 
        globals_set_students(globals_get_students() - 1);
        self[student->_id_buffet].queue_left[0] = student->_id;
        student->_buffet_position = 0;
        return TRUE;
    }
    else
    { 
        pthread_mutex_lock(&self[student->_id_buffet].mutexes_meals_right[0]);
        globals_set_students(globals_get_students() - 1);
        /* Verifica se a primeira posição está vaga */
        self[student->_id_buffet].queue_right[0] = student->_id;
        student->_buffet_position = 0;
        return TRUE;  /* Se o estudante vai para a fila direita */
    }
}

//Quando o estudante sai do buffet, a última posição do buffet é liberada
//ao definir o array da fila como 0. E a posição do estudante no buffet
//se torna -1, já que ele não está mais no buffet. Assim, o mutex é destravado
//e há um incremento no semáfaro da posição do buffet.
void student_exit_buffet(buffet_t *self, student_t *student){
    if (student->left_or_right == 'R') {
        self[student->_id_buffet].queue_right[4] = 0;
        student->_buffet_position = -1;
        pthread_mutex_unlock(&self[student->_id_buffet].mutexes_meals_right[4]);
    } else {        
        self[student->_id_buffet].queue_left[4] = 0;
        student->_buffet_position = -1;
        pthread_mutex_unlock(&self[student->_id_buffet].mutexes_meals_left[4]);
    }
    sem_post(&buffet_positions_semaphore);
}

void buffet_next_step(buffet_t *self, student_t *student)
{
    /* Se estudante ainda precisa se servir de mais alguma coisa... */
    if (student->_buffet_position + 1 < 5)
    {    /* Está na fila esquerda? */
        if (student->left_or_right == 'L')
        {   /* Caminha para a posição seguinte da fila do buffet.*/
            int position = student->_buffet_position;
            pthread_mutex_lock(&self[student->_id_buffet].mutexes_meals_left[position+1]);

            self[student->_id_buffet].queue_left[position] = 0;
            self[student->_id_buffet].queue_left[position + 1] = student->_id;
            pthread_mutex_unlock(&self[student->_id_buffet].mutexes_meals_left[position]);
            student->_buffet_position = student->_buffet_position + 1;
        }else /* Está na fila direita? */
        {   /* Caminha para a posição seguinte da fila do buffet.*/
            int position = student->_buffet_position;
            pthread_mutex_lock(&self[student->_id_buffet].mutexes_meals_right[position+1]);
            self[student->_id_buffet].queue_right[position] = 0;
            self[student->_id_buffet].queue_right[position + 1] = student->_id;
            pthread_mutex_unlock(&self[student->_id_buffet].mutexes_meals_right[position]);
            student->_buffet_position = student->_buffet_position + 1;
        }
    }else{
        student_exit_buffet(self, student);
    }
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

void buffet_finalize(buffet_t *self, int number_of_buffets)
{
    /* Espera as threads se encerrarem...*/
    for (int i = 0; i < number_of_buffets; i++)
    {
        pthread_join(self[i].thread, NULL);
    }
    /*Libera a memória.*/
    free(self);
}


void _log_buffet(buffet_t *self)
{
    /* Prints do buffet */
    int *ids_left = self->queue_left; 
    int *ids_right = self->queue_right;

    printf("\n\n\u250F\u2501 Queue left: [ %d %d %d %d %d ]\n", ids_left[0],ids_left[1],ids_left[2],ids_left[3],ids_left[4]);
    fflush(stdout);
    printf("\u2523\u2501 BUFFET %d = [RICE: %d/40 BEANS:%d/40 PLUS:%d/40 PROTEIN:%d/40 SALAD:%d/40]\n",
           self->_id, self->_meal[0], self->_meal[1], self->_meal[2], self->_meal[3], self->_meal[4]);
    fflush(stdout);
    printf("\u2517\u2501 Queue right: [ %d %d %d %d %d ]\n", ids_right[0],ids_right[1],ids_right[2],ids_right[3],ids_right[4]);
    fflush(stdout);
}