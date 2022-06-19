#include <stdlib.h>
#include <semaphore.h>
#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "queue.h"

sem_t total_seats_semaphore; //semáfaro do total de lugares nas mesas do RU.
sem_t students_queue_semaphore; //semáfaro dos estudantes na fila do RU.

//Quando o worker_gate olha para a fila, há um decremento no semáfaro
// dos estudantes na fila do RU, pois ele permite o estudante entrar no RU.
void worker_gate_look_queue()
{   
    sem_wait(&students_queue_semaphore);
}

//Quando o worker_gate permite a entrada do estudante no buffet,
//remove-se o estudante da fila de fora do RU e o insere na fila
//do buffet, respectivamente.
void worker_gate_remove_student()
{
    student_t* removed_student = queue_remove(globals_get_queue());
    worker_gate_insert_queue_buffet(removed_student);
}

//Ao olhar o buffet, o worker_gate tenta decrementar o semáfaro das posições do buffets
//para que o próximo estudante possa entrar no buffet.
void worker_gate_look_buffet()
{
    sem_wait(&buffet_positions_semaphore);
}

//Enquanto houver estudantes, o worker_gate irá realizar sua rotina.
//Primeiro, ele olha a fila de fora, depois olha para o buffet e, por fim,
//remove o estudante da fila de fora do RU.
void *worker_gate_run(void *arg)
{
    while (TRUE)
    {
        worker_gate_look_queue();
        worker_gate_look_buffet();
        worker_gate_remove_student();


        if(globals_get_students() <= 0){
            break;
        }
    }

    pthread_exit(NULL);
}

//Ao inicializar o worker_gate, calcula-se o número de lugares nas mesas do RU.
//Depois, inicia-se o semáfaro dos lugares do RU, o semáfaro da fila de fora do RU,
//e o das posições do buffet. E, assim, cria-se a thread do worker_gate.
void worker_gate_init(worker_gate_t *self)
{
    int total_number_of_seats = globals_get_seats_per_table() * globals_get_number_of_tables();
    sem_init(&total_seats_semaphore, 0 , total_number_of_seats);
    sem_init(&students_queue_semaphore, 0, 0);
    sem_init(&buffet_positions_semaphore, 0, globals_get_buffets_number() * 10);
    int number_students = globals_get_students();
    pthread_create(&self->thread, NULL, worker_gate_run, &number_students);
}

//Destrói os semáfaros, realiza um join das threads e libera a memória.
void worker_gate_finalize(worker_gate_t *self)
{

    sem_destroy(&students_queue_semaphore);
    sem_destroy(&total_seats_semaphore);
    sem_destroy(&buffet_positions_semaphore);

    pthread_join(self->thread, NULL);
    free(self);
}

//Percorrem-se os buffets até que um dos lados de um buffet esteja livre.
//Quando isto ocorrer, ou se o estudante já estiver em um dos lados de um buffet,
//o código sai do while(true), insere o estudante no buffet (buffet_queue_insert()) e 
//há um incremento no semáfaro sem_student.
void worker_gate_insert_queue_buffet(student_t *student)
{
    buffet_t* buffets = globals_get_buffets(); 
    int number_of_buffets = globals_get_buffets_number();
    while (TRUE) {
        for (int i = 0; i < number_of_buffets; i++) {
            if (buffets[i].queue_right[0] == 0) {
                student->left_or_right = 'R';
                student->_id_buffet = i;
                break;
            }
            if (buffets[i].queue_left[0] == 0) {
                student->left_or_right = 'L';
                student->_id_buffet = i;
                break;
            }
        }

        if(student->left_or_right == 'L' || student->left_or_right == 'R'){
            break;
        }
    }

    buffet_queue_insert(buffets, student);
    sem_post(&student->sem_student);
}