#include "ventana.h"
#include <stdio.h>

int init_allegro() {
    if (!al_init())
    {
        fprintf(stderr, "Error al inicializar allegro");
        return -1;
    }
    return 0;
}


ALLEGRO_DISPLAY *crear_ventana(int ancho, int largo, const char *titulo) {
    ALLEGRO_DISPLAY *ventana = al_create_display(ancho, largo);
    if (!ventana)
    {
        fprintf(stderr, "Error al crear la ventana.\n");
        return NULL;
    }
    al_set_window_title(ventana, titulo);
    return ventana;
}


void destruir_ventana(ALLEGRO_DISPLAY *ventana) {
    if (ventana) {
        al_destroy_display(ventana);
    }
}