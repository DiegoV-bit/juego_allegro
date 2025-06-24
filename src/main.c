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

    ALLEGRO_DISPLAY *ventana = NULL;
    ALLEGRO_EVENT_QUEUE *cola_eventos = NULL;
    ALLEGRO_TIMER *temporizador = NULL;
    ALLEGRO_FONT *fuente = NULL;
    ALLEGRO_BITMAP *fondo_juego = NULL;
    ALLEGRO_BITMAP *imagen_nave = NULL;
    ALLEGRO_BITMAP *imagen_asteroide = NULL;

    if (init_juego(&ventana, &cola_eventos, &temporizador, &fuente, &fondo_juego, &imagen_nave, &imagen_asteroide) != 0)
    {
        return -1;
    }

    /*Inicializar los botones del menu*/
    Boton botones[3];
    init_botones(botones);

    bool en_menu = true;
    bool jugando = false;
    bool mostrarRanking = false;
    bool volver_menu = false;

    int cursor_x = 0;
    int cursor_y = 0;

    Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS];
    cargar_tilemap("nivel1.txt", tilemap);

    while (true)
    {
        while (en_menu)
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(cola_eventos, &evento);

            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                en_menu = false;
                jugando = false;
                mostrarRanking = false;
                break;
            }
        
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            {
                int boton_clicado = detectar_click(botones, 3, evento.mouse.x, evento.mouse.y);
                if (boton_clicado == 0)
                {
                    en_menu = false;
                    jugando = true;
                }
                else if (boton_clicado == 1)
                {
                    en_menu = false;
                    mostrarRanking = true;
                }
                else if (boton_clicado == 2)
                {
                    en_menu = false;
                    jugando = false;
                    mostrarRanking = false;
                    break;
                }
            }

            if (evento.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                cursor_x = evento.mouse.x;
                cursor_y = evento.mouse.y;
            }

            if (evento.type == ALLEGRO_EVENT_TIMER)
            {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                dibujar_botones(botones, 3, fuente, cursor_x, cursor_y);
                al_flip_display();
            }
        }
    
        if (jugando)
        {
            // Inicializar asteroides
            Asteroide asteroides[NUM_ASTEROIDES];
            init_asteroides(asteroides, NUM_ASTEROIDES, 800, imagen_asteroide);

            // Inicializar nave
            Nave nave = init_nave(400, 500, 50, 50, 100, 0.1, imagen_nave);

            // Inicializar disparos
            Disparo disparos[10];
            init_disparos(disparos, 10);

            // Inicializar puntaje en 0
            int puntaje = 0;

            /*Bucle del juego*/
            while (jugando) 
            {
                ALLEGRO_EVENT evento;
                al_wait_for_event(cola_eventos, &evento);

                if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    jugando = false;
                    en_menu = false;
                    mostrarRanking = false;
                    break;
                }

                if (evento.type == ALLEGRO_EVENT_KEY_DOWN || evento.type == ALLEGRO_EVENT_KEY_UP)
                {
                    manejar_eventos(evento, &nave, teclas, disparos, 10);
                }

                if (evento.type == ALLEGRO_EVENT_TIMER)
                {
                    actualizar_juego(&nave, teclas, asteroides, 10, disparos, 10, &puntaje, tilemap);
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    dibujar_tilemap(tilemap, imagen_asteroide);
                    dibujar_juego(nave, asteroides, 10, fondo_juego);
                    dibujar_disparos(disparos, 10);
                    dibujar_puntaje(puntaje, fuente);
                    dibujar_barra_vida(nave); // Dibujar la barra de vida
                    al_flip_display();

                    if (nave.vida <= 0)
                    {
                        jugando = false;
                    }
                }
            }
            if (nave.vida <= 0)
            {
                char nombre_jugador[MAX_NOMBRE];
                capturar_nombre(fuente, nombre_jugador, cola_eventos);
                guardar_puntaje(nombre_jugador, puntaje);
                volver_menu = true;
            }
        }

        if (mostrarRanking)
        {
            Jugador ranking[MAX_JUGADORES];
            int num_jugadores;
            bool volver_menu_ranking = false;
            cargar_ranking(ranking, &num_jugadores);
            mostrar_ranking(fuente, ranking, num_jugadores, &volver_menu_ranking);
            if (volver_menu_ranking)
            {
                mostrarRanking = false;
                en_menu = true;

                cursor_x = 0;
                cursor_y = 0;init_botones(botones);
                al_flush_event_queue(cola_eventos);
            }
        }

        if (volver_menu)
        {
            volver_menu = false;
            en_menu = true;

            cursor_x = 0;
            cursor_y = 0;
            init_botones(botones);

            al_flush_event_queue(cola_eventos);
        }

        if (!en_menu && !jugando && !mostrarRanking)
        {
            break;
        }
    }    

    destruir_recursos(ventana, cola_eventos, temporizador, fuente, fondo_juego, imagen_nave, imagen_asteroide);

    al_uninstall_system(); // Esto evita fugas de memoria y libera recursos evitando el segmentation fault en WSL

    return 0;
}