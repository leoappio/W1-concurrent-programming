#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "queue.h"
#include "table.h"
#include "buffet.h"

/**
 * @brief Inicia uma fila (de modo global)
 * 
 * @param queue 
 */
extern void globals_set_queue(queue_t *queue);

/**
 * @brief Retorna uma fila (de modo global)
 * 
 * @return queue_t* 
 */
extern queue_t *globals_get_queue();

/**
 * @brief Insere o número de alunos (de modo global)
 * 
 */
extern void globals_set_students(int number);

/**
 * @brief Retorna o número de alunos (de modo global)
 * 
 * @return int 
 */

extern int globals_get_students();

/**
 * @brief Insere o número de mesas (de modo global)
 * @param num_tables 
 */
extern void globals_set_number_of_tables(int num_tables);

/**
 * @brief Retorna o número de mesas (de modo global)
 * 
 * @return int 
 */

extern int globals_get_number_of_tables();

/**
 * @brief Insere o número de buffets (de modo global)
 * @param num_tables 
 */
extern void globals_set_buffets_number(int number);

/**
 * @brief Retorna o número de buffets (de modo global)
 * 
 * @return int 
 */

extern int globals_get_buffets_number();


/**
 * @brief Insere o número de assentos por mesa (de modo global)
 * @param num_seats 
 */
extern void globals_set_seats_per_table(int num_seats);

/**
 * @brief Retorna o número de assentos por mesa (de modo global)
 * 
 * @return int 
 */

extern int globals_get_seats_per_table();

/**
 * @brief Inicia um array de mesas (de modo global).
 * 
 * @param t 
 */
extern void globals_set_table(table_t *t);

/**
 * @brief Retorna um array de mesas (de modo global)
 * 
 * @return table_t* 
 */
extern table_t *globals_get_table();


/**
 * @brief Finaliza todas as variáveis globais.
 * 
 */
extern void globals_finalize();

/**
 * @brief Inicia um array de buffets (de modo global)
 * 
 */
extern void globals_set_buffets(buffet_t *buffets_ref);

/**
 * @brief Retorna um array de buffets (de modo global)
 * 
 * @return buffet_t* 
 */
extern buffet_t *globals_get_buffets();

#endif