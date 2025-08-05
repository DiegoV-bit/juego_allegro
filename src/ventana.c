#include "ventana.h"

/**
 * @file ventana.c
 * @brief Este archivo contiene todas las funciones referentes a la ventana del juego
 * 
 */

ALLEGRO_DISPLAY *ventana = NULL;
ALLEGRO_EVENT_QUEUE *cola_eventos = NULL;
ALLEGRO_BITMAP* imagen_fondo = NULL;
ALLEGRO_BITMAP* imagen_nave = NULL;
ALLEGRO_BITMAP* imagen_asteroide = NULL;

/**
 * @brief Inicializa la biblioteca Allegro.
 *
 * Esta función inicializa Allegro, crea la ventana, cola de eventos, temporizador,
 * fuente y carga todas las imágenes necesarias para el juego (nave, asteroide, enemigo).
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

    if (!al_init_image_addon()) {
        fprintf(stderr, "Error: No se pudo inicializar las imagenes.\n");
        return -1;
    }

    if (!al_install_audio()) {
        fprintf(stderr, "Error: No se pudo inicializar el audio.\n");
        return -1;
    }
    
    if (!al_init_acodec_addon()) {
        fprintf(stderr, "Error: No se pudo inicializar los codecs de audio.\n");
        return -1;
    }

    if (!al_reserve_samples(1)) {
        fprintf(stderr, "Error: No se pudo reservar muestras de audio.\n");
        return -1;
    }

    printf("Allegro inicializado correctamente.\n");
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
 * @brief Destruye todos los recursos del juego una vez acabado de jugar
 * 
 * @param ventana Ventana del juego.
 * @param cola_eventos Cola de eventos.
 * @param temporizador Temporizador usado para los FPS
 * @param fuente Fuente de letra usada en el juego
 * @param imagen Imagen usada en el juego
 * @param imagen_nave Imagen usada para la nave
 * @param imagen_asteroide Imagen usada para asteroides
 * @param imagen_enemigo Imagen usada para enemigos
 * @param imagen_menu Imagen de fondo del menú
 * @param musica_fondo Música de fondo del juego
 */
void destruir_recursos(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* cola_eventos, ALLEGRO_TIMER* temporizador, ALLEGRO_FONT* fuente, ALLEGRO_BITMAP* imagen, ALLEGRO_BITMAP* imagen_nave, ALLEGRO_BITMAP* imagen_asteroide, ALLEGRO_BITMAP* imagen_enemigo, ALLEGRO_BITMAP *imagen_menu, ALLEGRO_SAMPLE *musica_fondo)
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

    if (imagen)
    {
        al_destroy_bitmap(imagen);
        imagen = NULL;
    }

    if (imagen_nave)
    {
        al_destroy_bitmap(imagen_nave);
        imagen_nave = NULL;
    }

    if (imagen_asteroide)
    {
        al_destroy_bitmap(imagen_asteroide);
        imagen_asteroide = NULL;
    }

    if (imagen_enemigo) 
    {
        al_destroy_bitmap(imagen_enemigo);
        imagen_enemigo = NULL;
    }

    if (imagen_menu)
    {
        al_destroy_bitmap(imagen_menu);
        imagen_menu = NULL;
    }

    if (musica_fondo)
    {
        al_destroy_sample(musica_fondo);
        musica_fondo = NULL;
    }
}

/**
 * @brief Permite inicializar todos los recursos del juego
 * 
 * @param ventana Puntero doble a la ventana del juego
 * @param cola_eventos Puntero doble a la cola de eventos
 * @param temporizador Puntero doble al temporizador
 * @param fuente Puntero doble a la fuente de letra
 * @param imagen_fondo Puntero doble a la imagen de fondo
 * @param imagen_nave Puntero doble a la imagen de la nave
 * @param imagen_asteroide Puntero doble a la imagen del asteoride
 * @return int Si la inicializacion fue exitosa retorna 0, en caso contrario retorna -1
 */
