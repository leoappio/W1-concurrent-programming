#include <stdlib.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "queue.h"


int worker_gate_look_queue()
{   
    queue_t* student_queue = globals_get_queue();
    return student_queue->_length;
}

void worker_gate_remove_student()
{
    student_t* student = globals_get_queue()->_first;
    while(student->_buffet_position == -1);
    queue_remove(globals_get_queue());
}

void worker_gate_look_buffet()
{
    buffet_t* buffets = globals_get_buffets();
    student_t* student = globals_get_queue()->_first;

    for(int i = 0; i < len(buffets); i++){
        if(buffets[i].queue_left[0] == 0){
            student->left_or_right = 'L';
            student->_id_buffet = i;
        }else if(buffets[i].queue_right[0] == 0){
            student->left_or_right = 'R';
            student->_id_buffet = i;
        }
    }
}

void *worker_gate_run(void *arg)
{
    int all_students_entered;
    int number_students;

    number_students = *((int *)arg);
    all_students_entered = number_students > 0 ? FALSE : TRUE;

    while (all_students_entered == FALSE)
    {
        number_students = worker_gate_look_queue();
        all_students_entered = number_students > 0 ? FALSE : TRUE;
        worker_gate_look_buffet();
        worker_gate_remove_student();
        //msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
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
    pthread_join(self->thread, NULL);
    free(self);
}

void worker_gate_insert_queue_buffet(student_t *student)
{
    buffet_t* buffets = globals_get_buffets();  
    while(!buffet_queue_insert(&buffets[student->_id_buffet], student));
}