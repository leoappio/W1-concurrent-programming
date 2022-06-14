#include <stdlib.h>

#include "chef.h"
#include "config.h"
#include "globals.h"
#include "buffet.h"

void *chef_run()
{
    //busy wait para não dar seg fault ao iniciar o chef
    while(globals_get_buffets() == NULL);
    while (TRUE)
    {
        if(globals_get_students() != 0){
            chef_check_food();
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

void chef_check_food()
{
    buffet_t* buffets = globals_get_buffets();
    int buffets_number = globals_get_buffets_number();
    while(globals_get_buffets() == NULL);
    for(int i = 0; i < buffets_number; i++){
        for(int j = 0; j < 5; j++){
            pthread_mutex_lock(&buffets[i].mutexes_meals_left[j]);
            pthread_mutex_unlock(&buffets[i].mutexes_meals_right[j]);
            if (buffets[i]._meal[j] == 0){
                chef_put_food(buffets, i, j);
            }
            pthread_mutex_unlock(&buffets[i].mutexes_meals_left[j]);
            pthread_mutex_unlock(&buffets[i].mutexes_meals_right[j]);
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