int init_juego(ALLEGRO_DISPLAY **ventana, ALLEGRO_EVENT_QUEUE **cola_eventos, ALLEGRO_TIMER **temporizador, ALLEGRO_FONT **fuente, ALLEGRO_BITMAP **imagen_fondo, ALLEGRO_BITMAP **imagen_nave, ALLEGRO_BITMAP **imagen_asteroide, ALLEGRO_BITMAP **imagen_enemigo, ALLEGRO_BITMAP **imagen_menu, ALLEGRO_SAMPLE **musica_fondo, ALLEGRO_SAMPLE_INSTANCE **instancia_musica)
{
    if (init_allegro() != 0) {
        return -1;
    }

    *ventana = crear_ventana(ANCHO_VENTANA, ALTO_VENTANA, "Juego de Naves");
    if (!*ventana) {
        return -1;
    }

    *cola_eventos = al_create_event_queue();
    if (!*cola_eventos) {
        fprintf(stderr, "Error: no se pudo crear la cola de eventos.\n");
        al_destroy_display(*ventana);
        return -1;
    }

    *temporizador = al_create_timer(1.0 / 60); // 60 FPS
    if (!*temporizador) {
        fprintf(stderr, "Error: no se pudo crear el temporizador.\n");
        al_destroy_event_queue(*cola_eventos);
        al_destroy_display(*ventana);
        return -1;
    }

    al_register_event_source(*cola_eventos, al_get_display_event_source(*ventana));
    al_register_event_source(*cola_eventos, al_get_timer_event_source(*temporizador));
    al_register_event_source(*cola_eventos, al_get_keyboard_event_source());
    al_register_event_source(*cola_eventos, al_get_mouse_event_source());

    al_start_timer(*temporizador);

    *fuente = al_load_ttf_font("pixel_arial_11/PIXEARG_.TTF", 24, 0);
    if (!*fuente) {
        fprintf(stderr, "Error: no se pudo crear la fuente.\n");
        al_destroy_event_queue(*cola_eventos);
        al_destroy_display(*ventana);
        al_destroy_timer(*temporizador);
        return -1;
    }

    *imagen_fondo = al_load_bitmap("imagenes/Fondo_juego.jpeg");
    if (!imagen_fondo) {
        fprintf(stderr, "Error: no se pudo cargar la imagen de fondo.\n");
        al_destroy_event_queue(*cola_eventos);
        al_destroy_display(*ventana);
        al_destroy_timer(*temporizador);
        al_destroy_font(*fuente);
        return -1;
    }

    *imagen_nave = al_load_bitmap("imagenes/jugador/nave.png");
    if (!*imagen_nave) {
        fprintf(stderr, "Error: no se pudo cargar la imagen de la nave.\n");
        al_destroy_event_queue(*cola_eventos);
        al_destroy_display(*ventana);
        al_destroy_timer(*temporizador);
        al_destroy_font(*fuente);
        al_destroy_bitmap(*imagen_fondo);
        return -1;
    }

    *imagen_asteroide = al_load_bitmap("imagenes/enemigos/asteroide.png");
    if (!*imagen_asteroide) {
        fprintf(stderr, "Error: no se pudo cargar la imagen del asteroide.\n");
        al_destroy_event_queue(*cola_eventos);
        al_destroy_display(*ventana);
        al_destroy_timer(*temporizador);
        al_destroy_font(*fuente);
        al_destroy_bitmap(*imagen_fondo);
        al_destroy_bitmap(*imagen_nave);
        return -1;
    }

        // Cargar imagen específica para enemigos
    *imagen_enemigo = al_load_bitmap("imagenes/enemigos/Enemigo1.png");
    if (!imagen_enemigo) {
        fprintf(stderr, "Error: no se pudo cargar la imagen del enemigo.\n");
        al_destroy_event_queue(*cola_eventos);
        al_destroy_display(*ventana);
        al_destroy_timer(*temporizador);
        al_destroy_font(*fuente);
        al_destroy_bitmap(*imagen_fondo);
        al_destroy_bitmap(*imagen_nave);
        al_destroy_bitmap(*imagen_asteroide);
        return -1;
    }

    *imagen_menu = al_load_bitmap("imagenes/Fondo.jpeg");
    if (!*imagen_menu)
    {
        fprintf(stderr, "Advertencia: no se pudo cargar la imagen del menú. Usando fondo negro.\n");
        *imagen_menu = NULL;
    }
    else
    {
        printf("Imagen de menú cargada correctamente.\n");
    }

    *musica_fondo = al_load_sample("audio/Cosmic-Circuitry.wav");
    if (!*musica_fondo)
    {
        fprintf(stderr, "Advertencia: no se pudo cargar la música de fondo.\n");
        *musica_fondo = al_load_sample("audio/Cosmic-Circuitry.wav");
        if (!*musica_fondo)
        {
            fprintf(stderr, "Advertencia: no se pudo cargar música en formato WAV tampoco.\n");
            *musica_fondo = NULL;
        }
    }

    if (*musica_fondo) 
    {
        *instancia_musica = al_create_sample_instance(*musica_fondo);
        if (*instancia_musica) {
            al_set_sample_instance_playmode(*instancia_musica, ALLEGRO_PLAYMODE_LOOP);
            al_set_sample_instance_gain(*instancia_musica, 0.5f); // Volumen al 50%
            al_attach_sample_instance_to_mixer(*instancia_musica, al_get_default_mixer());
            printf("Música de fondo configurada correctamente.\n");
        }
        else
        {
            fprintf(stderr, "Error: no se pudo crear la instancia de música.\n");
        }
    }
    else
    {
        *instancia_musica = NULL;
    }

    return 0;
}