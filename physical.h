#ifndef __PHYSICAL_H__
#define __PHYSICAL_H__

#include <stdint.h>
#include <stdbool.h>
#include "rnd.h"
#include "layers.h"

int physical_receive(struct link *link);
// retorna 1 se no link_process() informa que acabou a transmissão pro laço infinito na thread parar
// recebe por parâmetro struct link ao invés dr struct physical, pois a camada de enlace precisa da camada física  depois pra enviar quadros
void physical_send(struct physical *physical, unsigned char *message, size_t size); // envia via socket perfeitamente

struct physical physical_initialize(int fd);

#endif
