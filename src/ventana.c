#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>
#include "ventana.h"

ALLEGRO_DISPLAY *ventana = NULL;
ALLEGRO_EVENT_QUEUE *cola_eventos = NULL;

int init_allegro() {
    if (!al_init()) {
        fprintf(stderr, "Error al inicializar Allegro.\n");
        return -1;
    }

    if (!al_install_keyboard()) {
        fprintf(stderr, "Error: No se pudo inicializar el teclado.\n");
        return -1;
    }

    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Error: No se pudo inicializar los primitivos.\n");
        return -1;
    }

    return 0;
}

ALLEGRO_DISPLAY *crear_ventana(int ancho, int largo, const char *titulo) {
    ventana = al_create_display(ancho, largo);
    if (!ventana) {
        fprintf(stderr, "Error al crear la ventana.\n");
        return NULL;
    }

    cola_eventos = al_create_event_queue();
    if (!cola_eventos) {
        fprintf(stderr, "Error: No se pudo crear la cola de eventos.\n");
        al_destroy_display(ventana);
        return NULL;
    }

    al_register_event_source(cola_eventos, al_get_display_event_source(ventana));
    al_set_window_title(ventana, titulo);
    return ventana;
}

void mostrar_ventana() {
    
    if (!cola_eventos) {
        fprintf(stderr, "Error: La cola de eventos no está inicializada.\n");
        return;
    }
    bool ejec = true;

    while (ejec) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            ejec = false;
        }

        al_clear_to_color(al_map_rgb(0, 0, 0)); // Fondo negro
        al_flip_display();
        al_rest(0.01); // Reduce la carga del procesador
    }
}

void destruir_ventana() {
    if (ventana) {
        al_destroy_display(ventana);
        ventana = NULL;
    }

    if (cola_eventos) {
        al_destroy_event_queue(cola_eventos);
        cola_eventos = NULL;
    }
}