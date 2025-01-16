#include "ventana.h"
#include "juego.h"

int main() {
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

    al_register_event_source(cola_eventos, al_get_display_event_source(ventana));
    al_register_event_source(cola_eventos, al_get_keyboard_event_source());    

    Nave nave = init_nave(375, 500, 50, 20);
    Asteroide asteroide = init_asteroide(rand() % 750, -50, 5, 50, 50);

    bool jugando = true;
    while (jugando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jugando = false;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN || evento.type == ALLEGRO_EVENT_KEY_UP)
        {
            manejar_eventos(evento, &nave);
        }

        actualizar_juego(&nave, &asteroide);

        if (detectar_colision(nave, asteroide))
        {
            fprintf(stderr, "¡Colisión! Fin del juego.\n");
            jugando = false;
        }
        
        dibujar_juego(nave, asteroide);
    }

    al_destroy_event_queue(cola_eventos);
    destruir_ventana();
    
    return 0;
}
