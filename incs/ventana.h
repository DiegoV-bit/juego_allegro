#ifndef VENTANA_H
#define VENTANA_H

#include <allegro5/allegro.h>

int init_allegro();
ALLEGRO_DISPLAY *crear_ventana(int ancho, int largo, const char *titulo);
void destruir_ventana(ALLEGRO_DISPLAY *ventana);

#endif