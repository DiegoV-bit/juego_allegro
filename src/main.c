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
    srand(time(NULL)); // Inicializa el generador de números aleatorios
    bool teclas[ALLEGRO_KEY_MAX] = {false};

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

    ALLEGRO_TIMER* temporizador = al_create_timer(1.0 / 60); // 60 FPS
    if (!temporizador)
    {
        fprintf(stderr, "Error: no se pudo crear el temporizador.\n");
        al_destroy_event_queue(cola_eventos);
        al_destroy_display(ventana);
        return -1;
    }

    al_register_event_source(cola_eventos, al_get_display_event_source(ventana));
    al_register_event_source(cola_eventos, al_get_timer_event_source(temporizador));
    al_register_event_source(cola_eventos, al_get_keyboard_event_source());

    al_start_timer(temporizador);

    // Inicializar asteroides
    Asteroide asteroides[NUM_ASTEROIDES];
    init_asteroides(asteroides, NUM_ASTEROIDES, 800, 600);

    // Inicializar nave
    Nave nave = init_nave(400, 500, 50, 50);

    Disparo disparos[10];
    int puntaje = 0;

    init_disparos(disparos, 10);

    bool jugando = true;
    while (true) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        double tiempo_actual = al_get_time(); // Obtener el tiempo actual

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jugando = false;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN || evento.type == ALLEGRO_EVENT_KEY_UP)
        {
            manejar_eventos(evento, &nave, teclas, disparos, 10);
        }

        if (evento.type == ALLEGRO_EVENT_TIMER)
        {
            actualizar_juego(&nave, teclas, asteroides, 10, disparos, 10, &puntaje);
            for (int i = 0; i < NUM_ASTEROIDES; i++)
            {
                actualizar_asteroide(&asteroides[i]);
            }
            al_clear_to_color(al_map_rgb(0, 0, 0));
            dibujar_juego(nave, asteroides, 10);
            dibujar_disparos(disparos, 10);
            dibujar_barra_vida(nave); // Dibujar la barra de vida
            al_flip_display();
        }
    }

    al_destroy_event_queue(cola_eventos);
    al_destroy_display(ventana);
    al_destroy_timer(temporizador);

    return 0;
}