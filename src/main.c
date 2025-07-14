#include "ventana.h"
#include "juego.h"

/**
 * @file main.c 
 *
 * @brief Funcion main en donde se ejecutara todo el juego
 * 
 * @return int 
 */

int main() 
{
    srand(time(NULL)); // Inicializa el generador de números aleatorios
    bool teclas[ALLEGRO_KEY_MAX] = {false};
    int i;
    float nave_x_inicial;
    float nave_y_inicial;

    // Inicializar Allegro y sus addons
    ALLEGRO_DISPLAY *ventana = NULL;
    ALLEGRO_EVENT_QUEUE *cola_eventos = NULL;
    ALLEGRO_TIMER *temporizador = NULL;
    ALLEGRO_FONT *fuente = NULL;
    ALLEGRO_BITMAP *fondo_juego = NULL;
    ALLEGRO_BITMAP *imagen_nave = NULL;
    ALLEGRO_BITMAP *imagen_asteroide = NULL;
    ALLEGRO_BITMAP *imagen_enemigo = NULL;

    Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS];

    Enemigo enemigos_mapa[NUM_ENEMIGOS];
    int num_enemigos_cargados = 0;

    if (init_juego(&ventana, &cola_eventos, &temporizador, &fuente, &fondo_juego, &imagen_nave, &imagen_asteroide, &imagen_enemigo) != 0)
    {
        return -1;
    }

    cargar_tilemap("Nivel1.txt", tilemap, enemigos_mapa, &num_enemigos_cargados, imagen_enemigo, &nave_x_inicial, &nave_y_inicial);

    /*Inicializar los botones del menu*/
    Boton botones[3];
    init_botones(botones);

    bool en_menu = true;
    bool jugando = false;
    bool mostrarRanking = false;
    bool volver_menu = false;

    int cursor_x = 0;
    int cursor_y = 0;

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
            // Inicializar estado del juego
            EstadoJuego estado_nivel;
            init_estado_juego(&estado_nivel);

            bool recargar_nivel = false;
            bool juego_terminado = false;

            // Inicializar asteroides
            Asteroide asteroides[NUM_ASTEROIDES];
            init_asteroides(asteroides, NUM_ASTEROIDES, 800, imagen_asteroide);

            // Inicializar nave
            Nave nave = init_nave(nave_x_inicial, nave_y_inicial, 50, 50, 100, 0.1, imagen_nave);

            // Inicializar disparos
            Disparo disparos[MAX_DISPAROS];
            init_disparos(disparos, MAX_DISPAROS);

            // Inicializar Enemigos
            Disparo disparos_enemigos[NUM_DISPAROS_ENEMIGOS];

            Enemigo enemigos[NUM_ENEMIGOS];
            for (i = 0; i < num_enemigos_cargados && i < NUM_ENEMIGOS; i++) {
                enemigos[i] = enemigos_mapa[i];
                enemigos[i].imagen = imagen_enemigo; // Asegurar que usen el sprite correcto
            }
            // Inicializar el resto como inactivos
            for (i = num_enemigos_cargados; i < NUM_ENEMIGOS; i++) {
                enemigos[i].activo = false;
            }
            
            init_disparos(disparos_enemigos, NUM_DISPAROS_ENEMIGOS);

            // Inicializar puntaje en 0
            int puntaje = 0;

            // Inicializar Mensajes
            Mensaje mensaje_powerup;
            Mensaje mensaje_movilidad;
            init_mensaje(&mensaje_powerup);
            init_mensaje(&mensaje_movilidad);

            double tiempo_cache = 0;

            /*Bucle del juego*/
            while (jugando && !juego_terminado) 
            {
                ALLEGRO_EVENT evento;
                al_wait_for_event(cola_eventos, &evento);

                if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                {
                    jugando = false;
                    en_menu = false;
                    mostrarRanking = false;
                    break;
                }

                if (evento.type == ALLEGRO_EVENT_KEY_DOWN || evento.type == ALLEGRO_EVENT_KEY_UP)
                {
                    if (!estado_nivel.mostrar_transicion)
                    {
                        manejar_eventos(evento, &nave, teclas, disparos, MAX_DISPAROS);
                    }
                }

                if (evento.type == ALLEGRO_EVENT_TIMER)
                {
                    tiempo_cache = al_get_time();

                    // Verificar si necesitamos cargar el siguiente nivel
                    if (estado_nivel.nivel_completado && !estado_nivel.mostrar_transicion && !recargar_nivel)
                    {
                        recargar_nivel = true;
                        estado_nivel.nivel_completado = false;
                    }

                    // Cargar siguiente nivel si es necesario
                    if (recargar_nivel)
                    {
                        int siguiente_nivel = estado_nivel.nivel_actual + 1;

                        if (cargar_siguiente_nivel(siguiente_nivel, tilemap, enemigos_mapa, &num_enemigos_cargados, imagen_enemigo, &nave_x_inicial, &nave_y_inicial))
                        {
                            estado_nivel.nivel_actual = siguiente_nivel;
                            estado_nivel.todos_enemigos_eliminados = false;

                            // REINICIALIZAR NAVE COMPLETAMENTE
                            nave = init_nave(nave_x_inicial, nave_y_inicial, 50, 50, 100, 0.1, imagen_nave);
                            
                            // Limpiar todas las teclas
                            for(int k = 0; k < ALLEGRO_KEY_MAX; k++)
                            {
                                teclas[k] = false;
                            }

                            // MANEJAR ASTEROIDES SEGÚN EL NIVEL
                            if (asteroides_activados(estado_nivel.nivel_actual)) {
                                // Reinicializar asteroides para el nuevo nivel
                                init_asteroides(asteroides, NUM_ASTEROIDES, 800, imagen_asteroide);
                                printf("Asteroides ACTIVADOS en nivel %d\n", estado_nivel.nivel_actual);
                            } else {
                                // Desactivar completamente los asteroides
                                for (int k = 0; k < NUM_ASTEROIDES; k++) {
                                    asteroides[k].y = -1000; // Fuera de la pantalla
                                    asteroides[k].x = -1000;
                                    asteroides[k].velocidad = 0; // Sin movimiento
                                }
                                printf("Asteroides DESACTIVADOS en nivel %d\n", estado_nivel.nivel_actual);
                            }

                            // RECARGAR ENEMIGOS
                            int enemigos_a_copiar = (num_enemigos_cargados < NUM_ENEMIGOS) ? num_enemigos_cargados : NUM_ENEMIGOS;

                            for (int k = 0; k < enemigos_a_copiar; k++) 
                            {
                                enemigos[k] = enemigos_mapa[k];
                                enemigos[k].imagen = imagen_enemigo;
                            }

                            for (int k = num_enemigos_cargados; k < NUM_ENEMIGOS; k++) {
                                enemigos[k].activo = false;
                            }

                            // LIMPIAR DISPAROS
                            init_disparos(disparos, MAX_DISPAROS);
                            init_disparos(disparos_enemigos, NUM_DISPAROS_ENEMIGOS);
                        
                            printf("Nivel %d iniciado con %d enemigos.\n", estado_nivel.nivel_actual, num_enemigos_cargados);
                        }
                        else 
                        {
                            // No hay más niveles
                            juego_terminado = true;
                            printf("¡Felicidades! Has completado todos los niveles.\n");
                        }
                        recargar_nivel = false;
                    }

                    // Cambiar movilidad si corresponde se puso en 30 para probar
                    if (puntaje >= 30 && nave.tipo == 0)
                    {
                        nave.tipo = 1;
                        for(int k = 0; k < ALLEGRO_KEY_MAX; k++)
                        {
                            teclas[k] = false; // Reiniciar teclas para evitar problemas de movimiento
                        }
                        mostrar_mensaje(&mensaje_movilidad, "Nueva movilidad desbloqueada descubre como usarla", 150, 200, 4.0, al_map_rgb(0, 255, 0));
                    }

                    actualizar_juego(&nave, teclas, asteroides, 10, disparos, 10, &puntaje, tilemap, enemigos, num_enemigos_cargados, disparos_enemigos, NUM_DISPAROS_ENEMIGOS, &mensaje_powerup, &mensaje_movilidad, &estado_nivel, tiempo_cache);
                    
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    
                    // Si estamos en transición, mostrar pantalla de transición
                    if (estado_nivel.mostrar_transicion)
                    {
                        double tiempo_transcurrido = al_get_time() - estado_nivel.tiempo_inicio_transicion;
                        mostrar_pantalla_transicion(estado_nivel.nivel_actual, estado_nivel.nivel_actual + 1, fuente, tiempo_transcurrido, estado_nivel.duracion_transicion);
                    }
                    else
                    {
                        // Dibujar el juego normal
                        dibujar_tilemap(tilemap, imagen_asteroide);
                        dibujar_juego(nave, asteroides, 10, estado_nivel.nivel_actual);
                        dibujar_disparos(disparos, 10);
                        
                        dibujar_enemigos(enemigos, num_enemigos_cargados);
                        dibujar_disparos_enemigos(disparos_enemigos, NUM_DISPAROS_ENEMIGOS);
                        dibujar_puntaje(puntaje, fuente);
                        dibujar_barra_vida(nave);
                        dibujar_nivel_powerup(nave, fuente);

                        if (mensaje_powerup.activo) dibujar_mensaje(mensaje_powerup, fuente);
                        if (mensaje_movilidad.activo) dibujar_mensaje(mensaje_movilidad, fuente);
                        
                        // Mostrar nivel actual
                        char texto_nivel[50];
                        sprintf(texto_nivel, "Nivel: %d", estado_nivel.nivel_actual);
                        al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 120, ALLEGRO_ALIGN_LEFT, texto_nivel);
                    }
                    
                    al_flip_display();
                    
                    if (nave.vida <= 0)
                    {
                        jugando = false;
                        // Capturar nombre para el ranking
                        char nombre_jugador[MAX_NOMBRE];
                        capturar_nombre(fuente, nombre_jugador, cola_eventos);
                        guardar_puntaje(nombre_jugador, puntaje);
                        volver_menu = true;
                    }
                    
                    // Si terminamos todos los niveles, salir del juego
                    if (juego_terminado) {
                        jugando = false;
                        volver_menu = true;
                    }
                }
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
                cursor_y = 0;
                init_botones(botones);
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

    destruir_recursos(ventana, cola_eventos, temporizador, fuente, fondo_juego, imagen_nave, imagen_asteroide, imagen_enemigo);

    al_uninstall_system(); // Esto evita fugas de memoria y libera recursos evitando el segmentation fault en WSL

    return 0;
}