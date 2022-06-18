#include <stdlib.h>
#include <semaphore.h>
#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "queue.h"

sem_t total_seats_semaphore;
sem_t students_queue_semaphore;

pthread_mutex_t seats_mutex;


void worker_gate_look_queue()
{   
    sem_wait(&students_queue_semaphore);
}

void worker_gate_remove_student()
{
    student_t* removed_student = queue_remove(globals_get_queue());
    worker_gate_insert_queue_buffet(removed_student);
}

void worker_gate_look_buffet()
{
    sem_wait(&buffet_positions_semaphore);
}

void *worker_gate_run(void *arg)
{
    int total_number_of_seats = globals_get_seats_per_table() * globals_get_number_of_tables();

    sem_init(&total_seats_semaphore, 0 , total_number_of_seats);
    pthread_mutex_init(&seats_mutex, NULL);
    sem_init(&students_queue_semaphore, 0, 0);
    sem_init(&buffet_positions_semaphore, 0, globals_get_buffets_number() * 10);

    while (globals_get_students() != 0)
    {
        worker_gate_look_queue();
        worker_gate_look_buffet();
        worker_gate_remove_student();
    }

    pthread_exit(NULL);
}

void worker_gate_init(worker_gate_t *self)
{
    int number_students = globals_get_students();
    pthread_create(&self->thread, NULL, worker_gate_run, &number_students);
}

void worker_gate_finalize(worker_gate_t *self)
{
    pthread_mutex_destroy(&seats_mutex);

    sem_destroy(&students_queue_semaphore);
    sem_destroy(&total_seats_semaphore);
    sem_destroy(&buffet_positions_semaphore);

    pthread_join(self->thread, NULL);
    free(self);
}

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
    globals_set_students(globals_get_students() - 1);
    sem_post(&student->sem_student);
}