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
    int j;
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
    ALLEGRO_BITMAP *imagen_jefe = NULL;
    ALLEGRO_EVENT evento;
    bool recargar_nivel;
    bool juego_terminado;
    Asteroide asteroides[NUM_ASTEROIDES];
    EstadoJuego estado_nivel;
    Nave nave;
    Disparo disparos[MAX_DISPAROS];
    Powerup powerups[MAX_POWERUPS];
    Disparo disparos_enemigos[NUM_DISPAROS_ENEMIGOS];
    Enemigo enemigos[NUM_ENEMIGOS];
    int tipo_jefe;
    int puntaje;
    ColaMensajes cola_mensajes;
    double tiempo_cache;
    bool laser_activo;
    int laser_desactivado;
    int siguiente_nivel;
    int nivel_disparo_radial_guardado;
    int kills_para_mejora_guardado;
    int tipo_nave_guardado;
    SistemaArma armas_guardadas[4];
    int clear_i;
    TipoArma arma_actual_guardada;
    int arma_seleccionada_guardada;
    int enemigos_a_copiar;
    bool hay_lasers_activos;
    int laser_check;
    bool hay_explosivos_activos;
    int exp_check;
    bool hay_misiles_activos;
    int mis_check;
    int enemigos_restantes;
    int z;
    char msg_enemigos[100];
    float alcance_real;
    double tiempo_actual;
    double tiempo_transcurrido;
    char texto_nivel[50];
    char nombre_jugador[MAX_NOMBRE];
    char texto_puntaje_final[100];
    bool esperando;
    ALLEGRO_EVENT victoria;
    int num_jugadores;
    ConfiguracionControl config_control;

    if (init_juego(&ventana, &cola_eventos, &temporizador, &fuente, &fondo_juego, &imagen_nave, &imagen_asteroide, &imagen_enemigo, &imagen_menu) != 0)
    {
        return -1;
    }

    init_configuracion_control(&config_control);

    printf("Estado del joystick: %s\n", config_control.joystick_disponible ? "Disponible" : "No disponible");

    // ✅ MOSTRAR MENÚ DE SELECCIÓN DE CONTROL
    if (config_control.joystick_disponible)
    {
        printf("Mostrando menú de selección de control...\n");
        mostrar_menu_seleccion_control(fuente, &config_control, cola_eventos);
    }
    else
    {
        printf("Solo teclado disponible. Usando teclado por defecto.\n");
        config_control.tipo_control = CONTROL_TECLADO;

        // ✅ CREAR TIMER PARA ESTA PANTALLA TAMBIÉN
        ALLEGRO_TIMER *timer_aviso = al_create_timer(1.0/60.0);
        al_register_event_source(cola_eventos, al_get_timer_event_source(timer_aviso));
        al_start_timer(timer_aviso);

        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 250, ALLEGRO_ALIGN_CENTER, 
                    "No se detectaron controladores");
        al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, 
                    "Usando teclado como control predeterminado");
        al_draw_text(fuente, al_map_rgb(200, 200, 200), 400, 350, ALLEGRO_ALIGN_CENTER, 
                    "Presiona cualquier tecla para continuar...");
        al_flip_display();
        
        bool esperando = true;
        bool necesita_redibujado = true;
        while (esperando)
        {
            ALLEGRO_EVENT evento_temp;
            al_wait_for_event(cola_eventos, &evento_temp);
            if (evento_temp.type == ALLEGRO_EVENT_KEY_DOWN || evento_temp.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                esperando = false;
            }

            // ✅ REDIBUJADO OPTIMIZADO
            if (evento_temp.type == ALLEGRO_EVENT_TIMER && necesita_redibujado)
            {
                necesita_redibujado = false;
            
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 250, ALLEGRO_ALIGN_CENTER, 
                        "No se detectaron controladores");
                al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, 
                        "Usando teclado como control predeterminado");
                al_draw_text(fuente, al_map_rgb(200, 200, 200), 400, 350, ALLEGRO_ALIGN_CENTER, 
                        "Presiona cualquier tecla para continuar...");
                al_flip_display();
            }
        }

        al_stop_timer(timer_aviso);
        al_unregister_event_source(cola_eventos, al_get_timer_event_source(timer_aviso));
        al_destroy_timer(timer_aviso);
    }
    
    // ✅ REGISTRAR EVENTOS DE JOYSTICK SI ES NECESARIO
    if (config_control.tipo_control == CONTROL_JOYSTICK && config_control.joystick)
    {
        al_register_event_source(cola_eventos, al_get_joystick_event_source());
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

            // ✅ AGREGAR NAVEGACIÓN CON TECLADO Y JOYSTICK AL MENÚ PRINCIPAL
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                static int opcion_menu = 0;
        
                switch (evento.keyboard.keycode)
                {
                case ALLEGRO_KEY_UP:
                case ALLEGRO_KEY_W:
                    opcion_menu = (opcion_menu - 1 + 3) % 3;
                    cursor_x = botones[opcion_menu].x + botones[opcion_menu].ancho / 2;
                    cursor_y = botones[opcion_menu].y + botones[opcion_menu].alto / 2;
                    break;
                
                case ALLEGRO_KEY_DOWN:
                case ALLEGRO_KEY_S:
                    opcion_menu = (opcion_menu + 1) % 3;
                    cursor_x = botones[opcion_menu].x + botones[opcion_menu].ancho / 2;
                    cursor_y = botones[opcion_menu].y + botones[opcion_menu].alto / 2;
                    break;
                
                case ALLEGRO_KEY_ENTER:
                case ALLEGRO_KEY_SPACE:
                    if (opcion_menu == 0)
                    {
                        en_menu = false;
                        jugando = true;
                    }
                    else if (opcion_menu == 1)
                    {
                        en_menu = false;
                        mostrarRanking = true;
                    }
                    else if (opcion_menu == 2)
                    {
                        en_menu = false;
                        jugando = false;
                        mostrarRanking = false;
                    }
                    break;
                
                case ALLEGRO_KEY_ESCAPE:
                    en_menu = false;
                    jugando = false;
                    mostrarRanking = false;
                    break;
                }
            }

            // ✅ SOPORTE PARA JOYSTICK EN EL MENÚ PRINCIPAL
            if (config_control.tipo_control == CONTROL_JOYSTICK && config_control.joystick)
            {
                static int opcion_menu_joy = 0;
                static double ultimo_input_menu = 0;
                const double delay_menu = 0.3;
        
                if (evento.type == ALLEGRO_EVENT_JOYSTICK_AXIS)
                {
                    double tiempo_actual = al_get_time();
            
                    if (evento.joystick.axis == 1 && tiempo_actual - ultimo_input_menu > delay_menu)
                    {
                        if (evento.joystick.pos < -DEADZONE_JOYSTICK)
                        {
                            opcion_menu_joy = (opcion_menu_joy - 1 + 3) % 3;
                            cursor_x = botones[opcion_menu_joy].x + botones[opcion_menu_joy].ancho / 2;
                            cursor_y = botones[opcion_menu_joy].y + botones[opcion_menu_joy].alto / 2;
                            ultimo_input_menu = tiempo_actual;
                        }
                        else if (evento.joystick.pos > DEADZONE_JOYSTICK)
                        {
                            opcion_menu_joy = (opcion_menu_joy + 1) % 3;
                            cursor_x = botones[opcion_menu_joy].x + botones[opcion_menu_joy].ancho / 2;
                            cursor_y = botones[opcion_menu_joy].y + botones[opcion_menu_joy].alto / 2;
                            ultimo_input_menu = tiempo_actual;
                        }
                    }
                }
        
                if (evento.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
                {
                    if (evento.joystick.button == 0) // Botón A/X
                    {
                        if (opcion_menu_joy == 0)
                        {
                            en_menu = false;
                            jugando = true;
                        }
                        else if (opcion_menu_joy == 1)
                        {
                            en_menu = false;
                            mostrarRanking = true;
                        }
                        else if (opcion_menu_joy == 2)
                        {
                            en_menu = false;
                            jugando = false;
                            mostrarRanking = false;
                        }
                    }
                    else if (evento.joystick.button == 1) // Botón B/Circle
                    {
                        en_menu = false;
                        jugando = false;
                        mostrarRanking = false;
                    }
                }
            }

            if (evento.type == ALLEGRO_EVENT_TIMER)
            {
                // ✅ USAR BACKBUFFER PARA MEJOR RENDIMIENTO
                al_set_target_backbuffer(al_get_current_display());

                if (imagen_menu)
                {
                    al_draw_scaled_bitmap(imagen_menu, 0, 0, al_get_bitmap_width(imagen_menu), al_get_bitmap_height(imagen_menu), 0, 0, 800, 600, 0);
                }
                else
                {
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                }
                
                dibujar_botones(botones, 3, fuente, cursor_x, cursor_y);

                // ✅ MOSTRAR CONTROLES DISPONIBLES
                if (config_control.joystick_disponible)
                {
                    al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 570, ALLEGRO_ALIGN_LEFT, "Usa flechas/stick para navegar, Enter/A para seleccionar");
                }
                else
                {
                    al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 570, ALLEGRO_ALIGN_LEFT, "Usa flechas para navegar, Enter para seleccionar");
                }

                al_flip_display();
            }
        }
    
        if (jugando)
        {   
            hay_jefe_en_nivel = false;
            memset(&jefe_final, 0, sizeof(Jefe));
            jefe_nivel.activo = false;

            memset(teclas, false, sizeof(teclas)); // Reiniciar teclas

            // Recargo el nivel 1 desde cero
            cargar_tilemap("Nivel1.txt", tilemap, enemigos_mapa, &num_enemigos_cargados, imagen_enemigo, &nave_x_inicial, &nave_y_inicial);

            // Inicializar estado del juego
            init_estado_juego(&estado_nivel);

            recargar_nivel = false;
            juego_terminado = false;

            // Inicializar asteroides
            init_asteroides(asteroides, NUM_ASTEROIDES, 800, imagen_asteroide);

            // Inicializar nave
            nave = init_nave(nave_x_inicial, nave_y_inicial, 50, 50, 100.0f, 0.1, imagen_nave);

            memset(teclas, false, sizeof(teclas)); // Reiniciar teclas

            // Inicializar el sistema de armas
            init_sistema_armas(&nave);

            for (i = 0; i < 5; i++)
            {
                lasers[i].activo = false;
            }

            for (i = 0; i < 8; i++)
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

            for (i = 0; i < 6; i++)
            {
                misil[i].activo = false;
            }

            // Inicializar disparos
            init_disparos(disparos, MAX_DISPAROS);

            // Inicializar Powerups
            for (k = 0; k < MAX_POWERUPS; k++)
            {
                init_powerup(&powerups[k]);
            }
            
            // Inicializar Enemigos
            for (i = 0; i < num_enemigos_cargados && i < NUM_ENEMIGOS; i++)
            {
                enemigos[i] = enemigos_mapa[i];
                asignar_imagen_enemigo(&enemigos[i], imagenes_enemigos); // Asegurar que usen el sprite correcto
            }
            // Inicializar el resto como inactivos
            for (i = num_enemigos_cargados; i < NUM_ENEMIGOS; i++) 
            {
                enemigos[i].activo = false;
            }

            hay_jefe_en_nivel = false;

            printf("Verificando jefes para nivel %d con %d enemigos cargados\n", estado_nivel.nivel_actual, num_enemigos_cargados);

            if (estado_nivel.nivel_actual >= 4)
            {
                for (i = 0; i < num_enemigos_cargados; i++)
                {
                    printf("Enemigo %d: tipo %d en (%.0f, %.0f)\n", i, enemigos_mapa[i].tipo, enemigos_mapa[i].x, enemigos_mapa[i].y);

                    if (enemigos_mapa[i].tipo == 5 || enemigos_mapa[i].tipo == 6) 
                    {
                        tipo_jefe = enemigos_mapa[i].tipo == 5 ? 0 : 1; // 5=Destructor(0), 6=Supremo(1)
                        init_jefe(&jefe_nivel, tipo_jefe, enemigos_mapa[i].x, enemigos_mapa[i].y, imagen_jefe);
                        hay_jefe_en_nivel = true;
                        printf("Jefe encontrado en nivel %d: tipo %d\n", estado_nivel.nivel_actual, tipo_jefe);
                    
                        // Remover jefe del array de enemigos normales
                        for (k = i; k < num_enemigos_cargados - 1; k++) 
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
            puntaje = 0;

            // Inicializar Mensajes
            init_cola_mensajes(&cola_mensajes);
            
            // ✅ MOSTRAR INSTRUCCIONES INICIALES SEGÚN TIPO DE CONTROL
            if (config_control.tipo_control == CONTROL_JOYSTICK && config_control.joystick_disponible)
            {
                agregar_mensaje_cola(&cola_mensajes, "¡Controles de Joystick Activados!", 4.0, al_map_rgb(0, 255, 0), true);
                agregar_mensaje_cola(&cola_mensajes, "Stick izquierdo: Mover", 3.0, al_map_rgb(255, 255, 255), true);
                agregar_mensaje_cola(&cola_mensajes, "Stick derecho: Rotar", 3.0, al_map_rgb(255, 255, 255), true);
                agregar_mensaje_cola(&cola_mensajes, "Botón A/X: DISPARAR", 3.0, al_map_rgb(255, 255, 0), true);
                agregar_mensaje_cola(&cola_mensajes, "Cruceta/D-pad: Cambiar Arma", 3.0, al_map_rgb(255, 255, 255), true);
            }
            else
            {
                agregar_mensaje_cola(&cola_mensajes, "Stick izquierdo: Mover libremente", 3.5, al_map_rgb(255, 255, 255), true);
                agregar_mensaje_cola(&cola_mensajes, "Stick derecho: Rotar nave", 3.5, al_map_rgb(255, 255, 255), true);
                agregar_mensaje_cola(&cola_mensajes, "A/X: Disparo, D-pad: Cambiar Arma", 3.5, al_map_rgb(255, 255, 0), true);
            }

            tiempo_cache = 0;

            /*Bucle del juego*/
            while (jugando && !juego_terminado) 
            {
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
                                laser_activo = false;
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
                }
                    
                    if (evento.type == ALLEGRO_EVENT_KEY_UP && evento.keyboard.keycode == ALLEGRO_KEY_SPACE)
                    {
                        if (nave.arma_actual == Arma_laser)
                        {
                            laser_desactivado = 0;
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
                    

                        if (config_control.tipo_control == CONTROL_JOYSTICK && config_control.joystick)
                        {
                            if (evento.type == ALLEGRO_EVENT_JOYSTICK_AXIS || evento.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN || evento.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP)
                            {
                                if (!estado_nivel.mostrar_transicion)
                                {
                                    manejar_eventos_joystick(evento, &nave, teclas);
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

                    // ✅ DISPARO CON JOYSTICK
                    if (config_control.tipo_control == CONTROL_JOYSTICK && config_control.joystick)
                    {
                        debug_joystick_estado(config_control.joystick);
                        cambiar_arma_joystick(&nave, config_control.joystick);

                        static bool boton_disparar_presionado = false;
                        bool boton_actual = obtener_boton_joystick_disparar(config_control.joystick);
                
                        // Detectar cuando se presiona el botón (no mantener presionado)
                        if (boton_actual && !boton_disparar_presionado)
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
                                    printf("Laser activado con joystick\n");
                                }
                            }
                            else
                            {
                                disparar_segun_arma(nave, disparos, MAX_DISPAROS, lasers, 5, explosivos, 8, misil, 6, enemigos, num_enemigos_cargados);
                            }
                        }
                
                        // Para láser continuo: desactivar cuando se suelta el botón
                        if (!boton_actual && boton_disparar_presionado && nave.arma_actual == Arma_laser)
                        {
                            for (i = 0; i < 5; i++)
                            {
                                if (lasers[i].activo)
                                {
                                    lasers[i].activo = false;
                                }
                            }
                            printf("Laser desactivado con joystick\n");
                        }
                
                        boton_disparar_presionado = boton_actual;
                    }

                    actualizar_cola_mensajes(&cola_mensajes, tiempo_cache);

                    // Verificar si necesitamos cargar el siguiente nivel
                    if (estado_nivel.nivel_completado && !estado_nivel.mostrar_transicion && !recargar_nivel)
                    {
                        recargar_nivel = true;
                        estado_nivel.nivel_completado = false;
                    }

                    // Cargar siguiente nivel si es necesario
                    if (recargar_nivel)
                    {
                        siguiente_nivel = estado_nivel.nivel_actual + 1;

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

                            for (clear_i = 0; clear_i < MAX_DISPAROS; clear_i++)
                            {
                                disparos[clear_i].activo = false;
                            }

                            for (clear_i = 0; clear_i < NUM_DISPAROS_ENEMIGOS; clear_i++)
                            {
                                disparos_enemigos[clear_i].activo = false;
                            }

                            for (clear_i = 0; clear_i < 5; clear_i++)
                            {
                                lasers[clear_i].activo = false;
                            }

                            for (clear_i = 0; clear_i < 8; clear_i++)
                            {
                                explosivos[clear_i].activo = false;
                            }

                            for (clear_i = 0; clear_i < 6; clear_i++)
                            {
                                misil[clear_i].activo = false;
                            }

                            // Guardar el estado de la nave antes de reinicializarla
                            nivel_disparo_radial_guardado = nave.nivel_disparo_radial;
                            kills_para_mejora_guardado = nave.kills_para_mejora;
                            tipo_nave_guardado = nave.tipo;

                            for (i = 0; i < 4; i++)
                            {
                                armas_guardadas[i] = nave.armas[i];
                            }
                            
                            arma_actual_guardada = nave.arma_actual;
                            arma_seleccionada_guardada = nave.arma_seleccionada;

                            nave.escudo.activo = false;

                            // Reinicializar la nave con los valores guardados
                            nave = init_nave(nave_x_inicial, nave_y_inicial, 50, 50, 100, 0.1, imagen_nave);

                            // Restaurar los valores guardados
                            nave.nivel_disparo_radial = nivel_disparo_radial_guardado;
                            nave.kills_para_mejora = kills_para_mejora_guardado;
                            nave.tipo = tipo_nave_guardado;

                            for (i = 0; i < 4; i++)
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
                                for (k = 0; k < NUM_ASTEROIDES; k++)
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
                                for (k = 0; k < NUM_ASTEROIDES; k++)
                                {
                                    asteroides[k].y = -1000; // Fuera de la pantalla
                                    asteroides[k].x = -1000;
                                    asteroides[k].velocidad = 0; // Sin movimiento
                                }
                                printf("Asteroides DESACTIVADOS en nivel %d\n", estado_nivel.nivel_actual);
                            }

                            // RECARGAR ENEMIGOS
                            enemigos_a_copiar = (num_enemigos_cargados < NUM_ENEMIGOS) ? num_enemigos_cargados : NUM_ENEMIGOS;

                            for (k = 0; k < enemigos_a_copiar; k++) 
                            {
                                enemigos[k] = enemigos_mapa[k];
                                asignar_imagen_enemigo(&enemigos[k], imagenes_enemigos); // Asegurar que usen el sprite correcto
                                enemigos[k].activo = true;
                            }

                            hay_jefe_en_nivel = false;
                            memset(&jefe_nivel, 0, sizeof(Jefe));
                            jefe_nivel.activo = false;

                            for (k = 0; k < num_enemigos_cargados; k++)
                            {
                                printf("Enemigo recarga %d: tipo %d en (%.0f, %.0f)\n", k, enemigos_mapa[k].tipo, enemigos_mapa[k].x, enemigos_mapa[k].y);

                                if (enemigos_mapa[k].tipo == 5 || enemigos_mapa[k].tipo == 6)
                                {
                                    tipo_jefe = enemigos_mapa[k].tipo == 5 ? 0 : 1;
                                    init_jefe(&jefe_nivel, tipo_jefe, enemigos_mapa[k].x, enemigos_mapa[k].y, imagen_jefe);
                                    hay_jefe_en_nivel = true;
                                    printf("Jefe cargado en nivel %d: enemigo tipo %d -> jefe tipo %d\n", estado_nivel.nivel_actual, enemigos_mapa[k].tipo, tipo_jefe);

                                    for (j = k; j < num_enemigos_cargados - 1; j++) 
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
                        for(k = 0; k < ALLEGRO_KEY_MAX; k++)
                        {
                            teclas[k] = false; // Reiniciar teclas para evitar problemas de movimiento
                        }
                        
                        agregar_mensaje_cola(&cola_mensajes, "¡Nueva Movilidad Desbloqueada!", 4.0, al_map_rgb(0, 255, 0), true);
                        
                        // ✅ MOSTRAR INSTRUCCIONES SEGÚN TIPO DE CONTROL
                        if (config_control.tipo_control == CONTROL_JOYSTICK && config_control.joystick_disponible)
                        {
                            agregar_mensaje_cola(&cola_mensajes, "Stick izquierdo: Mover libremente", 3.5, al_map_rgb(255, 255, 255), true);
                            agregar_mensaje_cola(&cola_mensajes, "Stick derecho: Rotar nave", 3.5, al_map_rgb(255, 255, 255), true);
                        }
                        else
                        {
                            agregar_mensaje_cola(&cola_mensajes, "Usa las flechas para rotar y avanzar", 3.0, al_map_rgb(255, 255, 255), true);
                        }
                    }

                    hay_lasers_activos = false;
                    for (laser_check = 0; laser_check < 5; laser_check++)
                    {
                        if (lasers[laser_check].activo)
                        {
                            hay_lasers_activos = true;
                            break;
                        }
                    }

                    if (hay_lasers_activos)
                    {
                        actualizar_lasers(lasers, 5, enemigos, num_enemigos_cargados, &puntaje, &nave, tilemap, &contador_debug_lasers, powerups, MAX_POWERUPS, &cola_mensajes);
                    }
                    
                    hay_explosivos_activos = false;
                    for (exp_check = 0; exp_check < 8; exp_check++)
                    {
                        if (explosivos[exp_check].activo)
                        {
                            hay_explosivos_activos = true;
                            break;
                        }
                    }

                    if (hay_explosivos_activos)
                    {
                        actualizar_explosivos(explosivos, 8, enemigos, num_enemigos_cargados, &puntaje, tilemap, &nave, &cola_mensajes);
                    }
                    
                    hay_misiles_activos = false;
                    for (mis_check = 0; mis_check < 6; mis_check++)
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
                        for (k = 0; k < MAX_ATAQUES_JEFE; k++)
                        {
                            if (jefe_nivel.ataques[k].activo && detectar_colision_ataque_jefe_nave(jefe_nivel.ataques[k], nave))
                            {
                                if (escudo_recibir_dano(&nave.escudo))
                                {
                                    printf("Escudo absorbió ataque del jefe\n");
                                }
                                else
                                {
                                    nave.vida -= jefe_nivel.ataques[k].dano;
                                    printf("Jefe causo %.1f de daño. Vida restante: %.1f\n", jefe_nivel.ataques[k].dano, nave.vida);
                                    agregar_mensaje_cola(&cola_mensajes, "¡Ataque del Jefe!", 2.0, al_map_rgb(255, 0, 0), false);
                                }
                                jefe_nivel.ataques[k].activo = false;
                            }
                        }
                    }

                    // ✅ ACTUALIZAR JUEGO CON SOPORTE DE JOYSTICK
                    if (config_control.tipo_control == CONTROL_JOYSTICK && config_control.joystick)
                    {
                        actualizar_nave_joystick(&nave, config_control.joystick, tilemap);
                    }

                    actualizar_juego(&nave, teclas, asteroides, 10, disparos, 10, &puntaje, tilemap, enemigos, num_enemigos_cargados, disparos_enemigos, NUM_DISPAROS_ENEMIGOS, &cola_mensajes, &estado_nivel, tiempo_cache, powerups, MAX_POWERUPS);
                    
                    if (hay_jefe_en_nivel && jefe_nivel.activo)
                    {
                        actualizar_estado_nivel(&estado_nivel, enemigos, num_enemigos_cargados, tiempo_cache, hay_jefe_en_nivel, &jefe_nivel);

                        enemigos_restantes = 0;

                        for (z = 0; z < num_enemigos_cargados; z++)
                        {
                            if (enemigos[z].activo) 
                            {
                                enemigos_restantes++;
                            }
                        }

                        printf("Estado nivel %d: Jefe activo, Enemigos restantes: %d\n", estado_nivel.nivel_actual, enemigos_restantes);
                        
                        // Disparos normales vs jefe
                        for (j = 0; j < MAX_DISPAROS; j++) 
                        {
                            if (disparos[j].activo && detectar_colision_generica(disparos[j].x, disparos[j].y, 5, 10, jefe_nivel.x, jefe_nivel.y, jefe_nivel.ancho, jefe_nivel.alto))
                            {
                                if (jefe_recibir_dano(&jefe_nivel, 10, &cola_mensajes))
                                {
                                    puntaje += 50; // Puntos por golpear al jefe
                                }
                                else
                                {
                                    puntaje += 2000; // Gran bonificación por derrotar al jefe
                                    hay_jefe_en_nivel = false;
                                    jefe_nivel.activo = false;

                                    enemigos_restantes = 0;

                                    for (z = 0; z < num_enemigos_cargados; z++)
                                    {
                                        if (enemigos[z].activo) enemigos_restantes++;
                                    }
                                    
                                    if (enemigos_restantes > 0)
                                    {
                                        agregar_mensaje_cola(&cola_mensajes, "JEFE DERROTADO!", 3.0, al_map_rgb(255, 215, 0), true);
                                        sprintf(msg_enemigos, "Elimina los %d enemigos restantes", enemigos_restantes);
                                        agregar_mensaje_cola(&cola_mensajes, msg_enemigos, 4.0, al_map_rgb(255, 255, 255), true);
                                    }
                                    else
                                    {
                                        agregar_mensaje_cola(&cola_mensajes, "¡JEFE DERROTADO!", 3.0, al_map_rgb(255, 215, 0), true);
                                        agregar_mensaje_cola(&cola_mensajes, "¡NIVEL COMPLETADO!", 3.0, al_map_rgb(0, 255, 0), true);
                                    }
                                }
                                disparos[j].activo = false;
                            }
                        }
                        
                        // Láseres vs jefe
                        for (j = 0; j < 5; j++)
                        {
                            if (lasers[j].activo)
                            {
                                alcance_real = verificar_colision_laser_tilemap(lasers[j], tilemap);
                                Enemigo enemigo_jefe_temp = 
                                {
                                    .x = jefe_nivel.x,
                                    .y = jefe_nivel.y, 
                                    .ancho = jefe_nivel.ancho,
                                    .alto = jefe_nivel.alto,
                                    .velocidad = 0,
                                    .vida = 0,
                                    .vida_max = 0,
                                    .activo = true,
                                    .ultimo_disparo = 0,
                                    .intervalo_disparo = 0,
                                    .imagen = NULL,
                                    .tipo = 0
                                };

                                if (laser_intersecta_enemigo_limitado(lasers[j], enemigo_jefe_temp, alcance_real))
                                {
                                    tiempo_actual = al_get_time();
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
                                            jefe_nivel.activo = false;
                                            agregar_mensaje_cola(&cola_mensajes, "JEFE DERROTADO!", 5.0, al_map_rgb(255, 215, 0), true);
                                        }
                                        lasers[j].ultimo_dano = tiempo_actual;
                                    }
                                }
                            }
                        }
                        
                        // Explosivos vs jefe
                        for (j = 0; j < 8; j++)
                        {
                            if (explosivos[j].activo && !explosivos[j].exploto)
                            {
                                if (detectar_colision_generica(explosivos[j].x, explosivos[j].y, explosivos[j].ancho, explosivos[j].alto, jefe_nivel.x, jefe_nivel.y, jefe_nivel.ancho, jefe_nivel.alto))
                                {
                                    if (jefe_recibir_dano(&jefe_nivel, explosivos[j].dano_directo * 2, &cola_mensajes))
                                    {
                                        puntaje += 100;
                                    }
                                    else
                                    {
                                        puntaje += 2000;
                                        hay_jefe_en_nivel = false;
                                        jefe_nivel.activo = false;
                                        agregar_mensaje_cola(&cola_mensajes, "¡JEFE DERROTADO!", 5.0, al_map_rgb(255, 215, 0), true);
                                    }
                                    explosivos[j].exploto = true;
                                    explosivos[j].tiempo_vida = al_get_time();
                                }
                            }
                        }
                        
                        // Misiles vs jefe
                        for (j = 0; j < 6; j++)
                        {
                            if (misil[j].activo && detectar_colision_generica(misil[j].x, misil[j].y, misil[j].ancho, misil[j].alto, jefe_nivel.x, jefe_nivel.y, jefe_nivel.ancho, jefe_nivel.alto))
                            {
                                if (jefe_recibir_dano(&jefe_nivel, misil[j].dano * 3, &cola_mensajes))
                                {
                                    puntaje += 150;
                                }
                                else
                                {
                                    puntaje += 2000;
                                    hay_jefe_en_nivel = false;
                                    jefe_nivel.activo = false;
                                    agregar_mensaje_cola(&cola_mensajes, "JEFE DERROTADO!", 5.0, al_map_rgb(255, 215, 0), true);
                                }
                                misil[j].activo = false;
                            }
                        }
                    }
                    else if (hay_jefe_en_nivel && !jefe_nivel.activo)
                    {
                        actualizar_estado_nivel(&estado_nivel, enemigos, num_enemigos_cargados, tiempo_cache, hay_jefe_en_nivel, &jefe_nivel);

                        enemigos_restantes = 0;

                        for (z = 0; z < num_enemigos_cargados; z++)
                        {
                            if (enemigos[z].activo) enemigos_restantes++;
                        }
                        
                        if (enemigos_restantes == 0)
                        {
                            printf("Nivel %d completado! Jefe derrotado y todos los enemigos eliminados\n", estado_nivel.nivel_actual);
                        }
                        else
                        {
                            printf("Jefe derrotado pero quedan %d enemigos en nivel %d\n", enemigos_restantes, estado_nivel.nivel_actual);
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
                        tiempo_transcurrido = al_get_time() - estado_nivel.tiempo_inicio_transicion;
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

                        if (hay_jefe_en_nivel && jefe_nivel.activo)
                        {
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
                        dibujar_info_escudo(nave, fuente);
                        
                        // ✅ MOSTRAR INDICADOR DE CONTROL
                        dibujar_indicador_control(config_control, fuente);

                        dibujar_cola_mensajes(cola_mensajes, fuente);
                        
                        // Mostrar nivel actual
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
                    if (juego_terminado)
                    {
                        jugando = false;
                        
                        // Mostrar mensaje de victoria antes de pedir el nombre
                        al_clear_to_color(al_map_rgb(0, 0, 0));
                        al_draw_text(fuente, al_map_rgb(0, 255, 0), 400, 250, ALLEGRO_ALIGN_CENTER, "FELICIDADES!");
                        al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, "Has completado todos los niveles!");
                        sprintf(texto_puntaje_final, "Puntaje final: %d", puntaje);
                        al_draw_text(fuente, al_map_rgb(255, 255, 0), 400, 350, ALLEGRO_ALIGN_CENTER, texto_puntaje_final);
                        al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 400, ALLEGRO_ALIGN_CENTER, "Presiona cualquier tecla para continuar...");
                        al_flip_display();

                        esperando = true;
                        while (esperando)
                        {
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
    
    destruir_recursos(ventana, cola_eventos, temporizador, fuente, fondo_juego, imagen_nave, imagen_asteroide, imagen_enemigo, imagen_menu);

    al_uninstall_system(); // Esto evita fugas de memoria y libera recursos evitando el segmentation fault en WSL

    return 0;
}