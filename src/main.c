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

    if (init_juego(&ventana, &cola_eventos, &temporizador, &fuente) != 0) return -1; 

    /*Inicializar los botones del menu*/
    Boton botones[3];
    init_botones(botones);

    bool en_menu = true;
    bool jugando = false;
    bool mostrarRanking = false;
    bool volver_menu = false;

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

            if (evento.type == ALLEGRO_EVENT_TIMER)
            {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                dibujar_botones(botones, 3, fuente);
                al_flip_display();
            }
        }
    
        if (jugando)
        {
            // Inicializar asteroides
            Asteroide asteroides[NUM_ASTEROIDES];
            init_asteroides(asteroides, NUM_ASTEROIDES, 800);

            // Inicializar nave
            Nave nave = init_nave(400, 500, 50, 50, 100, 0.1);

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
                    actualizar_juego(&nave, teclas, asteroides, 10, disparos, 10, &puntaje);
                    for (int i = 0; i < NUM_ASTEROIDES; i++)
                    {
                        actualizar_asteroide(&asteroides[i]);
                    }
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    dibujar_juego(nave, asteroides, 10);
                    dibujar_disparos(disparos, 10);
                    dibujar_puntaje(puntaje);
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
                capturar_nombre(fuente, nombre_jugador);
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
            }
        }

        if (volver_menu)
        {
            volver_menu = false;
            en_menu = true;
        }

        if (!en_menu && !jugando && !mostrarRanking)
        {
            break;
        }
    }    

    destruir_recursos(ventana, cola_eventos, temporizador, fuente);

    return 0;
}