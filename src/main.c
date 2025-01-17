#include "ventana.h"
#include "juego.h"

/**
 * @file main.c 
 *
 * @brief Funcion main en donde se ejecutara todo el juego
 * 
 * @return int 
 */

int main() {
    bool teclas[ALLEGRO_KEY_MAX] = {false};
    srand(time(NULL));

    if (init_allegro() != 0) {
        return -1;
    }

    ALLEGRO_DISPLAY *ventana = crear_ventana(800, 600, "Juego de Naves");
    if (!ventana) {
        return -1;
    }

    ALLEGRO_EVENT_QUEUE* cola_eventos = al_create_event_queue();
    if (!cola_eventos)
    {
        fprintf(stderr, "Error: no se pudo crear la cola de eventos.\n");
        al_destroy_display(ventana);
        return -1;
    }

    ALLEGRO_TIMER* timer = NULL;

    timer = al_create_timer(1.0/FPS);
    if (!timer)
    {
        fprintf(stderr, "Error: No se pudo crear el temporizador.\n");
        al_destroy_event_queue(cola_eventos);
        al_destroy_display(ventana);
        return -1;
    }

    al_register_event_source(cola_eventos, al_get_display_event_source(ventana));
    al_register_event_source(cola_eventos, al_get_timer_event_source(timer));
    al_register_event_source(cola_eventos, al_get_keyboard_event_source());

    al_start_timer(timer);

    Nave nave = init_nave(375, 500, 50, 20);
    
    Asteroide asteroide[NUM_ASTEROIDES];
    init_asteroides(asteroide, NUM_ASTEROIDES, 800, 600);

    bool jugando = true;
    while (jugando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jugando = false;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN || evento.type == ALLEGRO_EVENT_KEY_UP)
        {
            manejar_eventos(evento, &nave, teclas);
        }

        if (evento.type == ALLEGRO_EVENT_TIMER)
        {
            actualizar_nave(&nave, teclas, asteroide, NUM_ASTEROIDES);
            for (int i = 0; i < NUM_ASTEROIDES; i++)
            {
                actualizar_asteroide(&asteroide[i]);
            }        
        }
        
        al_clear_to_color(al_map_rgb(0, 0, 0));
        dibujar_juego(nave, asteroide, NUM_ASTEROIDES);
        dibujar_barra_vida(nave);
        al_flip_display();
    }

    al_destroy_event_queue(cola_eventos);
    destruir_ventana();
    
    return 0;
}