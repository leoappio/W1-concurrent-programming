#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "student.h"
#include "config.h"
#include "worker_gate.h"
#include "globals.h"
#include "table.h"

//Inicia-se o semáfaro do estudante, que é liberado quando o worker gate o deixa
//entrar no RU. O estudante é inserido na fila e há um incremento no semáfaro da fila.
//O estudante se serve, realiza sua refeição e vai embora. Quando ele sai,
//o seu semáfaro é destruído.
void* student_run(void *arg)
{
    student_t *self = (student_t*) arg;
    table_t *tables  = globals_get_table();
    queue_t *students = globals_get_queue();
    sem_init(&self->sem_student, 0, 0);
    queue_insert(students, self);
    sem_post(&students_queue_semaphore);
    sem_wait(&self->sem_student);
    student_serve(self);
    student_seat(self, tables);
    student_leave(self, tables);
    sem_destroy(&self->sem_student);
    pthread_exit(NULL);
};

//Há um semáfaro do total de lugares nas mesas do RU. Este semáfaro é
//decrementado quando um estudante se senta. Percorre-se as mesas até
//achar uma com lugares disponíveis. Quando isto é achado, a variável
//_empty_seats ligada àquela mesa é decrementada.
void student_seat(student_t *self, table_t *table)
{
    sem_wait(&total_seats_semaphore);
    int tables_number = globals_get_number_of_tables();
    table_t* tables = globals_get_table();
    for (int i = 0; i < tables_number; i++) {
        if (tables[i]._empty_seats > 0) {
            tables[i]._empty_seats--;
            break;
        }
    }
}

//Enquanto o estudante estiver se servindo no buffet (_buffet_position != -1), caso
//o estudante queira aquela opção do buffet, há um decremento da quantidade disponível daquela opção.
//E a função buffet_next_step() é chamada. O msleep é referente ao tempo que o estudante demora
//se servir, para sincronização.
void student_serve(student_t *self)
{
    buffet_t *all_buffets = globals_get_buffets();
    while(self->_buffet_position != -1){
        if (self->_wishes[self->_buffet_position]){
            msleep(50);
            all_buffets[self->_id_buffet]._meal[self->_buffet_position]--;
        }
        buffet_next_step(all_buffets, self);        
    }
}

//Quando o estudante vai embora do RU, percorre-se as mesas do RU.
//Ao achar uma mesa que não está vazia, incrementa-se a variável
//_empty_seats. E há, também, um incremento do semáfaro referente
//ao total de lugares nas mesas do RU.
void student_leave(student_t *self, table_t *table)
{
    int tables_number = globals_get_number_of_tables();
    int seats_per_table = globals_get_seats_per_table();
    table_t* tables = globals_get_table();
    for (int i = 0; i < tables_number; i++) {
        if (tables[i]._empty_seats < seats_per_table) {
            tables[i]._empty_seats++;
            break;
        }
    }
    sem_post(&total_seats_semaphore);
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

student_t *student_init()
{
    student_t *student = malloc(sizeof(student_t));
    student->_id = rand() % 1000;
    student->_buffet_position = -1;
    int none = TRUE;
    for (int j = 0; j <= 4; j++)
    {
        student->_wishes[j] = _student_choice();
        if(student->_wishes[j] == 1) none = FALSE;
    }

    if(none == FALSE){
        /* O estudante só deseja proteína */
        student->_wishes[3] = 1;
    }

    return student;
};

void student_finalize(student_t *self){
    free(self);
};


pthread_t students_come_to_lunch(int number_students)
{
    pthread_t lets_go;
    pthread_create(&lets_go, NULL, _all_they_come, &number_students);
    return lets_go;
}

/**
 * @brief Função (privada) que inicializa as threads dos alunos.
 * 
 * @param arg 
 * @return void* 
 */
void* _all_they_come(void *arg)
{
    int number_students = *((int *)arg);
    
    student_t *students[number_students];

    for (int i = 0; i < number_students; i++)
    {
        students[i] = student_init();                                               /* Estudante é iniciado, recebe um ID e escolhe o que vai comer*/
    }

    for (int i = 0; i < number_students; i++)
    {
        pthread_create(&students[i]->thread, NULL, student_run, students[i]);       /*  Cria as threads  */
    }

    for (int i = 0; i < number_students; i++)
    {
        pthread_join(students[i]->thread, NULL);                                    /*  Aguarda o término das threads   */
    }

    for (int i = 0; i < number_students; i++)
    {
        student_finalize(students[i]);                                              /*  Libera a memória de cada estudante  */
    }

    pthread_exit(NULL);
}

/**
 * @brief Função que retorna as escolhas dos alunos, aleatoriamente (50% para cada opção)
 *        retornando 1 (escolhido) 0 (não escolhido). É possível que um aluno não goste de nenhuma opção
 *         de comida. Nesse caso, considere que ele ainda passa pela fila, como todos aqueles que vão comer.
 * @return int 
 */
int _student_choice()
{
    float prob = (float)rand() / RAND_MAX;
    return prob > 0.51 ? 1 : 0;
}