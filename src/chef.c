#include <stdlib.h>

#include "chef.h"
#include "config.h"
#include "globals.h"

void *chef_run()
{
    queue_t* students_queue = globals_get_queue();
    buffet_t* buffets = globals_get_buffets();

    while (TRUE)
    {
        if(students_queue->_length != 0){
            chef_check_food(buffets);
        }else{
            break;
        }
    }
    
    pthread_exit(NULL);
}

void chef_put_food(buffet_t* buffets, int i, int j)
{
    buffets[i]._meal[j] = 40;
}

void chef_check_food(buffet_t* buffets)
{
    for(int i = 0; i < len(buffets); i++){
        for(int j = 0; j < 5; j++){
            if (buffets[i]._meal[j] == 0){
                chef_put_food(buffets, i, j);
            }
        }
    }
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

void chef_init(chef_t *self)
{
    pthread_create(&self->thread, NULL, chef_run, NULL);
}

void chef_finalize(chef_t *self)
{
    pthread_join(self->thread, NULL);
    free(self);
}