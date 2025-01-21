#include "ventana.h"

/**
 * @file ventana.c
 * @brief Este archivo contiene todas las funciones referentes a la ventana del juego
 * 
 */

ALLEGRO_DISPLAY *ventana = NULL;
ALLEGRO_EVENT_QUEUE *cola_eventos = NULL;

/**
 * @brief Inicializa la biblioteca Allegro.
 *
 * Esta función inicializa Allegro y sus complementos necesarios, como el teclado y los primitivos.
 * 
 * @return 0 si la inicialización fue exitosa, -1 en caso de error.
 */
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

    if (!al_install_mouse()) {
        fprintf(stderr, "Error: No se pudo inicializar el mouse.\n");
        return -1;
    }

    if (!al_init_font_addon()) {
        fprintf(stderr, "Error: No se pudo inicializar las fuentes.\n");
        return -1;
    }
    
    if (!al_init_ttf_addon()) {
        fprintf(stderr, "Error: No se pudo inicializar las fuentes TTF.\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Crea una ventana de visualización.
 *
 * Esta función crea una ventana con el tamaño y título especificados.
 *
 * @param ancho Ancho de la ventana.
 * @param largo Alto de la ventana.
 * @param titulo Título de la ventana.
 * @return Puntero a la ventana creada, o NULL en caso de error.
 */
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

/**
 * @brief Muestra la ventana y maneja los eventos.
 *
 * Esta función entra en un bucle que espera eventos y actualiza la ventana.
 * Se cierra cuando se recibe un evento de cierre de ventana.
 */
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

/**
 * @brief Destruye todos los recursos del juego una vez acabado de jugar
 * 
 * @param ventana Ventana del juego.
 * @param cola_eventos Cola de eventos.
 * @param temporizador Temporizador usado para los FPS
 * @param fuente Fuente de letra usada en el juego
 */
void destruir_recursos(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* cola_eventos, ALLEGRO_TIMER* temporizador, ALLEGRO_FONT* fuente)
{
    if (ventana) 
    {
        al_destroy_display(ventana);
        ventana = NULL;
    }

    if (cola_eventos) 
    {
        al_destroy_event_queue(cola_eventos);
        cola_eventos = NULL;
    }

    if (temporizador)
    {
        al_destroy_timer(temporizador);
        temporizador = NULL;
    }
    
    if (fuente)
    {
        al_destroy_font(fuente);
        fuente = NULL;
    }
}