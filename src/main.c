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
    ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS];

    // Variables del menu principal
    ALLEGRO_BITMAP *imagen_menu = NULL;
    ALLEGRO_AUDIO_STREAM *stream_musica_fondo = NULL;
    Boton botones[3];
    bool en_menu;
    bool jugando;
    bool mostrarRanking;
    bool volver_menu;
    int cursor_x;
    int cursor_y;
    int boton_clicado;

    Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS];

    Enemigo enemigos_mapa[NUM_ENEMIGOS];
    int num_enemigos_cargados = 0;

    int contador_parpadeo_powerups = 0;
    int contador_debug_powerups = 0;
    int contador_debug_lasers = 0;
    Jefe jefe_final;
    Jefe jefe_nivel;
    bool hay_jefe_en_nivel = false;
    bool hay_jefe_activo;
    ALLEGRO_BITMAP *imagen_jefe = NULL;
    ALLEGRO_EVENT evento;

    if (init_juego(&ventana, &cola_eventos, &temporizador, &fuente, &fondo_juego, &imagen_nave, &imagen_asteroide, &imagen_enemigo, &imagen_menu, &stream_musica_fondo) != 0)
    {
        return -1;
    }

    if (!cargar_imagenes_enemigos(imagenes_enemigos))
    {
        printf("ERROR: No se pudieron cargar las imágenes de enemigos\n");
        return -1;
    }

    imagen_jefe = al_load_bitmap("jefe.png");
    if (!imagen_jefe)
    {
        printf("Advertencia: No se pudo cargar imagen del jefe, usando placeholder\n");
        // Crear placeholder para jefe
        imagen_jefe = al_create_bitmap(120, 80);
        al_set_target_bitmap(imagen_jefe);
        al_clear_to_color(al_map_rgb(150, 0, 150)); // Púrpura para jefe
        al_draw_rectangle(0, 0, 119, 79, al_map_rgb(255, 255, 255), 3);
        al_set_target_backbuffer(al_get_current_display());
    }

    /*Inicializar los botones del menu*/
    init_botones(botones);

    en_menu = true;
    jugando = false;
    mostrarRanking = false;
    volver_menu = false;

    cursor_x = 0;
    cursor_y = 0;

    while (true)
    {
        while (en_menu)
        {
            if (stream_musica_fondo && !al_get_audio_stream_playing(stream_musica_fondo))
            {
                al_set_audio_stream_playing(stream_musica_fondo, true);
                printf("Música del menú iniciada\n");
            }
            
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
                boton_clicado = detectar_click(botones, 3, evento.mouse.x, evento.mouse.y);
                if (boton_clicado == 0)
                {
                    if (stream_musica_fondo)
                    {
                        al_set_audio_stream_playing(stream_musica_fondo, false);
                        printf("Música del menú detenida\n");
                    }
                    
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
                if (imagen_menu)
                {
                    al_draw_scaled_bitmap(imagen_menu, 0, 0, 
                                        al_get_bitmap_width(imagen_menu), 
                                        al_get_bitmap_height(imagen_menu), 
                                        0, 0, 800, 600, 0);
                }
                else
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                }
                
                dibujar_botones(botones, 3, fuente, cursor_x, cursor_y);
                al_flip_display();
            }
        }
    
        if (jugando)
        {   
            hay_jefe_en_nivel = false;
            hay_jefe_activo = false;
            memset(&jefe_final, 0, sizeof(Jefe));
            jefe_nivel.activo = false;

            memset(teclas, false, sizeof(teclas)); // Reiniciar teclas

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
            Nave nave = init_nave(nave_x_inicial, nave_y_inicial, 50, 50, 100.0f, 0.1, imagen_nave);

            memset(teclas, false, sizeof(teclas)); // Reiniciar teclas

            // Inicializar el sistema de armas
            init_sistema_armas(&nave);

            for (int i = 0; i < 5; i++)
            {
                lasers[i].activo = false;
            }

            for (int i = 0; i < 8; i++)
            {
                explosivos[i].activo = false;
                explosivos[i].exploto = false;
                explosivos[i].dano_aplicado = false;
                explosivos[i].x = 0;
                explosivos[i].y = 0;
                explosivos[i].vx = 0;
                explosivos[i].vy = 0;
                explosivos[i].tiempo_vida = 0;
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
                asignar_imagen_enemigo(&enemigos[i], imagenes_enemigos); // Asegurar que usen el sprite correcto
            }
            // Inicializar el resto como inactivos
            for (i = num_enemigos_cargados; i < NUM_ENEMIGOS; i++) 
            {
                enemigos[i].activo = false;
            }

            hay_jefe_en_nivel = false;
            hay_jefe_activo = false;

            printf("Verificando jefes para nivel %d con %d enemigos cargados\n", estado_nivel.nivel_actual, num_enemigos_cargados);

            if (estado_nivel.nivel_actual >= 4)
            {
                for (i = 0; i < num_enemigos_cargados; i++)
                {
                    printf("Enemigo %d: tipo %d en (%.0f, %.0f)\n", i, enemigos_mapa[i].tipo, enemigos_mapa[i].x, enemigos_mapa[i].y);

                    if (enemigos_mapa[i].tipo == 5 || enemigos_mapa[i].tipo == 6) 
                    {
                        int tipo_jefe = enemigos_mapa[i].tipo == 5 ? 0 : 1; // 5=Destructor(0), 6=Supremo(1)
                        init_jefe(&jefe_nivel, tipo_jefe, enemigos_mapa[i].x, enemigos_mapa[i].y, imagen_jefe);
                        hay_jefe_en_nivel = true;
                        printf("Jefe encontrado en nivel %d: tipo %d\n", estado_nivel.nivel_actual, tipo_jefe);
                    
                        // Remover jefe del array de enemigos normales
                        for (int k = i; k < num_enemigos_cargados - 1; k++) 
                        {
                            enemigos_mapa[k] = enemigos_mapa[k + 1];
                        }
                        num_enemigos_cargados--;
                        break;
                    }
                }
                if (!hay_jefe_en_nivel)
                {
                    printf("Nivel %d: Sin jefes encontrados\n", estado_nivel.nivel_actual);
                }
                
            }
            else
            {
                printf("Nivel %d: No debería tener jefes\n", estado_nivel.nivel_actual);
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
                                else
                                {
                                    printf("Ya hay un laser activo\n");
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
                            int laser_desactivado = 0;
                            for (i = 0; i < 5; i++)
                            {
                                if (lasers[i].activo)
                                {
                                    lasers[i].activo = false;
                                    laser_desactivado++;
                                }
                            }

                            if (laser_desactivado > 0)
                            {
                                printf("Laser desactivado al soltar la tecla espacio\n");
                            }
                            else
                            {
                                printf("No hay lasers activos para desactivar\n");
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
                            for (int i = 0; i < 4; i++)
                            {
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

                            for (int i = 0; i < 4; i++)
                            {
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
                                asignar_imagen_enemigo(&enemigos[k], imagenes_enemigos); // Asegurar que usen el sprite correcto
                                enemigos[k].activo = true;
                            }

                            hay_jefe_en_nivel = false;
                            hay_jefe_activo = false;
                            memset(&jefe_nivel, 0, sizeof(Jefe));
                            jefe_nivel.activo = false;

                            for (k = 0; k < num_enemigos_cargados; k++)
                            {
                                printf("Enemigo recarga %d: tipo %d en (%.0f, %.0f)\n", k, enemigos_mapa[k].tipo, enemigos_mapa[k].x, enemigos_mapa[k].y);

                                if (enemigos_mapa[k].tipo == 5 || enemigos_mapa[k].tipo == 6)
                                {
                                    int tipo_jefe = enemigos_mapa[k].tipo == 5 ? 0 : 1;
                                    init_jefe(&jefe_nivel, tipo_jefe, enemigos_mapa[k].x, enemigos_mapa[k].y, imagen_jefe);
                                    hay_jefe_en_nivel = true;
                                    printf("Jefe cargado en nivel %d: enemigo tipo %d -> jefe tipo %d\n", estado_nivel.nivel_actual, enemigos_mapa[k].tipo, tipo_jefe);

                                    for (int j = k; j < num_enemigos_cargados - 1; j++) 
                                    {
                                        enemigos_mapa[j] = enemigos_mapa[j + 1];
                                    }
                                    num_enemigos_cargados--;
                                    break;
                                }
                            }
                            if (!hay_jefe_en_nivel) 
                            {
                                printf("Nivel %d: Sin jefes en recarga\n", estado_nivel.nivel_actual);
                            }
                        }
                        else 
                        {
                            if (siguiente_nivel > 5)
                            {
                                // No hay más niveles
                            juego_terminado = true;
                            printf("¡Felicidades! Has completado todos los niveles.\n");
                            }
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

                    bool hay_lasers_activos = false;
                    for (int laser_check = 0; laser_check < 5; laser_check++)
                    {
                        if (lasers[laser_check].activo)
                        {
                            hay_lasers_activos = true;
                            break;
                        }
                    }

                    if (hay_lasers_activos)
                    {
                        actualizar_lasers(lasers, 5, enemigos, num_enemigos_cargados, &puntaje, nave, tilemap, &contador_debug_lasers, powerups, MAX_POWERUPS);
                    }
                    
                    bool hay_explosivos_activos = false;
                    for (int exp_check = 0; exp_check < 8; exp_check++)
                    {
                        if (explosivos[exp_check].activo)
                        {
                            hay_explosivos_activos = true;
                            break;
                        }
                    }

                    if (hay_explosivos_activos)
                    {
                        actualizar_explosivos(explosivos, 8, enemigos, num_enemigos_cargados, &puntaje, tilemap);
                    }
                    
                    bool hay_misiles_activos = false;
                    for (int mis_check = 0; mis_check < 6; mis_check++)
                    {
                        if (misil[mis_check].activo)
                        {
                            hay_misiles_activos = true;
                            break;
                        }
                    }

                    if (hay_misiles_activos)
                    {
                        actualizar_misiles(misil, 6, enemigos, num_enemigos_cargados, &puntaje);
                    }
                    
                    if (hay_jefe_en_nivel && jefe_nivel.activo)
                    {
                        actualizar_jefe(&jefe_nivel, nave, enemigos, &num_enemigos_cargados, imagenes_enemigos, tiempo_cache);
                        
                        // Verificar colisiones ataques del jefe vs nave
                        for (int k = 0; k < MAX_ATAQUES_JEFE; k++) {
                            if (jefe_nivel.ataques[k].activo && detectar_colision_ataque_jefe_nave(jefe_nivel.ataques[k], nave)) {
                                if (escudo_recibir_dano(&nave.escudo)) {
                                    printf("Escudo absorbió ataque del jefe\n");
                                } else {
                                    nave.vida -= jefe_nivel.ataques[k].dano;
                                    printf("Jefe causo %.1f de daño. Vida restante: %.1f\n", jefe_nivel.ataques[k].dano, nave.vida);
                                    agregar_mensaje_cola(&cola_mensajes, "¡Ataque del Jefe!", 2.0, al_map_rgb(255, 0, 0), false);
                                }
                                jefe_nivel.ataques[k].activo = false;
                            }
                        }
                    }

                    actualizar_juego(&nave, teclas, asteroides, 10, disparos, 10, &puntaje, tilemap, enemigos, num_enemigos_cargados, disparos_enemigos, NUM_DISPAROS_ENEMIGOS, &cola_mensajes, &estado_nivel, tiempo_cache, powerups, MAX_POWERUPS);
                    
                    if (hay_jefe_en_nivel && jefe_nivel.activo)
                    {
                        actualizar_estado_nivel(&estado_nivel, enemigos, num_enemigos_cargados, tiempo_cache, hay_jefe_en_nivel, &jefe_nivel);
                        // Disparos normales vs jefe
                        for (int j = 0; j < MAX_DISPAROS; j++) 
                        {
                            if (disparos[j].activo && detectar_colision_generica(disparos[j].x, disparos[j].y, 5, 10, jefe_nivel.x, jefe_nivel.y, jefe_nivel.ancho, jefe_nivel.alto))
                            {
                                if (jefe_recibir_dano(&jefe_nivel, 10, &cola_mensajes)) {
                                    puntaje += 50; // Puntos por golpear al jefe
                                }
                                else
                                {
                                    puntaje += 2000; // Gran bonificación por derrotar al jefe
                                    hay_jefe_en_nivel = false;
                                    hay_jefe_activo = false;
                                    jefe_nivel.activo = false;
                                    agregar_mensaje_cola(&cola_mensajes, "¡JEFE DERROTADO!", 5.0, al_map_rgb(255, 215, 0), true);
                                }
                                disparos[j].activo = false;
                            }
                        }
                        
                        // Láseres vs jefe
                        for (int j = 0; j < 5; j++)
                        {
                            if (lasers[j].activo)
                            {
                                float alcance_real = verificar_colision_laser_tilemap(lasers[j], tilemap);
                                if (laser_intersecta_enemigo_limitado(lasers[j], (Enemigo){jefe_nivel.x, jefe_nivel.y, jefe_nivel.ancho, jefe_nivel.alto, 0, 0, 0, true}, alcance_real))
                                {
                                    double tiempo_actual = al_get_time();
                                    if (tiempo_actual - lasers[j].ultimo_dano >= 0.1)
                                    {
                                        if (jefe_recibir_dano(&jefe_nivel, lasers[j].poder * 2, &cola_mensajes))
                                        {
                                            puntaje += 25;
                                        }
                                        else
                                        {
                                            puntaje += 2000;
                                            hay_jefe_en_nivel = false;
                                            hay_jefe_activo = false;
                                            jefe_nivel.activo = false;
                                            agregar_mensaje_cola(&cola_mensajes, "¡JEFE DERROTADO!", 5.0, al_map_rgb(255, 215, 0), true);
                                        }
                                        lasers[j].ultimo_dano = tiempo_actual;
                                    }
                                }
                            }
                        }
                        
                        // Explosivos vs jefe
                        for (int j = 0; j < 8; j++)
                        {
                            if (explosivos[j].activo && !explosivos[j].exploto)
                            {
                                if (detectar_colision_generica(
                                    explosivos[j].x, explosivos[j].y, explosivos[j].ancho, explosivos[j].alto,
                                    jefe_nivel.x, jefe_nivel.y, jefe_nivel.ancho, jefe_nivel.alto)) {
                                    
                                    if (jefe_recibir_dano(&jefe_nivel, explosivos[j].dano_directo * 2, &cola_mensajes))
                                    {
                                        puntaje += 100;
                                    }
                                    else
                                    {
                                        puntaje += 2000;
                                        hay_jefe_en_nivel = false;
                                        hay_jefe_activo = false;
                                        jefe_nivel.activo = false;
                                        agregar_mensaje_cola(&cola_mensajes, "¡JEFE DERROTADO!", 5.0, al_map_rgb(255, 215, 0), true);
                                    }
                                    explosivos[j].exploto = true;
                                    explosivos[j].tiempo_vida = al_get_time();
                                }
                            }
                        }
                        
                        // Misiles vs jefe
                        for (int j = 0; j < 6; j++)
                        {
                            if (misil[j].activo && detectar_colision_generica(
                                misil[j].x, misil[j].y, misil[j].ancho, misil[j].alto,
                                jefe_nivel.x, jefe_nivel.y, jefe_nivel.ancho, jefe_nivel.alto)) {
                                
                                if (jefe_recibir_dano(&jefe_nivel, misil[j].dano * 3, &cola_mensajes))
                                {
                                    puntaje += 150;
                                }
                                else
                                {
                                    puntaje += 2000;
                                    hay_jefe_en_nivel = false;
                                    hay_jefe_activo = false;
                                    jefe_nivel.activo = false;
                                    agregar_mensaje_cola(&cola_mensajes, "¡JEFE DERROTADO!", 5.0, al_map_rgb(255, 215, 0), true);
                                }
                                misil[j].activo = false;
                            }
                        }
                    }
                    else
                    {
                        actualizar_estado_nivel_sin_jefe(&estado_nivel, enemigos, num_enemigos_cargados, tiempo_cache);
                    }
                    
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

                        if (hay_jefe_en_nivel && jefe_nivel.activo) {
                            dibujar_jefe(jefe_nivel);
                            dibujar_ataques_jefe(jefe_nivel.ataques, MAX_ATAQUES_JEFE);
                        }

                        dibujar_powerups(powerups, MAX_POWERUPS, &contador_parpadeo_powerups, &contador_debug_powerups, fuente);

                        if (debug_mode)
                        {
                            dibujar_hitboxes_debug(nave, enemigos, num_enemigos_cargados, disparos, MAX_DISPAROS, disparos_enemigos, NUM_DISPAROS_ENEMIGOS, asteroides, NUM_ASTEROIDES, tilemap, fuente);
                        }
                        

                        dibujar_puntaje(puntaje, fuente);
                        dibujar_barra_vida(nave, fuente);
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

            hay_jefe_en_nivel = false;
            hay_jefe_activo = false;
            memset(&jefe_nivel, 0, sizeof(Jefe));
            jefe_nivel.activo = false;

            memset(teclas, false, sizeof(teclas)); // Reiniciar teclas

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

    liberar_imagenes_enemigos(imagenes_enemigos);

    if (imagen_jefe)
    {
        al_destroy_bitmap(imagen_jefe);
        imagen_jefe = NULL;
    }

    if (stream_musica_fondo)
    {
        al_set_audio_stream_playing(stream_musica_fondo, false);
        al_destroy_audio_stream(stream_musica_fondo);
        stream_musica_fondo = NULL;
        printf("Stream de música destruido al finalizar\n");
    }
    
    destruir_recursos(ventana, cola_eventos, temporizador, fuente, fondo_juego, imagen_nave, imagen_asteroide, imagen_enemigo, imagen_menu, stream_musica_fondo);

    al_uninstall_system(); // Esto evita fugas de memoria y libera recursos evitando el segmentation fault en WSL

    return 0;
}