#include <stdlib.h>
#include "globals.h"

queue_t *students_queue = NULL;
table_t *table = NULL;
buffet_t *buffets_ref = NULL;

int students_number = 0;
int number_of_tables = 0;
int seats_per_table = 0;
int buffets_number = 0;

void globals_set_queue(queue_t *queue)
{
    students_queue = queue;
}

queue_t *globals_get_queue()
{
    return students_queue;
}

void globals_set_table(table_t *t)
{
    table = t;
}

table_t *globals_get_table()
{
    return table;
}

void globals_set_buffets_number(int number)
{
    buffets_number = number;
}

int globals_get_buffets_number()
{
    return buffets_number;
}


void globals_set_students(int number)
{
    students_number = number;
}

int globals_get_students()
{
    return students_number;
}

void globals_set_number_of_tables(int num_tables)
{
    number_of_tables = num_tables;
}

int globals_get_number_of_tables()
{
    return number_of_tables;
}

void globals_set_seats_per_table(int num_seats)
{
    seats_per_table = num_seats;
}

int globals_get_seats_per_table()
{
    return seats_per_table;
}

void globals_set_buffets(buffet_t *buffets)
{
    buffets_ref = buffets;
}

buffet_t *globals_get_buffets()
{
    return buffets_ref;
}

/**
 * @brief Finaliza todas as variáveis globais que ainda não foram liberadas.
 *  Se criar alguma variável global que faça uso de mallocs, lembre-se sempre de usar o free dentro
 * dessa função.
 */
void globals_finalize()
{
    free(table);
}