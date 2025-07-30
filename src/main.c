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
    int k;
    float nave_x_inicial;
    float nave_y_inicial;
    bool debug_mode = false;
    DisparoLaser lasers[5];
    DisparoExplosivo explosivos[8];
    MisilTeledirigido misil[6];

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

    int contador_parpadeo_powerups = 0;
    int contador_debug_powerups = 0;
    int contador_debug_lasers = 0;
    //int contador_debug_cola = 0;

    if (init_juego(&ventana, &cola_eventos, &temporizador, &fuente, &fondo_juego, &imagen_nave, &imagen_asteroide, &imagen_enemigo) != 0)
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
            // Recargo el nivel 1 desde cero
            cargar_tilemap("Nivel1.txt", tilemap, enemigos_mapa, &num_enemigos_cargados, imagen_enemigo, &nave_x_inicial, &nave_y_inicial);

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

            // Inicializar el sistema de armas
            init_sistema_armas(&nave);

            for (int i = 0; i < 5; i++)
            {
                lasers[i].activo = false;
            }

            for (int i = 0; i < 8; i++)
            {
                explosivos[i].activo = false;
            }

            for (int i = 0; i < 6; i++)
            {
                misil[i].activo = false;
            }

            // Inicializar disparos
            Disparo disparos[MAX_DISPAROS];
            init_disparos(disparos, MAX_DISPAROS);

            // Inicializar Powerups
            Powerup powerups[MAX_POWERUPS];
            for (k = 0; k < MAX_POWERUPS; k++)
            {
                init_powerup(&powerups[k]);
            }
            
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
            ColaMensajes cola_mensajes;
            init_cola_mensajes(&cola_mensajes);

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
                        if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_SPACE)
                        {
                            if (nave.arma_actual == Arma_laser)
                            {
                                bool laser_activo = false;
                                for (i = 0; i < 5; i++)
                                {
                                    if (lasers[i].activo)
                                    {
                                        laser_activo = true;
                                        break;
                                    }
                                }

                                if (!laser_activo)
                                {
                                    disparar_laser(lasers, 5, nave);
                                    printf("Laser activado al presionar espacio\n");
                                }
                            }
                            else
                            {
                                disparar_segun_arma(nave, disparos, MAX_DISPAROS, lasers, 5, explosivos, 8, misil, 6, enemigos, num_enemigos_cargados);
                            }
                        }
                        
                        // Se manejan los eventos de teclado que no tengan que ver con la tecla espacio
                        if (evento.keyboard.keycode != ALLEGRO_KEY_SPACE)
                        {
                            manejar_eventos(evento, &nave, teclas);
                        }
                    }
                    
                    if (evento.type == ALLEGRO_EVENT_KEY_UP && evento.keyboard.keycode == ALLEGRO_KEY_SPACE)
                    {
                        if (nave.arma_actual == Arma_laser)
                        {
                            bool laser_desactivado = false;
                            for (i = 0; i < 5; i++)
                            {
                                if (lasers[i].activo)
                                {
                                    lasers[i].activo = false;
                                    laser_desactivado = true;
                                    printf("Laser desactivado al soltar espacio\n");
                                }
                            }

                            if (!laser_desactivado)
                            {
                                printf("No hay laseres en pantalla\n");
                            }
                        }
                    }

                    // Manejo de eventos de debug
                    if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_F1)
                    {
                        debug_mode = !debug_mode;
                        printf("Modo debug %s\n", debug_mode ? "ACTIVADO" : "DESACTIVADO");
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

                            for (k = 0; k < MAX_POWERUPS; k++)
                            {
                                powerups[k].activo = false;
                                powerups[k].x = 0;
                                powerups[k].y = 0;
                            }

                            for (int clear_i = 0; clear_i < MAX_DISPAROS; clear_i++)
                            {
                                disparos[clear_i].activo = false;
                            }

                            for (int clear_i = 0; clear_i < NUM_DISPAROS_ENEMIGOS; clear_i++)
                            {
                                disparos_enemigos[clear_i].activo = false;
                            }

                            for (int clear_i = 0; clear_i < 5; clear_i++)
                            {
                                lasers[clear_i].activo = false;
                            }

                            for (int clear_i = 0; clear_i < 8; clear_i++)
                            {
                                explosivos[clear_i].activo = false;
                            }

                            for (int clear_i = 0; clear_i < 6; clear_i++)
                            {
                                misil[clear_i].activo = false;
                            }

                            // Guardar el estado de la nave antes de reinicializarla
                            int nivel_disparo_radial_guardado = nave.nivel_disparo_radial;
                            int kills_para_mejora_guardado = nave.kills_para_mejora;
                            int tipo_nave_guardado = nave.tipo;

                            SistemaArma armas_guardadas[4];
                            for (int i = 0; i < 4; i++) {
                                armas_guardadas[i] = nave.armas[i];
                            }
                            
                            TipoArma arma_actual_guardada = nave.arma_actual;
                            int arma_seleccionada_guardada = nave.arma_seleccionada;

                            nave.escudo.activo = false;

                            // Reinicializar la nave con los valores guardados
                            nave = init_nave(nave_x_inicial, nave_y_inicial, 50, 50, 100, 0.1, imagen_nave);

                            // Restaurar los valores guardados
                            nave.nivel_disparo_radial = nivel_disparo_radial_guardado;
                            nave.kills_para_mejora = kills_para_mejora_guardado;
                            nave.tipo = tipo_nave_guardado;

                            for (int i = 0; i < 4; i++) {
                                nave.armas[i] = armas_guardadas[i];
                            }
                            
                            nave.arma_actual = arma_actual_guardada;
                            nave.arma_seleccionada = arma_seleccionada_guardada;

                            memset(teclas, false, sizeof(teclas)); // Reiniciar teclas

                            if (asteroides_activados(estado_nivel.nivel_actual)) 
                            {
                                init_asteroides(asteroides, NUM_ASTEROIDES, 800, imagen_asteroide);
                            }
                            else
                            {
                                for (int k = 0; k < NUM_ASTEROIDES; k++)
                                {
                                    asteroides[k].y = -2000;
                                    asteroides[k].x = -2000;
                                    asteroides[k].velocidad = 0;
                                    asteroides[k].ancho = 0;
                                    asteroides[k].alto = 0;
                                }
                                printf("Asteroides DESACTIVADOS en nivel %d\n", estado_nivel.nivel_actual);
                            }

                            // MANEJAR ASTEROIDES SEGÚN EL NIVEL
                            if (asteroides_activados(estado_nivel.nivel_actual)) {
                                // Reinicializar asteroides para el nuevo nivel
                                init_asteroides(asteroides, NUM_ASTEROIDES, 800, imagen_asteroide);
                                printf("Asteroides ACTIVADOS en nivel %d\n", estado_nivel.nivel_actual);
                            }
                            else
                            {
                                // Desactivar completamente los asteroides
                                for (int k = 0; k < NUM_ASTEROIDES; k++)
                                {
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
                                enemigos[k].activo = true;
                            }

                            printf("Nivel %d iniciado con %d enemigos.\n", estado_nivel.nivel_actual, num_enemigos_cargados);
                            printf("Powerups conservados: Radial Nv.%d, Tipo Nave: %d\n", nave.nivel_disparo_radial, nave.tipo);

                            // LIMPIAR DISPAROS
                            init_disparos(disparos, MAX_DISPAROS);
                            init_disparos(disparos_enemigos, NUM_DISPAROS_ENEMIGOS);
                        
                            printf("Nivel %d iniciado con %d enemigos.\n", estado_nivel.nivel_actual, num_enemigos_cargados);
                            printf("Powerups conservados: Radial Nv.%d, Tipo Nave: %d\n", nave.nivel_disparo_radial, nave.tipo);
                        }
                        else 
                        {
                            // No hay más niveles
                            juego_terminado = true;
                            printf("¡Felicidades! Has completado todos los niveles.\n");
                        }

                        for (k = 0; k < MAX_POWERUPS; k++)
                        {
                            init_powerup(&powerups[k]);
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
                        
                        agregar_mensaje_cola(&cola_mensajes, "¡Nueva Movilidad Desbloqueada!", 4.0, al_map_rgb(0, 255, 0), true); // Centrado
                        
                        agregar_mensaje_cola(&cola_mensajes, "Usa las flechas para rotar y avanzar", 3.0, al_map_rgb(255, 255, 255), true); // Centrado
                    }

                    actualizar_lasers(lasers, 5, enemigos, num_enemigos_cargados, &puntaje, nave, tilemap, &contador_debug_lasers);
                    actualizar_explosivos(explosivos, 8, enemigos, num_enemigos_cargados, &puntaje);
                    actualizar_misiles(misil, 6, enemigos, num_enemigos_cargados, &puntaje);

                    actualizar_juego(&nave, teclas, asteroides, 10, disparos, 10, &puntaje, tilemap, enemigos, num_enemigos_cargados, disparos_enemigos, NUM_DISPAROS_ENEMIGOS, &cola_mensajes, &estado_nivel, tiempo_cache, powerups, MAX_POWERUPS);
                    
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
                        dibujar_juego(nave, asteroides, 10, estado_nivel.nivel_actual, fondo_juego);
                        dibujar_tilemap(tilemap, imagen_asteroide);
                        dibujar_escudo(nave);
                        dibujar_disparos(disparos, 10);

                        dibujar_lasers(lasers, 5, tilemap);
                        dibujar_explosivos(explosivos, 8);
                        dibujar_misiles(misil, 6);
                        
                        dibujar_enemigos(enemigos, num_enemigos_cargados);
                        dibujar_disparos_enemigos(disparos_enemigos, NUM_DISPAROS_ENEMIGOS);
                        dibujar_powerups(powerups, MAX_POWERUPS, &contador_parpadeo_powerups, &contador_debug_powerups);

                        if (debug_mode)
                        {
                            dibujar_hitboxes_debug(nave, enemigos, num_enemigos_cargados, disparos, MAX_DISPAROS, disparos_enemigos, NUM_DISPAROS_ENEMIGOS, asteroides, NUM_ASTEROIDES, tilemap);
                        }
                        

                        dibujar_puntaje(puntaje, fuente);
                        dibujar_barra_vida(nave);
                        dibujar_nivel_powerup(nave, fuente);

                        dibujar_info_armas(nave, fuente);

                        dibujar_cola_mensajes(cola_mensajes, fuente);
                        
                        // Mostrar nivel actual
                        char texto_nivel[50];
                        sprintf(texto_nivel, "Nivel: %d", estado_nivel.nivel_actual);
                        al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 120, ALLEGRO_ALIGN_LEFT, texto_nivel);

                        if (escudo_activo(nave))
                        {
                            char texto_escudo[50];
                            sprintf(texto_escudo, "Escudo: %d hits", nave.escudo.hits_restantes);
                            al_draw_text(fuente, al_map_rgb(0, 255, 255), 10, 100, ALLEGRO_ALIGN_CENTER, texto_escudo);
                        }
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
                    if (juego_terminado)
                    {
                        jugando = false;
                        // Capturar nombre para el ranking
                        char nombre_jugador[MAX_NOMBRE];
                        
                        // Mostrar mensaje de victoria antes de pedir el nombre
                        al_clear_to_color(al_map_rgb(0, 0, 0));
                        al_draw_text(fuente, al_map_rgb(0, 255, 0), 400, 250, ALLEGRO_ALIGN_CENTER, "FELICIDADES!");
                        al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, "Has completado todos los niveles!");
                        char texto_puntaje_final[100];
                        sprintf(texto_puntaje_final, "Puntaje final: %d", puntaje);
                        al_draw_text(fuente, al_map_rgb(255, 255, 0), 400, 350, ALLEGRO_ALIGN_CENTER, texto_puntaje_final);
                        al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 400, ALLEGRO_ALIGN_CENTER, "Presiona cualquier tecla para continuar...");
                        al_flip_display();

                        bool esperando = true;
                        while (esperando)
                        {
                            ALLEGRO_EVENT victoria;
                            al_wait_for_event(cola_eventos, &victoria);
                            if (victoria.type == ALLEGRO_EVENT_KEY_DOWN || victoria.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                            {
                                esperando = false;
                            }
                        }

                        capturar_nombre(fuente, nombre_jugador, cola_eventos);
                        guardar_puntaje(nombre_jugador, puntaje);
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