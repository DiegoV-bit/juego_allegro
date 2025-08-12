#include "juego.h"

/**
 * @file juego.c
 * @brief Este archivo contiene todas las funciones referentes al juego.
 *
*/

/**
 * @brief Inicializa la nave.
 *
 * Esta funcion Inicializa la nave dandole una posicion en los ejes "x" e "y", al igual que le da un tamano y angulo inicial.
 * 
 * @param x Posicion en el eje x.
 * @param y posicion en el eje y.
 * @param ancho Anchura de la nave.
 * @param largo Largo de la nave. 
 * @param vida Vida de la nave.
 * @param tiempo_invulnerable Tiempo que la nave es invulnerable.
 * @param imagen_nave Imagen de la nave.
 * @return Nave inicializada.
 */
Nave init_nave(float x, float y, float ancho, float largo, float vida, double tiempo_invulnerable, ALLEGRO_BITMAP *imagen_nave)
{
    int i;

    // Inicializa la nave con los parámetros dados
    Nave nave;
    nave.x = x;
    nave.y = y;
    nave.ancho = ancho;
    nave.largo = largo;
    nave.vida = vida;
    nave.tiempo_invulnerable = tiempo_invulnerable;
    nave.tiempo_ultimo_dano = -nave.tiempo_invulnerable;
    nave.imagen = imagen_nave;
    nave.angulo = 0.0f;
    nave.tipo = 0;
    nave.nivel_disparo_radial = 0;
    nave.kills_para_mejora = 0;

    init_escudo(&nave.escudo);

    for(i = 0; i < MAX_DISPAROS; i++)
    {
        nave.disparos[i].activo = false;
    }

    return nave;
}

/**
 * @brief Inicializa los asteroides
 * 
 * Esta funcion inicializa los asteroides dandole una posicion en los ejes "x" e "y", al igual que le da un tamano
 * 
 * @param asteroides Arreglo de asteroides a inicializar.
 * @param num_asteroides Numero de asteroides en el arreglo.
 * @param ancho_ventana Anchura de los asteroides.
 * @param imagen_asteroide Sprite de los asteroides.
 */ 
void init_asteroides(Asteroide asteroides[], int num_asteroides, int ancho_ventana, ALLEGRO_BITMAP* imagen_asteroide)
{
    int i;

    for (i = 0; i < num_asteroides; i++)
    {
        asteroides[i].x = rand() % (ancho_ventana - 50);
        asteroides[i].y = rand() % 600 - 600;
        asteroides[i].velocidad = 1.5;
        asteroides[i].ancho = 50;
        asteroides[i].alto = 50;
        asteroides[i].imagen = imagen_asteroide;
    }
}

/**
 * @brief Actualiza la posición de un asteroide.
 *
 * Esta función mueve el asteroide hacia abajo según su velocidad. Si el asteroide sale de la pantalla,
 * se reinicia su posición en la parte superior.
 *
 * @param asteroide Puntero al asteroide a actualizar.
 * @param tilemap Mapa de tiles del juego, usado para detectar colisiones.
 * @param nave Puntero a la nave, usado para detectar colisiones.
 * @param powerups Arreglo de powerups, usado para detectar colisiones.
 * @param max_powerups Tamaño del arreglo de powerups.
 */
void actualizar_asteroide(Asteroide* asteroide, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Nave* nave, Powerup powerups[], int max_powerups)
{
    // Verifica colisión con la nave
    float centro_nave_x, centro_nave_y;
    obtener_centro_nave(*nave, &centro_nave_x, &centro_nave_y);
    float radio_nave = obtener_radio_nave(*nave);
    float centro_asteroide_x = asteroide->x + asteroide->ancho / 2;
    float centro_asteroide_y = asteroide->y + asteroide->alto / 2;
    float radio_asteroide = asteroide->ancho / 2.0f;

    // Verifica colisión con escudos del tilemap (MEJORADO)
    int col_izquierda = (int)(asteroide->x / TILE_ANCHO);
    int col_derecha = (int)((asteroide->x + asteroide->ancho - 1) / TILE_ANCHO);
    int fila_superior = (int)(asteroide->y / TILE_ALTO);
    int fila_inferior = (int)((asteroide->y + asteroide->alto - 1) / TILE_ALTO);

    int fila;
    int col;
    int i;

    asteroide->y += asteroide->velocidad;

    if (detectar_colision_circular(centro_nave_x, centro_nave_y, radio_nave, centro_asteroide_x, centro_asteroide_y, radio_asteroide))
    {
        if (escudo_recibir_dano(&nave->escudo))
        {
            printf("El escudo absorbió el daño del asteroide\n");
        }
        else
        {
            nave->vida -= 20.0f;
            printf("La nave recibió 10 de daño, Vida restante: %.1f\n", nave->vida);
        }

        // Asteroide desaparece tras impactar
        asteroide->y = -asteroide->alto;
        asteroide->x = rand() % (800 - (int)asteroide->ancho);
        return;
    }

    for (i = 0; i < max_powerups; i++)
    {
        if (powerups[i].activo)
        {
            if (detectar_colision_generica(asteroide->x, asteroide->y, asteroide->ancho, asteroide->alto, powerups[i].x, powerups[i].y, 30, 30))
            {
                powerups[i].activo = false;
                //asteroide->y = -asteroide->alto;
                //asteroide->x = rand() % (800 - (int)asteroide->ancho);

                //const char *tipo_powerup = (powerups[i].tipo == 0) ? "ESCUDO" : (powerups[i].tipo == 1) ? "VIDA" : "DESCONOCIDO";
                printf("Asteroide destruyó powerup\n");
            }
        }
    }

    // Verifica TODAS las celdas que ocupa el asteroide
    for (fila = fila_superior; fila <= fila_inferior; fila++)
    {
        for (col = col_izquierda; col <= col_derecha; col++)
        {
            if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS)
            {
                Tile *tile = &tilemap[fila][col];

                if (tile->tipo == 3)
                {
                    asteroide->y = -asteroide->alto;
                    asteroide->x = rand() % (800 - (int)asteroide->ancho);
                    printf("Asteroide rebotó en bloque sólido\n");
                    return;
                }
                else if (tile->tipo == 1)
                {
                    asteroide->y = -asteroide->alto;
                    asteroide->x = rand() % (800 - (int)asteroide->ancho);
                    printf("Asteroide rebotó en asteroide fijo\n");
                    return;
                }
                else if (tile->tipo == 2 && tile->vida > 0)
                {
                    tile->vida--;
                    printf("Asteroide dañó escudo en (%d, %d). Vida restante: %d\n", col, fila, tile->vida);
                    
                    if (tile->vida <= 0)
                    {
                        printf("Escudo del mapa destruido por asteroide en (%d, %d)\n", col, fila);
                    }

                    asteroide->y = -asteroide->alto;
                    asteroide->x = rand() % (800 - (int)asteroide->ancho);
                    return;
                }
            }
        }
    }

    // Si el asteroide sale de la pantalla, reaparece arriba
    if (asteroide->y > 600)
    {
        asteroide->y = -asteroide->alto;
        asteroide->x = rand() % (800 - (int)asteroide->ancho);
    }
}


/**
   * @brief Maneja los eventos de teclado para mover la nave.
   *
   * Esta función actualiza la posición de la nave en función de las teclas presionadas.
   *
   * @param evento El evento de teclado que se va a manejar.
   * @param nave Puntero a la nave que se va a mover.
   * @param teclas Arreglo de teclas presionadas.
   * @param disparos Arreglo de disparos.
   * @param num_disparos Número de disparos en el arreglo.
   */
void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave, bool teclas[])
{
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch (evento.keyboard.keycode)
        {
        case ALLEGRO_KEY_UP:
            teclas[0] = true;
            break;

        case ALLEGRO_KEY_DOWN:
            teclas[1] = true;
            break;

        case ALLEGRO_KEY_LEFT:
            teclas[2] = true;
            break;

        case ALLEGRO_KEY_RIGHT:
            teclas[3] = true;
            break;

        case ALLEGRO_KEY_SPACE:
            // disparar_radial(disparos, num_disparos, *nave);
            break;

        case ALLEGRO_KEY_1:
            cambiar_arma(nave, Arma_normal);
            break;

        case ALLEGRO_KEY_2:
            cambiar_arma(nave, Arma_laser);
            break;

        case ALLEGRO_KEY_3:
            cambiar_arma(nave, Arma_explosiva);
            break;
            
        case ALLEGRO_KEY_4:
            cambiar_arma(nave, Arma_misil);
            break;
        }
    }
    else if (evento.type == ALLEGRO_EVENT_KEY_UP)
    {
        switch (evento.keyboard.keycode)
        {
            case ALLEGRO_KEY_UP:
                teclas[0] = false;
                break;
            case ALLEGRO_KEY_DOWN:
                teclas[1] = false;
                break;
            case ALLEGRO_KEY_LEFT:
                teclas[2] = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                teclas[3] = false;
                break;
            case ALLEGRO_KEY_SPACE:
            break;
        }
    }
}

/**
 * @brief Dibuja la nave y los asteroides en la pantalla.
 *
 * Esta función dibuja la nave y todos los asteroides en sus posiciones actuales.
 *
 * Ahora se dibuja la imagen de la nave reescalada y rotada según su ángulo.
 * @param nave La nave a dibujar.
 * @param asteroides Arreglo de asteroides a dibujar.
 * @param num_asteroides Número de asteroides en el arreglo.
 * @param imagen_fondo Imagen de fondo del juego.
 */
void dibujar_juego(Nave nave, Asteroide asteroides[], int num_asteroides, int nivel_actual, ALLEGRO_BITMAP *imagen_fondo)
{
    float cx = al_get_bitmap_width(nave.imagen) / 2.0f;
    float cy = al_get_bitmap_height(nave.imagen) / 2.0f;
    float escala_x = nave.ancho / al_get_bitmap_width(nave.imagen);
    float escala_y = nave.largo / al_get_bitmap_height(nave.imagen);
    int i;

    if (imagen_fondo)
    {
        al_draw_scaled_bitmap(imagen_fondo, 0, 0, al_get_bitmap_width(imagen_fondo), al_get_bitmap_height(imagen_fondo), 0, 0, 800, 600, 0);
    }
    else
    {
        // Fondo negro por defecto si no hay imagen
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    // Dibujar la nave
    al_draw_scaled_rotated_bitmap(nave.imagen, cx, cy, nave.x + nave.ancho / 2, nave.y + nave.largo / 2, escala_x, escala_y, nave.angulo, 0);

    if (asteroides_activados(nivel_actual))
    {
        for (i = 0; i < num_asteroides; i++)
        {
            al_draw_scaled_bitmap(asteroides[i].imagen, 0, 0, al_get_bitmap_width(asteroides[i].imagen), al_get_bitmap_height(asteroides[i].imagen), asteroides[i].x, asteroides[i].y, asteroides[i].ancho, asteroides[i].alto, 0);
        }
    }
}

/**
 * @brief Actualiza la posición y el ángulo de la nave según las teclas presionadas.
 * 
 * La nave rota con las teclas izquierda/derecha y avanza o retrocede en la dirección de su ángulo
 * cuando se presionan las teclas de arriba o abajo, respectivamente. El movimiento se ajusta para
 * que coincida con la orientación visual de la nave (punta hacia arriba). También gestiona la colisión
 * circular con los asteroides y aplica invulnerabilidad temporal tras recibir daño.
 *
 * @param nave Puntero a la nave que se va a mover.
 * @param teclas Arreglo de teclas presionadas.
 * @param asteroides Arreglo de asteroides.
 * @param tiempo_actual Tiempo actual en segundos.
 */
void actualizar_nave(Nave* nave, bool teclas[], Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS])
{
    float nueva_x = nave->x;
    float nueva_y = nave->y;

    if(nave->tipo == 0)
    {
        // Movilidad tipo Space Invaders: solo izquierda/derecha
        if (teclas[2])
        {
            nueva_x -= 5; // Izquierda
            printf("Izquierda\n");
        } 

        if (teclas[3])
        {
            nueva_x += 5; // Derecha
            printf("Derecha\n");
        }

        // Limitar el movimiento de la nave dentro de la ventana
        if (nueva_x < 0) nueva_x = 0;
        if (nueva_x > 800 - nave->ancho) nueva_x = 800 - nave->ancho;

        if (!verificar_colision_nave_muro(nueva_x, nave->y, nave->ancho, nave->largo, tilemap))
        {
            nave->x = nueva_x;
        }
        
        // No permitir movimiento vertical ni rotación
        nave->angulo = 0.0f;
    }
    else
    {
        // Rotación de la nave
        if(teclas[2]) 
        {
            nave->angulo -= 0.07f; // Izquierda
            printf("Rotar a la izquierda\n");
        }
        if(teclas[3]) 
        {
            nave->angulo += 0.07f; // Derecha
            printf("Rotar a la derecha\n");
        }

        if (teclas[0]) // Arriba (avanzar)
        {
            nueva_x += cos(nave->angulo - ALLEGRO_PI/2) * 5;
            nueva_y += sin(nave->angulo - ALLEGRO_PI/2) * 5;
            printf("Avanzando\n");
        }

        // Limitar el movimiento de la nave dentro de la ventana
        if (nueva_x < 0) nueva_x = 0;
        if (nueva_x > 800 - nave->ancho) nueva_x = 800 - nave->ancho;
        if (nueva_y < 0) nueva_y = 0;
        if (nueva_y > 600 - nave->largo) nueva_y = 600 - nave->largo;

        if (!verificar_colision_nave_muro(nueva_x, nueva_y, nave->ancho, nave->largo, tilemap))
        {
            nave->x = nueva_x;
            nave->y = nueva_y;
        }
    }
}


/**
 * @brief Dibuja la barra de vida de la nave.
 * 
 * @param nave Nave a la que se le va a dibujar la barra de vida.
 */
void dibujar_barra_vida(Nave nave, ALLEGRO_FONT *fuente)
{
    float vida_maxima = 100.0f; // Vida máxima de la nave
    float porcentaje_vida = nave.vida / vida_maxima;
    ALLEGRO_COLOR color_vida;
    char texto_vida[50];
    
    // Asegurar que el porcentaje esté entre 0 y 1
    if (porcentaje_vida < 0.0f) porcentaje_vida = 0.0f;
    if (porcentaje_vida > 1.0f) porcentaje_vida = 1.0f;
    
    // Color de la barra según la vida restante
    if (porcentaje_vida > 0.6f)
    {
        color_vida = al_map_rgb(0, 255, 0);      // Verde
    }
    else if (porcentaje_vida > 0.3f)
    {
        color_vida = al_map_rgb(255, 255, 0);    // Amarillo
    }
    else
    {
        color_vida = al_map_rgb(255, 0, 0);      // Rojo
    }
    
    // Dibujar barra de vida
    al_draw_filled_rectangle(10, 10, 10 + (200 * porcentaje_vida), 30, color_vida);
    al_draw_rectangle(10, 10, 210, 30, al_map_rgb(255, 255, 255), 2);
    
    // Mostrar vida actual como texto
    sprintf(texto_vida, "Vida: %.1f/%.1f", nave.vida, vida_maxima);
    
    al_draw_text(fuente, al_map_rgb(255, 255, 255), 220, 15, ALLEGRO_ALIGN_LEFT, texto_vida);
}

/**
 * @brief 
 * 
 * @param disparos Arreglo de disparos
 * @param num_disparos Cantidad de disparos definida
 */
void init_disparos(Disparo disparos[], int num_disparos)
{
    int i;
    for (i = 0; i < num_disparos; i++)
    {
        disparos[i].activo = false;
    }
}

/**
 * @brief Actualiza la posición de todos los disparos activos.
 * 
 * Mueve cada disparo activo en la dirección de su ángulo y desactiva los disparos que salen de la pantalla.
 * 
 * @param disparos Arreglo de disparos.
 * @param num_disparos Número total de disparos.
 */
void actualizar_disparos(Disparo disparos[], int num_disparos)
{
    int i;

    for (i = 0; i < num_disparos; i++)
    {
        if (disparos[i].activo)
        {
            disparos[i].x += cos(disparos[i].angulo) * disparos[i].velocidad;
            disparos[i].y += sin(disparos[i].angulo) * disparos[i].velocidad;
            if (disparos[i].x < 0 || disparos[i].x > 800 || disparos[i].y < 0 || disparos[i].y > 600)
            {
                disparos[i].activo = false;
            }
        }
    }
}

/**
 * @brief Dibuja todos los disparos activos en pantalla.
 * 
 * @param disparos Arreglo de disparos.
 * @param num_disparos Número total de disparos.
 */
void dibujar_disparos(Disparo disparos[], int num_disparos)
{
    int i;

    for (i = 0; i < num_disparos; i++)
    {
        if (disparos[i].activo)
        {
            al_draw_filled_rectangle(disparos[i].x, disparos[i].y, disparos[i].x + 5, disparos[i].y + 10, al_map_rgb(255, 0, 0));
        }
    }
}

/**
 * @brief Permite que la nave dispare proyectiles desde su punta en la dirección actual.
 *
 * Busca un disparo inactivo en el arreglo y lo activa, posicionándolo en la punta de la nave
 * y asignándole el ángulo actual de la nave. El disparo se moverá en la dirección en la que apunta la nave.
 *
 * @param disparos Arreglo de disparos
 * @param num_disparos Cantidad de disparos definida
 * @param nave Nave que ejecuta los disparos
 */
void disparar(Disparo disparos[], int num_disparos, Nave nave)
{
    int i;
    float centro_x;
    float centro_y;
    float punta_x;
    float punta_y;

    for (i = 0; i < num_disparos; i++)
    {
        if (!disparos[i].activo)
        {
            // Calcula el centro de la nave
            centro_x = nave.x + nave.ancho / 2.0f;
            centro_y = nave.y + nave.largo / 2.0f;
            // Calcula la punta de la nave desde el centro, usando el ángulo y la mitad del largo
            punta_x = centro_x + cos(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);
            punta_y = centro_y + sin(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);

            disparos[i].x = punta_x;
            disparos[i].y = punta_y;
            disparos[i].velocidad = 10;
            disparos[i].angulo = nave.angulo - ALLEGRO_PI/2; // Asignar el ángulo de la nave al disparo
            disparos[i].activo = true;
            break;
        }
    }
}

/**
 * @brief Detecta colisiones entre un asteroide y un disparo.
 * 
 * @param asteroide Asteroide a verificar.
 * @param disparo Disparo a verificar.
 * 
 */
bool detectar_colision_disparo(Asteroide asteroide, Disparo disparo)
{
    return disparo.x < asteroide.x + asteroide.ancho && disparo.x + 5 > asteroide.x && disparo.y < asteroide.y + asteroide.alto && disparo.y + 10 > asteroide.y;
}

/**
 * @brief Actualiza la posición de la nave, los asteroides y los disparos.
 * 
 * @param nave Puntero a la nave que se va a mover
 * @param teclas Arreglo de teclas presionadas
 * @param asteroides Arreglo de asteroides
 * @param num_asteroides Número de asteroides en el arreglo
 * @param disparos Arreglo de disparos
 * @param num_disparos Número de disparos en el arreglo
 * @param puntaje Puntero al puntaje del jugador
 * 
 */
void actualizar_juego(Nave *nave, bool teclas[], Asteroide asteroides[], int num_asteroides, Disparo disparos[], int num_disparos, int* puntaje, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos[], int num_enemigos, Disparo disparos_enemigos[], int num_disparos_enemigos, ColaMensajes *cola_mensajes, EstadoJuego *estado_nivel, double tiempo_actual, Powerup powerups[], int max_powerups)
{
    int i;
    int j;
    int fila;
    int col;

    float pos_enemigo_x;
    float pos_enemigo_y;
    int tipo_enemigo;
    int dano;
    bool disparo_procesado;
    float tile_x;
    float tile_y;


    int probabilidad_powerup;

    if (estado_nivel->mostrar_transicion)
    {
        actualizar_estado_nivel_sin_jefe(estado_nivel, enemigos, num_enemigos, tiempo_actual);
        return;
    }

    actualizar_nave(nave, teclas, tilemap);
    actualizar_disparos(disparos, num_disparos);
    actualizar_enemigos(enemigos, num_enemigos, disparos_enemigos, num_disparos_enemigos, tiempo_actual, *nave);
    actualizar_disparos_enemigos(disparos_enemigos, num_disparos_enemigos);

    actualizar_escudo(&nave->escudo, tiempo_actual);
    actualizar_powerups(powerups, max_powerups, tiempo_actual);

    for (i = 0; i < max_powerups; i++)
    {
        if (powerups[i].activo && detectar_colision_powerup(*nave, powerups[i]))
        {
            recoger_powerup(nave, &powerups[i], cola_mensajes);
        }
    }

    for (i = 0; i < num_disparos; i++)
    {
        if (!disparos[i].activo) continue;
    
        for (j = 0; j < num_enemigos; j++)
        {
            if (!enemigos[j].activo) continue;
        
            if (detectar_colision_disparo_enemigo(disparos[i], enemigos[j]))
            {
                enemigos[j].vida -= 10;
                disparos[i].activo = false;
            
                if (enemigos[j].vida <= 0)
                {
                    enemigos[j].activo = false;
                    (*puntaje) += 10;
                
                    // AGREGAR PROGRESO DEL ARMA NORMAL
                    actualizar_progreso_arma(nave, Arma_normal);
                    verificar_mejora_arma(nave, Arma_normal, cola_mensajes);
                
                    // Crear powerup aleatorio
                    probabilidad_powerup = rand() % 100;
                    if (probabilidad_powerup < POWERUP_PROB)
                    {
                        crear_powerup_aleatorio(powerups, max_powerups, enemigos[j].x, enemigos[j].y);
                    }
                }
                break;
            }
        }
    }

    if (asteroides_activados(estado_nivel->nivel_actual))
    {
        for (i = 0; i < num_asteroides; i++)
        {
            actualizar_asteroide(&asteroides[i], tilemap, nave, powerups, max_powerups);
            
            for (j = 0; j < num_disparos; j++)
            {
                if (disparos[j].activo && detectar_colision_disparo(asteroides[i], disparos[j]))
                {
                    disparos[j].activo = false;
                    asteroides[i].y = -asteroides[i].alto;
                    asteroides[i].x = rand() % (800 - (int)asteroides[i].ancho);
                    (*puntaje)++;
                    nave->kills_para_mejora++;
                    verificar_mejora_disparo_radial(nave, cola_mensajes);
                }
            }
        }
    }
    

    // Colisiones con enemigos
    for (i = 0; i < num_enemigos; i++)
    {
        if (!enemigos[i].activo) continue;
        
        // Disparos del jugador vs enemigos (USA LA NUEVA FUNCIÓN)
        for (j = 0; j < num_disparos; j++)
        {
            if (disparos[j].activo && detectar_colision_disparo_enemigo(disparos[j], enemigos[i]))
            {
                disparos[j].activo = false;
                enemigos[i].vida--;

                if (enemigos[i].vida <= 0)
                {
                    pos_enemigo_x = enemigos[i].x;
                    pos_enemigo_y = enemigos[i].y;
                    tipo_enemigo = enemigos[i].tipo;

                    enemigos[i].activo = false;
                    (*puntaje) += 10; // Incrementa el puntaje por destruir un enemigo
                    nave->kills_para_mejora++;
                    verificar_mejora_disparo_radial(nave, cola_mensajes);

                    if (rand() % 100 < POWERUP_PROB)
                    {
                        crear_powerup_aleatorio(powerups, max_powerups, pos_enemigo_x, pos_enemigo_y);
                        printf("Powerup aleatorio generado tras eliminar enemigo tipo %d en (%.0f, %.0f)\n", tipo_enemigo, pos_enemigo_x, pos_enemigo_y);
                    }
                    else
                    {
                        printf("No se generó powerup (probabilidad %d%%)\n", POWERUP_PROB);
                    }
                }
                else
                {
                    (*puntaje) += 2;
                }
                break;
            }
        }

        // Nave vs enemigos (USA LA NUEVA FUNCIÓN)
        if (detectar_colision_nave_enemigo(*nave, enemigos[i]))
        {
            if (escudo_recibir_dano(&nave->escudo))
            {
                printf("El escudo absorbió el daño del enemigo\n");
            }
            else
            {
                nave->vida -= 10.0f;
                printf("La nave recibió 20 de daño directo, Vida restante: %.1f\n", nave->vida);
            }
            enemigos[i].activo = false;
        }
    }

    actualizar_estado_nivel_sin_jefe(estado_nivel, enemigos, num_enemigos, tiempo_actual);

    // Disparos de enemigos vs nave (USA LA NUEVA FUNCIÓN)
    for (i = 0; i < num_disparos_enemigos; i++)
    {
        if (disparos_enemigos[i].activo && detectar_colision_disparo_enemigo_nave(*nave, disparos_enemigos[i]))
        {
            disparos_enemigos[i].activo = false;
            
           if (!escudo_recibir_dano(&nave->escudo))
           {
                dano = 15;
                if (disparos_enemigos[i].velocidad <= 2.5f)
                {
                    dano = 25; // Tanques hacen más daño
                }
                else if (disparos_enemigos[i].velocidad >= 4.0f)
                {
                    dano = 20; // Francotiradores hacen daño medio-alto
                }
                
                nave->vida -= dano;
                printf("Nave recibio %d de daño por disparo enemigo. vida restante: %.1f\n", dano, nave->vida);
           }
           else
           {
                printf("El escudo absorbio el dano del disparo enemigo\n");
           }
        }
    }

    for (i = 0; i < num_disparos_enemigos; i++)
    {
        if (disparos_enemigos[i].activo)
        {
            disparo_procesado = false;
            for (fila = 0; fila < MAPA_FILAS && !disparo_procesado; fila++)
            {
                for (col = 0; col < MAPA_COLUMNAS && !disparo_procesado; col++)
                {
                    if (tilemap[fila][col].tipo == 2 && tilemap[fila][col].vida > 0)
                    {
                        tile_x = col * TILE_ANCHO;
                        tile_y = fila * TILE_ALTO;

                        if (detectar_colision_disparo_enemigo_escudo(disparos_enemigos[i], tile_x, tile_y))
                        {
                            disparos_enemigos[i].activo = false;
                            tilemap[fila][col].vida--;

                            printf("Disparo enemigo impactó escudo en (%d, %d)! Vida restante: %d\n", col, fila, tilemap[fila][col].vida);

                            if (tilemap[fila][col].vida <= 0)
                            {
                                tilemap[fila][col].tipo = 0; // El escudo se destruye
                                printf("Escudo en (%d, %d) destruido completamente!\n", col, fila);
                            }

                            disparo_procesado = true;
                        }
                    }
                    else if (tilemap[fila][col].tipo == 3)
                    {
                        tile_x = col * TILE_ANCHO;
                        tile_y = fila * TILE_ALTO;

                        if (detectar_colision_disparo_enemigo_escudo(disparos_enemigos[i], tile_x, tile_y))
                        {
                            disparos_enemigos[i].activo = false;
                            printf("Disparo enemigo rebotó en muro indestructible en (%d, %d)!\n", col, fila);
                            disparo_procesado = true;
                        }
                    }
                    else if (tilemap[fila][col].tipo == 1)
                    {
                        tile_x = col * TILE_ANCHO;
                        tile_y = fila * TILE_ALTO;

                        if (detectar_colision_disparo_enemigo_escudo(disparos_enemigos[i], tile_x, tile_y))
                        {
                            disparos_enemigos[i].activo = false;
                            printf("Disparo enemigo impactó asteroide fijo en (%d, %d)!\n", col, fila);
                            disparo_procesado = true;
                        }
                    }
                }
            }
        }
    }
}


/**
 * @brief Permite dibujar el puntaje que hace el jugador en la pantalla.
 * 
 * @param puntaje Puntaje que hace el jugador
 */
void dibujar_puntaje(int puntaje, ALLEGRO_FONT* fuente)
{
    char text_puntaje[20];
    sprintf(text_puntaje, "Puntaje: %d", puntaje);
    al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 40, ALLEGRO_ALIGN_LEFT, text_puntaje);
}

/**
 * @brief Inicializa los botones del menú principal.
 * 
 * @param botones Arreglo de botones del menú principal.
 */
void init_botones(Boton botones[])
{
    strcpy(botones[0].texto, "Jugar");
    botones[0].x = 300;
    botones[0].y = 200;
    botones[0].ancho = 200;
    botones[0].alto = 50;

    strcpy(botones[1].texto, "Ranking");
    botones[1].x = 300;
    botones[1].y = 300;
    botones[1].ancho = 200;
    botones[1].alto = 50;

    strcpy(botones[2].texto, "Salir");
    botones[2].x = 300;
    botones[2].y = 400;
    botones[2].ancho = 200;
    botones[2].alto = 50;
}

/**
 * @brief Dibuja los botones del menú principal.
 * 
 * @param botones Arreglo de botones.
 * @param num_botones Número de botones.
 * @param fuente Fuente usada para el texto.
 * @param cursor_x Posición x del cursor.
 * @param cursor_y Posición y del cursor.
 */
void dibujar_botones(Boton botones[], int num_botones, ALLEGRO_FONT* fuente, int cursor_x, int cursor_y)
{
    int i;
    ALLEGRO_COLOR color_borde;
    ALLEGRO_COLOR color_texto;
    float grosor_borde = 2.0f;

    for (i = 0; i < num_botones; i++)
    {
        if (cursor_sobre_boton(botones[i], cursor_x, cursor_y))
        {
            color_borde = al_map_rgb(255, 255, 255);
            color_texto = al_map_rgb(0, 255, 0);
            grosor_borde = 3.0f;
        }
        else
        {
            color_borde = al_map_rgb(150, 150, 150);
            color_texto = al_map_rgb(43, 158, 0);
        }

        al_draw_rectangle(botones[i].x, botones[i].y, botones[i].x + botones[i].ancho, botones[i].y + botones[i].alto, color_borde, grosor_borde);

        al_draw_text(fuente, color_texto, botones[i].x + botones[i].ancho / 2, botones[i].y + botones[i].alto / 2 - al_get_font_line_height(fuente) / 2, ALLEGRO_ALIGN_CENTER, botones[i].texto);
    }   
}

/**
 * @brief Detecta si el mouse hizo clic sobre algún botón.
 * 
 * @param botones Arreglo de botones.
 * @param num_botones Número de botones.
 * @param x Posición x del mouse.
 * @param y Posición y del mouse.
 * @return int Índice del botón clickeado, o -1 si no se clickeó ningún botón.
 */
int detectar_click(Boton botones[], int num_botones, int x, int y)
{
    int i;

    for (i = 0; i < num_botones; i++)
    {
        if (x >= botones[i].x && x <= botones[i].x + botones[i].ancho && y >= botones[i].y && y <= botones[i].y + botones[i].alto)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Guarda el puntaje obtenido en el juego a un archivo de texto.
 * 
 * @param nombre Nombre del jugador
 * @param puntaje Puntaje del jugador respectivo
 */
void guardar_puntaje(const char* nombre, int puntaje)
{
    Jugador ranking[MAX_JUGADORES];
    int num_jugadores = 0;
    int i;

    FILE *archivo = fopen("ranking.txt", "r");

    if (archivo)
    {
        while (num_jugadores < MAX_JUGADORES && fscanf(archivo, "%39s %d", ranking[num_jugadores].nombre, &ranking[num_jugadores].puntaje) == 2)
        {
            num_jugadores++;
        }
        fclose(archivo);
    }
    
    if (num_jugadores < MAX_JUGADORES)
    {
        strcpy(ranking[num_jugadores].nombre, nombre);
        ranking[num_jugadores].puntaje = puntaje;
        num_jugadores++;
    }
    else
    {
        qsort(ranking, num_jugadores, sizeof(Jugador), comparar_puntajes);
        if (puntaje > ranking[num_jugadores-1].puntaje)
        {
            strcpy(ranking[num_jugadores-1].nombre, nombre);
            ranking[num_jugadores-1].puntaje = puntaje;
        }
    }

    qsort(ranking, num_jugadores, sizeof(Jugador), comparar_puntajes);

    FILE *archivo_escritura = fopen("ranking.txt", "w");

    if (archivo_escritura)
    {
        for (i = 0; i < num_jugadores; i++)
        {
            fprintf(archivo_escritura, "%s %d\n", ranking[i].nombre, ranking[i].puntaje);
        }
        fclose(archivo_escritura);
        printf("Puntaje guardado: %s - %d puntos\n", nombre, puntaje);
    }
    else
    {
        printf("Error: No se pudo guardar el puntaje.\n");
    }
}

/**
 * @brief Carga el archivo en donde se guardan los puntajes de los jugadores.
 * 
 * @param ranking Jugador dentro del ranking
 * @param num_jugadores Puntero al numero de jugadores
 */
void cargar_ranking(Jugador ranking[], int* num_jugadores)
{
    FILE* archivo = fopen("ranking.txt", "r");
    *num_jugadores = 0;
    int i;

    if (archivo)
    {
        while (*num_jugadores < MAX_JUGADORES && fscanf(archivo, "%39s %d", ranking[*num_jugadores].nombre, &ranking[*num_jugadores].puntaje) == 2)
        {
            (*num_jugadores)++;
        }
        fclose(archivo);

        // Ordenar el ranking de mayor a menor puntaje
        qsort(ranking, *num_jugadores, sizeof(Jugador), comparar_puntajes);
        printf("Ranking cargado: %d jugadores\n", *num_jugadores);
        
        for (i = 0; i < *num_jugadores && i < 5; i++)
        {
            printf("%d. %s - %d puntos\n", i+1, ranking[i].nombre, ranking[i].puntaje);
        }
    }
    else
    {
        fprintf(stderr, "Error: no se pudo abrir el archivo de ranking.\n");
    }
}

/**
 * @brief Muestra el ranking de los jugadores en la pantalla.
 * 
 * @param fuente Fuente de letra usada en el ranking
 * @param ranking Jugadores dentro del ranking
 * @param num_jugadores Numero de jugadores dentro del ranking
 * @param volver_menu Puntero a la variable que indica si se debe volver al menu principal
 */
void mostrar_ranking(ALLEGRO_FONT* fuente, Jugador ranking[], int num_jugadores, bool* volver_menu)
{
    int i;
    char texto_jugador[100];
    bool mostrar = true;
    ALLEGRO_EVENT evento;
    ALLEGRO_EVENT_QUEUE* cola_eventos = al_create_event_queue();
    ALLEGRO_COLOR color_texto;
    al_register_event_source(cola_eventos, al_get_mouse_event_source());

    Boton boton_volver;
    strcpy(boton_volver.texto, "Volver");

    boton_volver.x = 50;
    boton_volver.y = 500;
    boton_volver.ancho = 120;
    boton_volver.alto = 50;

    int cursor_x = 0;
    int cursor_y = 0;

    while (mostrar)
    {
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            mostrar = false;
            *volver_menu = true;
        }
        
        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            cursor_x = evento.mouse.x;
            cursor_y = evento.mouse.y;
        }
        
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (cursor_sobre_boton(boton_volver, evento.mouse.x, evento.mouse.y))
            {
                mostrar = false;
                *volver_menu = true;
            }
        }
        
        if (evento.type == ALLEGRO_EVENT_TIMER || mostrar)
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 50, ALLEGRO_ALIGN_CENTER, "RANKING DE JUGADORES");

            for (i = 0; i < num_jugadores && i < 10; i++)
            {
                sprintf(texto_jugador, "%d. %s - %d puntos", i + 1, ranking[i].nombre, ranking[i].puntaje);

                if (i == 0)
                {
                    color_texto = al_map_rgb(255, 215, 0);
                }
                else if (i == 1)
                {
                    color_texto = al_map_rgb(192, 192, 192);
                }
                else if (i == 2)
                {
                    color_texto = al_map_rgb(205, 127, 50);
                }
                else
                {
                    color_texto = al_map_rgb(255, 255, 255);
                }

                al_draw_text(fuente, color_texto, 400, 120 + i *30, ALLEGRO_ALIGN_CENTER, texto_jugador);
            }
            
            if (num_jugadores == 0)
            {
                al_draw_text(fuente, al_map_rgb(150, 150, 150), 400, 300, ALLEGRO_ALIGN_CENTER, "No hay puntuaciones registradas");
            }
            
            dibujar_boton_individual(boton_volver, fuente, cursor_x, cursor_y);
            al_flip_display();
        }
    }

    al_destroy_event_queue(cola_eventos);
}

/**
 * @brief Permite obtener el nombre del jugador una vez que este pierde.
 * 
 * @param fuente Fuente de letra a usar.
 * @param nombre Puntero al nombre del jugador
 */
void capturar_nombre(ALLEGRO_FONT* fuente, char* nombre, ALLEGRO_EVENT_QUEUE* cola_eventos)
{
    int pos = 0;
    nombre[0] = '\0';
    bool terminado = false;
    bool necesita_redibujar = true;
    ALLEGRO_EVENT evento;

    while (!terminado)
    {
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if ((evento.keyboard.unichar >= 'a' && evento.keyboard.unichar <= 'z') || (evento.keyboard.unichar >= 'A' && evento.keyboard.unichar <= 'Z') || (evento.keyboard.unichar >= '0' && evento.keyboard.unichar <= '9') || evento.keyboard.unichar == '-' || evento.keyboard.unichar == '_')
            {
                if (pos < MAX_NOMBRE - 1)
                {
                    nombre[pos] = evento.keyboard.unichar;
                    pos++;
                    nombre[pos] = '\0';
                    necesita_redibujar = true;
                }   
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && pos > 0)
            {
                pos--;
                nombre[pos] = '\0';
                necesita_redibujar = true;
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                terminado = true;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            nombre[0] = '\0';
            terminado = true;
        }

        if (necesita_redibujar)
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, "Ingrese su nombre:");
            al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 350, ALLEGRO_ALIGN_CENTER, nombre);
            al_flip_display();
            necesita_redibujar = false;
        }
    }
}

/**
 * @brief Compara los puntajes de dos jugadores.
 *
 * Esta función se utiliza para ordenar un arreglo de estructuras Jugador en orden descendente de puntaje.
 *
 * @param a Puntero al primer jugador.
 * @param b Puntero al segundo jugador.
 * @return int Un valor negativo si el puntaje del primer jugador es menor que el del segundo,
 *             cero si los puntajes son iguales,
 *             un valor positivo si el puntaje del primer jugador es mayor que el del segundo.
 */
int comparar_puntajes(const void* a, const void* b)
{
    Jugador* jugadorA = (Jugador*)a;
    Jugador* jugadorB = (Jugador*)b;

    return jugadorB->puntaje - jugadorA->puntaje;
}


/**
 * @brief Determina si el cursor está sobre un botón.
 * 
 * @param boton Botón a verificar.
 * @param x Posición x del cursor.
 * @param y Posición y del cursor.
 * @return true si el cursor está sobre el botón, false en caso contrario.
 */
bool cursor_sobre_boton(Boton boton, int x, int y)
{
    return x >= boton.x && x <= boton.x + boton.ancho && y >= boton.y && y <= boton.y + boton.alto;
}


/**
 * @brief Detecta colisión circular entre dos objetos.
 * 
 * Calcula si dos círculos definidos por sus centros y radios se intersectan.
 * 
 * @param x1 Centro x del primer objeto.
 * @param y1 Centro y del primer objeto.
 * @param r1 Radio del primer objeto.
 * @param x2 Centro x del segundo objeto.
 * @param y2 Centro y del segundo objeto.
 * @param r2 Radio del segundo objeto.
 * @return true si hay colisión, false en caso contrario.
 */
bool detectar_colision_circular(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    float distancia = sqrt(dx * dx + dy * dy);
    return distancia < (r1 + r2);
}


/**
 * @brief Detecta colisión entre dos objetos rectangulares usando colisión circular.
 * 
 * @param x1 Posición x del primer objeto.
 * @param y1 Posición y del primer objeto.
 * @param ancho1 Ancho del primer objeto.
 * @param alto1 Alto del primer objeto.
 * @param x2 Posición x del segundo objeto.
 * @param y2 Posición y del segundo objeto.
 * @param ancho2 Ancho del segundo objeto.
 * @param alto2 Alto del segundo objeto.
 * @return true si hay colisión.
 */
bool detectar_colision_generica(float x1, float y1, float ancho1, float alto1, float x2, float y2, float ancho2, float alto2)
{
    float centro_x1 = x1 + ancho1 / 2;
    float centro_y1 = y1 + alto1 / 2;
    float radio1 = ancho1 / 2.0f;

    float centro_x2 = x2 + ancho2 / 2;
    float centro_y2 = y2 + alto2 / 2;
    float radio2 = ancho2 / 2.0f;

    return detectar_colision_circular(centro_x1, centro_y1, radio1, centro_x2, centro_y2, radio2);
}


/**
 * @brief Carga un tilemap desde un archivo de texto y extrae enemigos.
 * 
 * Lee un archivo que contiene números que representan diferentes tipos de tiles:
 * - 0: vacío
 * - 1: asteroide fijo
 * - 2: escudo destructible
 * - 3: enemigo normal
 * - 4: enemigo perseguidor
 * 
 * Los enemigos (3 y 4) se extraen del tilemap y se almacenan en el arreglo de enemigos.
 * 
 * @param filename Nombre del archivo del tilemap.
 * @param tilemap Matriz de tiles a llenar.
 * @param enemigos Arreglo donde se almacenarán los enemigos extraídos.
 * @param num_enemigos Puntero al contador de enemigos cargados.
 * @param imagen_enemigo Imagen que se asignará a todos los enemigos.
 */
void cargar_tilemap(const char* filename, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos[], int* num_enemigos, ALLEGRO_BITMAP* imagen_enemigo, float *nave_x, float *nave_y) 
{
    bool nave_encontrada;
    char linea[50];
    int longitud_linea;
    char c;

    int fila;
    int col;


    FILE* archivo = fopen(filename, "r");
    if (!archivo) 
    {
        fprintf(stderr, "ERROR: No se pudo abrir %s\n", filename);
        *num_enemigos = 0;
        *nave_x = 400;
        *nave_y = 500;
        return;
    }

    *num_enemigos = 0;
    nave_encontrada = false;

    printf("=== CARGANDO: %s ===\n", filename);

    for (fila = 0; fila < MAPA_FILAS; fila++)
    {
        // Leer línea completa
        if (fgets(linea, sizeof(linea), archivo) == NULL)
        {
            printf("ERROR: No se pudo leer la fila %d\n", fila);
            break;
        }

        printf("Fila %d: %.10s... (primeros 10 chars)\n", fila, linea);

        // Procesar cada columna de la línea
        longitud_linea = (int)strlen(linea);
        for (col = 0; col < MAPA_COLUMNAS && col < longitud_linea; col++)
        {
            c = linea[col];

            // Inicializar tile por defecto
            tilemap[fila][col].tipo = 0;
            tilemap[fila][col].vida = 0;

            switch (c) 
            {
                case '1':
                    tilemap[fila][col].tipo = 1;
                    break;

                case '2':
                    tilemap[fila][col].tipo = 2;
                    tilemap[fila][col].vida = 3;
                    break;

                case '3':
                    tilemap[fila][col].tipo = 3;
                    tilemap[fila][col].vida = 999;
                    break;

                case 'P':
                    *nave_x = col * TILE_ANCHO + (TILE_ANCHO - 50)/2;
                    *nave_y = fila * TILE_ALTO + (TILE_ALTO - 50)/2;
                    nave_encontrada = true;
                    printf("*** NAVE ENCONTRADA: fila=%d, col=%d -> (%.0f, %.0f) ***\n", fila, col, *nave_x, *nave_y);
                    break;

                case 'E':
                    if (*num_enemigos < NUM_ENEMIGOS) 
                    {
                        init_enemigo_tipo(&enemigos[*num_enemigos], col, fila, 0, imagen_enemigo);
                        printf("E: enemigo normal en (%d,%d)\n", col, fila);
                        (*num_enemigos)++;
                    }
                    break;

                case 'H':
                    if (*num_enemigos < NUM_ENEMIGOS) 
                    {
                        init_enemigo_tipo(&enemigos[*num_enemigos], col, fila, 1, imagen_enemigo);
                        printf("H: perseguidor en (%d,%d)\n", col, fila);
                        (*num_enemigos)++;
                    }
                    break;

                case 'S':
                    if (*num_enemigos < NUM_ENEMIGOS)
                    {
                        init_enemigo_tipo(&enemigos[*num_enemigos], col, fila, 2, imagen_enemigo);
                        printf("S: francotirador en (%d,%d)\n", col, fila);
                        (*num_enemigos)++;
                    }
                    break;

                case 'T':
                    if (*num_enemigos < NUM_ENEMIGOS)
                    {
                        init_enemigo_tipo(&enemigos[*num_enemigos], col, fila, 3, imagen_enemigo);
                        printf("T: tanque en (%d,%d)\n", col, fila);
                        (*num_enemigos)++;
                    }
                    break;

                case 'K':
                    if (*num_enemigos < NUM_ENEMIGOS)
                    {
                        init_enemigo_tipo(&enemigos[*num_enemigos], col, fila, 4, imagen_enemigo);
                        printf("K: kamikaze en (%d,%d)\n", col, fila);
                        (*num_enemigos)++;
                    }
                    break;

                case 'B':
                    if (*num_enemigos < NUM_ENEMIGOS)
                    {
                        init_enemigo_tipo(&enemigos[*num_enemigos], col, fila, 5, imagen_enemigo); // Tipo 5 = Jefe Destructor
                        printf("B: Jefe Destructor en (%d,%d)\n", col, fila);
                        (*num_enemigos)++;
                    }
                    tilemap[fila][col].tipo = 0;
                    tilemap[fila][col].vida = 0;
                    break;

                case 'C':
                    if (*num_enemigos < NUM_ENEMIGOS)
                    {
                        init_enemigo_tipo(&enemigos[*num_enemigos], col, fila, 6, imagen_enemigo); // Tipo 6 = Jefe Supremo
                        printf("C: Jefe Supremo en (%d,%d)\n", col, fila);
                        (*num_enemigos)++;
                    }
                    tilemap[fila][col].tipo = 0;
                    tilemap[fila][col].vida = 0;
                    break;

                case '0':
                default:
                    break;
            }
        }
    }

    if (!nave_encontrada) {
        *nave_x = 400;
        *nave_y = 500;
        printf("ADVERTENCIA: Nave no encontrada, usando (400, 500)\n");
    }

    printf("=== RESULTADO: %d enemigos, nave en (%.0f, %.0f) ===\n", *num_enemigos, *nave_x, *nave_y);
    fclose(archivo);
}


/**
 * @brief Dibuja el tilemap en pantalla.
 * 
 * @param tilemap Matriz de tiles.
 * @param imagen_asteroide Imagen para los tiles de tipo asteroide.
 */
void dibujar_tilemap(Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], ALLEGRO_BITMAP* imagen_asteroide) 
{
    int fila;
    int col;
    ALLEGRO_COLOR color;

    for (fila = 0; fila < MAPA_FILAS; fila++)
    {
        for (col = 0; col < MAPA_COLUMNAS; col++)
        {
            if (tilemap[fila][col].tipo == 1)
            {
                al_draw_scaled_bitmap(imagen_asteroide, 0, 0, al_get_bitmap_width(imagen_asteroide), al_get_bitmap_height(imagen_asteroide), col * TILE_ANCHO, fila * TILE_ALTO, TILE_ANCHO, TILE_ALTO, 0);
            }
            else if (tilemap[fila][col].tipo == 2) 
            {
                // Dibuja el escudo como un rectángulo azul (puedes usar una imagen si prefieres)
                color = al_map_rgb(0, 128, 255);
                if (tilemap[fila][col].vida == 2) color = al_map_rgb(0, 200, 255);
                if (tilemap[fila][col].vida == 1) color = al_map_rgb(100, 100, 255);
                al_draw_filled_rectangle(col * TILE_ANCHO, fila * TILE_ALTO, (col + 1) * TILE_ANCHO, (fila + 1) * TILE_ALTO, color);
            }
            else if (tilemap[fila][col].tipo == 3)
            {
                al_draw_filled_rectangle(col * TILE_ANCHO, fila * TILE_ALTO, (col + 1) * TILE_ANCHO, (fila + 1) * TILE_ALTO, al_map_rgb(80, 80, 80));
            }
        }
    }
}



/**
 * @brief Inicializa los enemigos.
 * 
 * @param enemigos Arreglo de enemigos.
 * @param num_enemigos Número de enemigos.
 * @param imagen_enemigo Imagen del enemigo.
 */
void init_enemigos(Enemigo enemigos[], int num_enemigos, ALLEGRO_BITMAP* imagen_enemigo)
{
    int i;

    for (i = 0; i < num_enemigos; i++)
    {
        enemigos[i].x = rand() % (800 - 60);
        enemigos[i].y = rand() % 200 + 50; // Aparecen en la parte superior
        enemigos[i].ancho = 60;
        enemigos[i].alto = 40;
        enemigos[i].velocidad = 0.5f + (rand() % 100) / 100.0f; // Velocidad aleatoria
        enemigos[i].vida = 2;
        enemigos[i].activo = true;
        enemigos[i].ultimo_disparo = 0;
        enemigos[i].intervalo_disparo = 1.5 + (rand() % 200) / 100.0; // Dispara cada 1.5-3.5 segundos
        enemigos[i].imagen = imagen_enemigo;
    }
}


/**
 * @brief Actualiza la posición y comportamiento de todos los enemigos según su tipo.
 * 
 * Esta función maneja el comportamiento específico de cada tipo de enemigo:
 * - Tipo 0 (Normal): Movimiento horizontal rebotando en bordes
 * - Tipo 1 (Perseguidor): Sigue a la nave cuando está en rango
 * - Tipo 2 (Francotirador): Dispara con precisión hacia la nave
 * - Tipo 3 (Tanque): Movimiento lento, disparo en abanico
 * - Tipo 4 (Kamikaze): Se lanza directamente hacia la nave
 * 
 * @param enemigos Arreglo de enemigos a actualizar.
 * @param num_enemigos Número de enemigos en el arreglo.
 * @param disparos_enemigos Arreglo de disparos de enemigos para ataques.
 * @param num_disparos_enemigos Número máximo de disparos de enemigos.
 * @param tiempo_actual Tiempo actual del juego en segundos.
 * @param nave Nave del jugador (para persecución y cálculo de disparos).
 */
void actualizar_enemigos(Enemigo enemigos[], int num_enemigos, Disparo disparos_enemigos[], int num_disparos_enemigos, double tiempo_actual,Nave nave)
{
    int i;

    float dx;
    float dy;
    float distancia;
    float rango_vision;
    float norm;
    float velocidad_persecucion;

    for (i = 0; i < num_enemigos; i++)
    {
        if (!enemigos[i].activo) continue;

        switch (enemigos[i].tipo)
        {
            case 0: // Enemigo normal
                // Movimiento horizontal (de lado a lado)
                enemigos[i].x += enemigos[i].velocidad;
    
                if (enemigos[i].x <= 0 || enemigos[i].x >= 800 - enemigos[i].ancho)
                {
                    enemigos[i].velocidad *= -1;
                }
    
                if (tiempo_actual - enemigos[i].ultimo_disparo >= enemigos[i].intervalo_disparo)
                {
                    enemigo_disparar(disparos_enemigos, num_disparos_enemigos, enemigos[i]);
                    enemigos[i].ultimo_disparo = tiempo_actual;
                }
                break;

            case 1: // Enemigo perseguidor
                {
                    dx = nave.x + nave.ancho/2 - (enemigos[i].x + enemigos[i].ancho/2);
                    dy = nave.y + nave.largo/2 - (enemigos[i].y + enemigos[i].alto/2);
                    distancia = sqrt(dx*dx + dy*dy);
                    rango_vision = 250.0f;
                    
                    if (distancia < rango_vision)
                    {
                        norm = sqrt(dx*dx + dy*dy);
                        if (norm > 0.1f) 
                        {
                            velocidad_persecucion = enemigos[i].velocidad * 1.5f;
                            enemigos[i].x += (dx / norm) * velocidad_persecucion;
                            enemigos[i].y += (dy / norm) * velocidad_persecucion;
                        }

                        if (distancia < 150.0f && tiempo_actual - enemigos[i].ultimo_disparo >= enemigos[i].intervalo_disparo * 1.5f)
                        {
                            enemigo_disparar(disparos_enemigos, num_disparos_enemigos, enemigos[i]);
                            enemigos[i].ultimo_disparo = tiempo_actual;
                        }
                    }
                }
                break;

            case 2: // Francotirador
                if (tiempo_actual - enemigos[i].ultimo_disparo >= enemigos[i].intervalo_disparo)
                {
                    francotirador_disparar(disparos_enemigos, num_disparos_enemigos, enemigos[i], nave);
                    enemigos[i].ultimo_disparo = tiempo_actual;
                }
                break;

            case 3: // Tanque
                enemigos[i].x += enemigos[i].velocidad;
    
                if (enemigos[i].x <= 0 || enemigos[i].x >= 800 - enemigos[i].ancho)
                {
                    enemigos[i].velocidad *= -1;
                }
    
                if (tiempo_actual - enemigos[i].ultimo_disparo >= enemigos[i].intervalo_disparo)
                {
                    tanque_disparar(disparos_enemigos, num_disparos_enemigos, enemigos[i]);
                    enemigos[i].ultimo_disparo = tiempo_actual;
                }
                break;

            case 4: // Kamikaze
                {
                    dx = nave.x + nave.ancho/2 - (enemigos[i].x + enemigos[i].ancho/2);
                    dy = nave.y + nave.largo/2 - (enemigos[i].y + enemigos[i].alto/2);
                    distancia = sqrt(dx*dx + dy*dy);
                    
                    if (distancia > 10.0f)
                    {
                        float velocidad_kamikaze = enemigos[i].velocidad * 2.0f;
                        enemigos[i].x += (dx / distancia) * velocidad_kamikaze;
                        enemigos[i].y += (dy / distancia) * velocidad_kamikaze;
                    }
                    else
                    {
                        nave.vida -= 35;
                        enemigos[i].activo = false;
                        printf("Enemigo kamikaze impactó! Vida restante: %.1f\n", nave.vida);
                    }
                }
                break;
        }
    }
}


/**
 * @brief Dibuja todos los enemigos activos en pantalla con sus sprites específicos.
 * 
 * Renderiza cada enemigo usando su imagen correspondiente según su tipo,
 * aplicando escalado y rotación si es necesario.
 * 
 * @param enemigos Arreglo de enemigos a dibujar.
 * @param num_enemigos Número total de enemigos en el arreglo.
 */
void dibujar_enemigos(Enemigo enemigos[], int num_enemigos)
{
    int i;
    float porcentaje_vida;

    for (i = 0; i < num_enemigos; i++)
    {
        if (enemigos[i].activo)
        {
            // Dibujar enemigo base
            al_draw_scaled_bitmap(enemigos[i].imagen, 0, 0, al_get_bitmap_width(enemigos[i].imagen), al_get_bitmap_height(enemigos[i].imagen), enemigos[i].x, enemigos[i].y, enemigos[i].ancho, enemigos[i].alto, 0);
            
            // Mostrar barra de vida para tanques
            if (enemigos[i].tipo == 3)
            {
                porcentaje_vida = enemigos[i].vida / enemigos[i].vida_max;
                
                // Fondo de la barra
                al_draw_filled_rectangle(enemigos[i].x, enemigos[i].y - 8, enemigos[i].x + enemigos[i].ancho, enemigos[i].y - 4, al_map_rgba(100, 0, 0, 150));
                
                // Vida actual
                al_draw_filled_rectangle(enemigos[i].x, enemigos[i].y - 8, enemigos[i].x + (enemigos[i].ancho * porcentaje_vida), enemigos[i].y - 4, al_map_rgba(0, 255, 0, 200));
                
                // Borde
                al_draw_rectangle(enemigos[i].x, enemigos[i].y - 8, enemigos[i].x + enemigos[i].ancho, enemigos[i].y - 4, al_map_rgba(255, 255, 255, 180), 1);
            }
        }
    }
}


/**
 * @brief Actualiza la posición de todos los disparos de enemigos.
 * 
 * Mueve cada disparo activo según su ángulo y velocidad, y desactiva
 * los disparos que salen de los límites de la pantalla.
 * 
 * @param disparos Arreglo de disparos de enemigos.
 * @param num_disparos Número de disparos en el arreglo.
 */
void actualizar_disparos_enemigos(Disparo disparos[], int num_disparos)
{
    int i;

    for(i = 0; i < num_disparos; i++)
    {
        if(disparos[i].activo)
        {
            disparos[i].x += cos(disparos[i].angulo) * disparos[i].velocidad;
            disparos[i].y += sin(disparos[i].angulo) * disparos[i].velocidad;

            // Desactivar si sale de la pantalla
            if (disparos[i].y > 600 || disparos[i].x < 0 || disparos[i].x > 800)
            {
                disparos[i].activo = false;
            }
        }
    }
}


/**
 * @brief Dibuja todos los disparos activos de los enemigos en pantalla.
 * 
 * @param disparos Arreglo de disparos de enemigos.
 * @param num_disparos Número total de disparos de enemigos.
 */
void dibujar_disparos_enemigos(Disparo disparos[], int num_disparos)
{
    int i;

    for (i = 0; i < num_disparos; i++)
    {
        if (disparos[i].activo)
        {
            al_draw_filled_rectangle(disparos[i].x, disparos[i].y, disparos[i].x + 4, disparos[i].y + 8, al_map_rgb(255, 255, 0)); // Disparos amarillos
        }
    }
}


/**
 * @brief Hace que un enemigo dispare un proyectil.
 * 
 * Busca un disparo inactivo en el arreglo y lo activa, posicionándolo en la parte inferior del enemigo.
 * 
 * @param disparos Arreglo de disparos de enemigos.
 * @param num_disparos Número total de disparos de enemigos.
 * @param enemigo Enemigo que realiza el disparo.
 */
void enemigo_disparar(Disparo disparos[], int num_disparos, Enemigo enemigo)
{
    int i;

    for(i = 0; i < num_disparos; i++)
    {
        if (!disparos[i].activo)
        {
            disparos[i].x = enemigo.x + enemigo.ancho / 2;
            disparos[i].y = enemigo.y + enemigo.alto;
            disparos[i].velocidad = 3.0f; // Disparan hacia abajo
            disparos[i].angulo = ALLEGRO_PI / 2; // Disparan hacia abajo
            disparos[i].activo = true;
            break;
        }
    }
}


/**
 * @brief Detecta colisión entre disparo del jugador y enemigo.
 * 
 * @param disparo Disparo del jugador.
 * @param enemigo Enemigo a verificar.
 * @return true si hay colisión.
 */
bool detectar_colision_disparo_enemigo(Disparo disparo, Enemigo enemigo)
{
    return detectar_colision_generica(disparo.x, disparo.y, 5, 10, enemigo.x, enemigo.y, enemigo.ancho, enemigo.alto);
}


/**
 * @brief Detecta colisión entre nave del jugador y enemigo.
 * 
 * @param nave Nave del jugador.
 * @param enemigo Enemigo a verificar.
 * @return true si hay colisión.
 */
bool detectar_colision_nave_enemigo(Nave nave, Enemigo enemigo)
{
    return detectar_colision_generica(nave.x, nave.y, nave.ancho, nave.largo, enemigo.x, enemigo.y, enemigo.ancho, enemigo.alto);
}


/**
 * @brief Detecta colisión entre disparo de enemigo y nave.
 * 
 * @param nave Nave del jugador.
 * @param disparo Disparo del enemigo.
 * @return true si hay colisión.
 */
bool detectar_colision_disparo_enemigo_nave(Nave nave, Disparo disparo)
{
    return detectar_colision_generica(disparo.x, disparo.y, 4, 8, nave.x, nave.y, nave.ancho, nave.largo);
}


void disparar_radial(Disparo disparos[], int num_disparos, Nave nave)
{
    float centro_x;
    float centro_y;
    int num_disparos_radiales;
    float separacion_angular;
    float angulos[5];
    int i;
    int j;
    int offset;
    float punta_x;
    float punta_y;
    int indice_angulo;

    if (nave.nivel_disparo_radial == 0) 
    {
        disparar(disparos, num_disparos, nave);
        return;
    }

    // Calcular el centro de la nave
    centro_x = nave.x + nave.ancho / 2.0f;
    centro_y = nave.y + nave.largo / 2.0f;

    //num_disparos_radiales;
    //separacion_angular;

    // Determinar número de disparos según el nivel
    if (nave.nivel_disparo_radial == 1) 
    {
        num_disparos_radiales = 3; // Cambiar a 3 disparos para mejor distribución
        separacion_angular = ALLEGRO_PI / 8; // 22.5 grados entre disparos
    } 
    else if (nave.nivel_disparo_radial == 2) 
    {
        num_disparos_radiales = 5;
        separacion_angular = ALLEGRO_PI / 12; // 15 grados entre disparos (más cerrado)
    }

    if (num_disparos_radiales == 3)
    {
        // 3 disparos: izquierda, centro, derecha
        angulos[0] = nave.angulo - ALLEGRO_PI/2 - separacion_angular; // Izquierda
        angulos[1] = nave.angulo - ALLEGRO_PI/2;                     // Centro
        angulos[2] = nave.angulo - ALLEGRO_PI/2 + separacion_angular; // Derecha
    }
    else
    {
        for (j = 0; j < 5; j++)
        {
            offset = j - 2;
            angulos[j] = nave.angulo - ALLEGRO_PI/2 + offset * separacion_angular;
        }
    }

    // Calcular posición de disparo desde la punta de la nave
    punta_x = centro_x + cos(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);
    punta_y = centro_y + sin(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);

    indice_angulo = 0;
    for (i = 0; i < num_disparos && indice_angulo < num_disparos_radiales; i++)
    {
        if (!disparos[i].activo)
        {
            disparos[i].x = punta_x;
            disparos[i].y = punta_y;
            disparos[i].velocidad = 10;
            disparos[i].angulo = angulos[indice_angulo]; // Usar el ángulo precalculado
            disparos[i].activo = true;
            
            indice_angulo++; // Avanzar al siguiente ángulo
        }
    }
}


void verificar_mejora_disparo_radial(Nave *nave, ColaMensajes* cola_mensajes)
{
    char texto_mejora[100];

    if (nave->kills_para_mejora >= 16 && nave->nivel_disparo_radial < 2) 
    {
        nave->nivel_disparo_radial++;
        nave->kills_para_mejora = 0;
        
        // Mostrar mensaje de mejora
        sprintf(texto_mejora, "Disparo Radial Nivel %d Desbloqueado!", nave->nivel_disparo_radial);
        
        agregar_mensaje_cola(cola_mensajes, texto_mejora, 3.5, al_map_rgb(255, 255, 0), true);

        if (nave->nivel_disparo_radial == 1)
        {
            agregar_mensaje_cola(cola_mensajes, "Ahora disparas 3 proyectiles", 2.5, al_map_rgb(255, 255, 255), true);
        }
        else if (nave->nivel_disparo_radial == 2)
        {
            agregar_mensaje_cola(cola_mensajes, "Ahora disparas 5 proyectiles", 2.5, al_map_rgb(255, 255, 255), true);
        }
        
        printf("Mejora de disparo radial! Nivel actual: %d\n", nave->nivel_disparo_radial);
    }
}



void dibujar_nivel_powerup(Nave nave, ALLEGRO_FONT* fuente)
{
    char texto_powerup[50];
    if (nave.nivel_disparo_radial == 0)
    {
        sprintf(texto_powerup, "Radial: Desactivado (%d/16)", nave.kills_para_mejora);
    }
    else
    {
        sprintf(texto_powerup, "Radial Nv.%d (%d/16)", nave.nivel_disparo_radial, nave.kills_para_mejora);
    }

    al_draw_text(fuente, al_map_rgb(255, 255, 0), 10, 70, ALLEGRO_ALIGN_LEFT, texto_powerup);
}


void init_mensaje(Mensaje *mensaje)
{
    mensaje->activo = false;
    mensaje->x = 0;
    mensaje->y = 0;
    mensaje->texto[0] = '\0';
    mensaje->duracion = 0;
    mensaje->color = al_map_rgb(255, 255, 255);
}


void mostrar_mensaje(Mensaje *mensaje, const char *texto, float x, float y, double duracion, ALLEGRO_COLOR color)
{
    snprintf(mensaje->texto, sizeof(mensaje->texto), "%s", texto);

    mensaje->x = x;
    mensaje->y = y;
    mensaje->tiempo_inicio = al_get_time();
    mensaje->duracion = duracion;
    mensaje->activo = true;
    mensaje->color = color;
}


void actualizar_mensaje(Mensaje *mensaje, double tiempo_actual)
{
    if (mensaje->activo && (tiempo_actual - mensaje->tiempo_inicio >= mensaje->duracion)) 
    {
        mensaje->activo = false; // Desactivar mensaje después de la duración
    }
}


void dibujar_mensaje(Mensaje mensaje, ALLEGRO_FONT* fuente)
{
    int ancho_texto = al_get_text_width(fuente, mensaje.texto);
    int alto_texto = al_get_font_line_height(fuente);
    float x_final;
    float y_final;
    double tiempo_transcurrido = al_get_time() - mensaje.tiempo_inicio;
    double progreso = tiempo_transcurrido / mensaje.duracion;
    float alpha = 1.0f;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    float padding = 20;
    ALLEGRO_COLOR color_alpha;

    if (!mensaje.activo) return;

    if (mensaje.centrado)
    {
        x_final = mensaje.x - (ancho_texto / 2);
        y_final = mensaje.y - (alto_texto / 2);
    }
    else
    {
        x_final = mensaje.x;
        y_final = mensaje.y;
    }
    
    if (progreso < 0.2)
    {
        alpha = progreso / 0.2f;
    }
    else if (progreso > 0.8)
    {
        alpha = (1.0f - progreso) / 0.2f;
    }

    al_unmap_rgba(mensaje.color, &r, &g, &b, &a);
    color_alpha = al_map_rgba(r, g, b, (int)(alpha * 255));

    al_draw_filled_rectangle(x_final - padding, y_final - 10, x_final + ancho_texto + padding, y_final + alto_texto + 10, al_map_rgba(0, 0, 0, (int)(180 * alpha)));

    al_draw_rectangle(x_final - padding, y_final - 10, x_final + ancho_texto + padding, y_final + alto_texto + 10, al_map_rgba(255, 255, 255, (int)(150 * alpha)), 2);

    if (mensaje.centrado)
    {
        al_draw_text(fuente, color_alpha, mensaje.x, y_final, ALLEGRO_ALIGN_CENTER, mensaje.texto);
    }
    else
    {
        al_draw_text(fuente, color_alpha, x_final, y_final, ALLEGRO_ALIGN_LEFT, mensaje.texto);
    }
}


/**
 * @brief Inicializa el estado del juego.
 * 
 * @param estado Puntero al estado del juego a inicializar.
 */
void init_estado_juego(EstadoJuego *estado)
{
    estado->nivel_actual = 1;
    estado->todos_enemigos_eliminados = false;
    estado->mostrar_transicion = false;
    estado->tiempo_inicio_transicion = 0;
    estado->duracion_transicion = 3.0;
    estado->nivel_completado = false;
}


/**
 * @brief Verifica si todos los enemigos del nivel han sido eliminados.
 * 
 * @param enemigos Arreglo de enemigos del nivel.
 * @param num_enemigos Número de enemigos en el arreglo.
 * @return true si todos los enemigos están inactivos, false en caso contrario.
 */
bool verificar_nivel_completado(Enemigo enemigos[], int num_enemigos, bool hay_jefe_en_nivel, Jefe *jefe)
{
    int i;
    int enemigos_activos = 0;
    bool jefe_activo = false;

    if (hay_jefe_en_nivel && jefe != NULL) 
    {
        if (jefe->activo)
        {
            jefe->activo = true;
            printf("Jefe sigue activo\n");
        }
        else
        {
            printf("Jefe ha sido derrotado\n");
        }
    }

    for (i = 0; i < num_enemigos; i++)
    {
        if (enemigos[i].activo)
        {
            enemigos_activos++;
        }
    }
    
    if (hay_jefe_en_nivel)
    {
        if (jefe_activo || enemigos_activos > 0)
        {
            printf("DEBUG: Nivel no completado - Jefe: %s, Enemigos: %d\n", jefe_activo ? "VIVO" : "MUERTO", enemigos_activos);
            return false; // Nivel no completado
        }
        else
        {
            printf("DEBUG: NIVEL CON JEFE COMPLETADO! Jefe y todos los enemigos eliminados\n");
            return true; // Nivel completado: ni jefe ni enemigos
        }
    }
    else
    {
        // En niveles sin jefe, solo verificar enemigos normales
        if (enemigos_activos > 0)
        {
            printf("DEBUG: Nivel normal no completado - Enemigos restantes: %d\n", enemigos_activos);
            return false;
        }
        else
        {
            printf("DEBUG: NIVEL NORMAL COMPLETADO! Todos los enemigos eliminados\n");
            return true;
        }
    }
}


/**
 * @brief Muestra la pantalla de transición entre niveles.
 * 
 * @param nivel_completado Nivel que se acaba de completar.
 * @param nivel_siguiente Nivel que viene a continuación.
 * @param fuente Fuente para renderizar el texto.
 * @param tiempo_transcurrido Tiempo transcurrido de la transición.
 * @param duracion_total Duración total de la transición.
 */
void mostrar_pantalla_transicion(int nivel_completado, int siguiente_nivel, ALLEGRO_FONT *fuente, double tiempo_transcurrido, double duracion_total)
{
    float alpha;
    float progreso;
    float alpha_texto;
    ALLEGRO_COLOR color_titulo;
    ALLEGRO_COLOR color_subtitulo;
    ALLEGRO_COLOR color_progreso;

    // Textos de transición
    char texto_titulo[100];
    char texto_subtitulo[100];
    char texto_progreso[100];

    float barra_ancho;
    float barra_alto;
    float barra_x;
    float barra_y;

    float progreso_barra;

    // Fondo oscuro con efecto de fade
    alpha = 200.0f;
    al_draw_filled_rectangle(0, 0, 800, 600, al_map_rgba(0, 0, 0, (int)alpha));
    
    // Calcular el progreso de la animación (0.0 a 1.0)
    progreso = (float)(tiempo_transcurrido / duracion_total);
    if (progreso > 1.0f) progreso = 1.0f;
    
    // Efecto de aparición/desaparición del texto
    alpha_texto = 255.0f;

    if (progreso < 0.2f)
    {
        // Fade in en los primeros 20%
        alpha_texto = 255.0f * (progreso / 0.2f);
    }
    else if (progreso > 0.8f)
    {
        // Fade out en los últimos 20%
        alpha_texto = 255.0f * ((1.0f - progreso) / 0.2f);
    }
    
    // Colores con transparencia animada
    color_titulo = al_map_rgba(255, 255, 0, (int)alpha_texto);
    color_subtitulo = al_map_rgba(0, 255, 0, (int)alpha_texto);
    color_progreso = al_map_rgba(255, 255, 255, (int)alpha_texto);

    sprintf(texto_titulo, "Nivel %d Completado!", nivel_completado);
    
    if (siguiente_nivel == 4)
    {
        sprintf(texto_subtitulo, "ALERTA! Jefe Destructor detectado...");
        al_draw_text(fuente, al_map_rgba(255, 0, 0, (int)alpha_texto), 400, 350, ALLEGRO_ALIGN_CENTER, "Prepárate para el combate!");
    }
    else if (siguiente_nivel == 5)
    {
        sprintf(texto_subtitulo, "PELIGRO MÁXIMO! Jefe Supremo aproximándose...");
        al_draw_text(fuente, al_map_rgba(255, 0, 0, (int)alpha_texto), 400, 350, ALLEGRO_ALIGN_CENTER, "Este es el enemigo final!");
    }
    else if (siguiente_nivel > 5) 
    {
        sprintf(texto_titulo, "MISIÓN COMPLETADA!");
        sprintf(texto_subtitulo, "Has salvado la galaxia!");
        al_draw_text(fuente, al_map_rgba(0, 255, 0, (int)alpha_texto), 400, 350, ALLEGRO_ALIGN_CENTER, "Felicidades, héroe!");
    }
    else
    {
        sprintf(texto_subtitulo, "Preparando Nivel %d...", siguiente_nivel);
    }
    
    sprintf(texto_progreso, "%.0f%%", progreso * 100);
    
    // Dibujar textos centrados
    al_draw_text(fuente, color_titulo, 400, 250, ALLEGRO_ALIGN_CENTER, texto_titulo);
    al_draw_text(fuente, color_subtitulo, 400, 300, ALLEGRO_ALIGN_CENTER, texto_subtitulo);
    
    // Barra de progreso
    barra_ancho = 300;
    barra_alto = 20;
    barra_x = 400 - barra_ancho / 2;
    barra_y = 380;
    
    // Fondo de la barra
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, al_map_rgba(100, 100, 100, (int)alpha_texto));
    
    // Progreso de la barra
    progreso_barra = barra_ancho * progreso;
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + progreso_barra, barra_y + barra_alto, al_map_rgba(0, 255, 0, (int)alpha_texto));
    
    // Borde de la barra
    al_draw_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, color_progreso, 2);
    
    // Texto de progreso
    al_draw_text(fuente, color_progreso, 400, 380, ALLEGRO_ALIGN_CENTER, texto_progreso);
}


/**
 * @brief Carga el siguiente nivel del juego.
 * 
 * @param nivel Número del nivel a cargar.
 * @param tilemap Matriz de tiles del nivel.
 * @param enemigos_mapa Arreglo donde se cargarán los enemigos del nivel.
 * @param num_enemigos_cargados Puntero al número de enemigos cargados.
 * @param imagen_enemigo Imagen del enemigo.
 * @param nave_x Puntero a la coordenada x inicial de la nave.
 * @param nave_y Puntero a la coordenada y inicial de la nave.
 * @return true si se cargó correctamente, false si no existe el nivel.
 */
bool cargar_siguiente_nivel(int nivel, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos_mapa[], int* num_enemigos_cargados, ALLEGRO_BITMAP* imagen_enemigo, float* nave_x, float* nave_y) {
    char nombre_archivo[50];
    int f;
    int c;

    sprintf(nombre_archivo, "Nivel%d.txt", nivel);
    
    // Verificar si el archivo existe
    FILE* test = fopen(nombre_archivo, "r");
    if (!test) {
        printf("No se encontró el archivo %s. Fin del juego.\n", nombre_archivo);
        return false;
    }
    fclose(test);
    
    // Limpiar el tilemap anterior
    for (f = 0; f < MAPA_FILAS; f++) {
        for (c = 0; c < MAPA_COLUMNAS; c++) {
            tilemap[f][c].tipo = 0;
            tilemap[f][c].vida = 0;
        }
    }
    
    // Cargar el nuevo nivel
    cargar_tilemap(nombre_archivo, tilemap, enemigos_mapa, num_enemigos_cargados, imagen_enemigo, nave_x, nave_y);
    
    printf("Nivel %d cargado: %d enemigos encontrados.\n", nivel, *num_enemigos_cargados);
    return true;
}

/**
 * @brief Actualiza el estado del nivel y maneja las transiciones.
 * 
 * @param estado Puntero al estado del juego.
 * @param enemigos Arreglo de enemigos del nivel actual.
 * @param num_enemigos Número de enemigos en el arreglo.
 * @param tiempo_actual Tiempo actual del juego.
 */
void actualizar_estado_nivel(EstadoJuego* estado, Enemigo enemigos[], int num_enemigos, double tiempo_actual, bool hay_jefe_en_nivel, Jefe *jefe) {
    // Verificar si todos los enemigos han sido eliminados
    if (!estado->todos_enemigos_eliminados && verificar_nivel_completado(enemigos, num_enemigos, hay_jefe_en_nivel, jefe)) 
    {
        estado->todos_enemigos_eliminados = true;
        estado->mostrar_transicion = true;
        estado->tiempo_inicio_transicion = tiempo_actual;
        estado->nivel_completado = true;
        
        printf("Nivel %d completado! Iniciando transición...\n", estado->nivel_actual);
    }
    
    // Manejar la transición
    if (estado->mostrar_transicion) {
        double tiempo_transcurrido = tiempo_actual - estado->tiempo_inicio_transicion;
        
        if (tiempo_transcurrido >= estado->duracion_transicion) {
            // Finalizar transición
            estado->nivel_completado = true;
            estado->mostrar_transicion = false;
            
            printf("Transición completada. Avanzando al nivel %d.\n", estado->nivel_actual);
        }
    }
}


bool asteroides_activados(int nivel_actual)
{
    return nivel_actual == 1;
}


/**
 * @brief Inicializa un enemigo según su tipo específico.
 * 
 * Configura las propiedades del enemigo (vida, velocidad, tamaño, intervalo de disparo)
 * según el tipo especificado y lo posiciona en las coordenadas del tilemap.
 * 
 * @param enemigo Puntero al enemigo a inicializar.
 * @param col Columna en el tilemap donde aparecerá.
 * @param fila Fila en el tilemap donde aparecerá.
 * @param tipo Tipo de enemigo (0-4).
 * @param imagen_enemigo Imagen temporal (se reemplazará por la específica después).
 */
void init_enemigo_tipo(Enemigo* enemigo, int col, int fila, int tipo, ALLEGRO_BITMAP* imagen_enemigo)
{
    enemigo->x = col * TILE_ANCHO;
    enemigo->y = fila * TILE_ALTO;
    enemigo->activo = true;
    enemigo->ultimo_disparo = 0;
    enemigo->imagen = imagen_enemigo;
    enemigo->tipo = tipo;
    
    switch (tipo)
    {
        case 0: // Enemigo normal
            enemigo->ancho = 50;
            enemigo->alto = 40;
            enemigo->velocidad = 1.0f;
            enemigo->vida = 2.0f;
            enemigo->vida_max = 2;
            enemigo->intervalo_disparo = 2.0 + (rand() % 100) / 100.0;
            break;
            
        case 1: // Perseguidor
            enemigo->ancho = 45;
            enemigo->alto = 35;
            enemigo->velocidad = 0.8f;
            enemigo->vida = 2.0f;
            enemigo->vida_max = 2;
            enemigo->intervalo_disparo = 2.5;
            break;
            
        case 2: // Francotirador
            enemigo->ancho = 40;
            enemigo->alto = 30;
            enemigo->velocidad = 0; // Inmóvil
            enemigo->vida = 1.0f;
            enemigo->vida_max = 1;
            enemigo->intervalo_disparo = 1.5;
            break;
            
        case 3: // Tanque
            enemigo->ancho = 70;
            enemigo->alto = 50;
            enemigo->velocidad = 0.3f;
            enemigo->vida = 6.0f;
            enemigo->vida_max = 6;
            enemigo->intervalo_disparo = 3.0;
            break;
            
        case 4: // Kamikaze
            enemigo->ancho = 35;
            enemigo->alto = 30;
            enemigo->velocidad = 1.5f;
            enemigo->vida = 1.0f;
            enemigo->vida_max = 1;
            enemigo->intervalo_disparo = 999; // No dispara
            break;

        case 5: // Jefe Destructor
            enemigo->ancho = 120;
            enemigo->alto = 80;
            enemigo->velocidad = 1.0f;
            enemigo->vida = 150.0f;
            enemigo->vida_max = 150.0f;
            enemigo->intervalo_disparo = 999; // Los jefes no usan disparos normales
            break;
    
        case 6: // Jefe Supremo
            enemigo->ancho = 160;
            enemigo->alto = 100;
            enemigo->velocidad = 1.5f;
            enemigo->vida = 250.0f;
            enemigo->vida_max = 250.0f;
            enemigo->intervalo_disparo = 999; // Los jefes no usan disparos normales
            break;
            
        default:
            // Valores por defecto
            enemigo->ancho = 50;
            enemigo->alto = 40;
            enemigo->velocidad = 1.0f;
            enemigo->vida = 1.0f;
            enemigo->vida_max = 1;
            enemigo->intervalo_disparo = 2.0;
            break;
    }

    printf("Enemigo tipo %d inicializado: vel=%.1f, vida=%.1f, tamaño=%dx%d\n", tipo, enemigo->velocidad, enemigo->vida, (int)enemigo->ancho, (int)enemigo->alto);
}


/**
 * @brief Disparo especializado del francotirador que apunta directamente a la nave.
 * 
 * Calcula el ángulo hacia la posición actual de la nave y crea un disparo
 * dirigido con alta precisión.
 * 
 * @param disparos Arreglo de disparos de enemigos.
 * @param num_disparos Número total de disparos disponibles.
 * @param enemigo Enemigo francotirador que realiza el disparo.
 * @param nave Nave objetivo para calcular la trayectoria.
 */
void francotirador_disparar(Disparo disparos[], int num_disparos, Enemigo enemigo, Nave nave)
{
    int i;
    float dx;
    float dy;
    float angulo_hacia_nave;

    for(i = 0; i < num_disparos; i++)
    {
        if (!disparos[i].activo)
        {
            // Calcular ángulo hacia la nave
            dx = nave.x + nave.ancho/2 - (enemigo.x + enemigo.ancho/2);
            dy = nave.y + nave.largo/2 - (enemigo.y + enemigo.alto/2);
            angulo_hacia_nave = atan2(dy, dx);
            
            disparos[i].x = enemigo.x + enemigo.ancho / 2;
            disparos[i].y = enemigo.y + enemigo.alto;
            disparos[i].velocidad = 4.0f; // Más rápido que disparos normales
            disparos[i].angulo = angulo_hacia_nave;
            disparos[i].activo = true;
            break;
        }
    }
}


/**
 * @brief Disparo especializado del tanque que crea múltiples proyectiles en abanico.
 * 
 * El tanque dispara 3 proyectiles simultáneamente en diferentes ángulos
 * para cubrir un área más amplia de ataque.
 * 
 * @param disparos Arreglo de disparos de enemigos.
 * @param num_disparos Número total de disparos disponibles.
 * @param enemigo Enemigo tanque que realiza el disparo.
 */
void tanque_disparar(Disparo disparos[], int num_disparos, Enemigo enemigo)
{
    // El tanque dispara 3 proyectiles en abanico
    float angulos[3] = {
        ALLEGRO_PI / 2 - 0.3f, // Izquierda
        ALLEGRO_PI / 2,        // Centro
        ALLEGRO_PI / 2 + 0.3f  // Derecha
    };
    
    int disparos_creados = 0;
    int i;

    for(i = 0; i < num_disparos && disparos_creados < 3; i++)
    {
        if (!disparos[i].activo)
        {
            disparos[i].x = enemigo.x + enemigo.ancho / 2;
            disparos[i].y = enemigo.y + enemigo.alto;
            disparos[i].velocidad = 2.5f; // Más lento pero más daño
            disparos[i].angulo = angulos[disparos_creados];
            disparos[i].activo = true;
            disparos_creados++;
        }
    }
}


bool detectar_colision_disparo_enemigo_escudo(Disparo disparo, float tile_x, float tile_y)
{
    return (disparo.x >= tile_x && disparo.x <= tile_x + TILE_ANCHO && disparo.y >= tile_y && disparo.y <= tile_y + TILE_ALTO);
}


void init_powerup(Powerup *powerup)
{
    powerup->x = 0;
    powerup->y = 0;
    powerup->activo = false;
    powerup->tipo = 0;
    powerup->tiempo_aparicion = 0;
    powerup->duracion_vida = 10.0;
    powerup->color = al_map_rgb(0, 255, 255);
}


void crear_powerup_escudo(Powerup powerups[], int max_powerups, float x, float y)
{
    int i;

    for (i = 0; i < max_powerups; i++)
    {
        if (!powerups[i].activo)
        {
            powerups[i].x = x;
            powerups[i].y = y;
            powerups[i].tipo = 0;
            powerups[i].activo = true;
            powerups[i].tiempo_aparicion = al_get_time();
            powerups[i].duracion_vida = 20.0;
            powerups[i].color = al_map_rgb(0, 255, 255);
            printf("Powerup de escudo creado en (%.0f, %.0f)\n", x, y);
            break;
        }
    }
}


void actualizar_powerups(Powerup powerups[], int max_powerups, double tiempo_actual)
{
    int i;
    static double ultimo_debug = 0;
    double tiempo_restante;

    for (i = 0; i < max_powerups; i++)
    {
        if (powerups[i].activo)
        {
            powerups[i].y += 1.5f;

            if (powerups[i].y > 600) // Sale por la parte inferior
            {
                powerups[i].activo = false;
                printf("Powerup se perdió al salir de la pantalla.\n");
            }
            else if (tiempo_actual - powerups[i].tiempo_aparicion >= powerups[i].duracion_vida)
            {
                powerups[i].activo = false;
                printf("Powerup expirado por tiempo.\n");
            }
            else
            {
                if (tiempo_actual - ultimo_debug >= 3.0)
                {
                    tiempo_restante = powerups[i].duracion_vida - (tiempo_actual - powerups[i].tiempo_aparicion);
                    printf("Powerup %d activo: pos(%.1f, %.1f), tiempo restante: %.1fs\n", i, powerups[i].x, powerups[i].y, tiempo_restante);
                    ultimo_debug = tiempo_actual;
                }
            }
        }
    }
}


void dibujar_powerups(Powerup powerups[], int max_powerups, int *contador_parpadeo, int *contador_debug, ALLEGRO_FONT *fuente)
{
    int i;
    int j;
    int powerups_activos;
    float cx;
    float cy;
    ALLEGRO_COLOR color_powerup;
    float hex_x[6];
    float hex_y[6];

    *contador_parpadeo = (*contador_parpadeo + 1) % 60; // Incrementar contador de parpadeo

    if (++(*contador_debug) % 120 == 0)
    {
        powerups_activos = 0;

        for (i = 0; i < max_powerups; i++)
        {
            if (powerups[i].activo)
            {
                powerups_activos++;
            }
        }
        printf("Powerups activos: %d\n", powerups_activos);
    }
    

    for (i = 0; i < max_powerups; i++)
    {
        if (powerups[i].activo)
        {
            cx = powerups[i].x + 15;
            cy = powerups[i].y + 15;
            color_powerup = powerups[i].color;
            
            if (powerups[i].tipo == 0)
            {
                // ... código de dibujo del escudo ...
                al_draw_filled_circle(cx, cy, 16, al_map_rgba(0, 255, 255, 50));
                al_draw_filled_circle(cx, cy, 12, al_map_rgba(0, 255, 255, 80)); 
                al_draw_circle(cx, cy, 12, color_powerup, 2);

                for (j = 0; j < 6; j++)
                {
                    hex_x[j] = cx + cos(j * ALLEGRO_PI / 3) * 8;
                    hex_y[j] = cy + sin(j * ALLEGRO_PI / 3) * 8;
                }
                
                for (j = 0; j < 6; j++)
                {
                    al_draw_line(hex_x[j], hex_y[j], hex_x[(j + 1) % 6], hex_y[(j + 1) % 6], color_powerup, 2);
                }
                
                al_draw_line(cx - 5, cy, cx + 5, cy, color_powerup, 2);
                al_draw_line(cx, cy - 5, cx, cy + 5, color_powerup, 2);

                for (j = 0; j < 6; j++)
                {
                    al_draw_filled_circle(hex_x[j], hex_y[j], 1.5f, color_powerup);
                }
                
                if (*contador_parpadeo < 15)
                {
                    al_draw_text(fuente, al_map_rgb(200, 255, 255), cx, cy + 20, ALLEGRO_ALIGN_CENTER, "ESCUDO");
                }
            }
            else if (powerups[i].tipo == 1) // Vida
            {
                // ... código similar para vida ...
                al_draw_filled_circle(cx, cy, 16, al_map_rgba(255, 0, 0, 50));
                al_draw_filled_circle(cx, cy, 12, al_map_rgba(255, 0, 0, 80));
                al_draw_circle(cx, cy, 12, color_powerup, 2);
                
                al_draw_line(cx - 5, cy, cx + 5, cy, color_powerup, 2);
                al_draw_line(cx, cy - 5, cx, cy + 5, color_powerup, 2);
                
                if (*contador_parpadeo < 15)
                {
                    al_draw_text(fuente, al_map_rgb(255, 150, 150), cx, cy + 20, ALLEGRO_ALIGN_CENTER, "VIDA");
                }
            }
            else if (powerups[i].tipo == 2) // Láser
            {
                al_draw_filled_circle(cx, cy, 16, al_map_rgba(255, 0, 0, 50));
                al_draw_filled_circle(cx, cy, 12, al_map_rgba(255, 0, 0, 80));
                al_draw_circle(cx, cy, 12, color_powerup, 2);
                
                al_draw_line(cx - 8, cy, cx + 8, cy, color_powerup, 3);
                al_draw_line(cx - 6, cy - 3, cx + 6, cy - 3, color_powerup, 2);
                al_draw_line(cx - 6, cy + 3, cx + 6, cy + 3, color_powerup, 2);
                
                if (*contador_parpadeo < 15)
                {
                    al_draw_text(fuente, al_map_rgb(255, 200, 200), cx, cy + 20, ALLEGRO_ALIGN_CENTER, "LÁSER");
                }
            }
            else if (powerups[i].tipo == 3) // Explosivo
            {
                al_draw_filled_circle(cx, cy, 16, al_map_rgba(255, 100, 0, 50));
                al_draw_filled_circle(cx, cy, 12, al_map_rgba(255, 100, 0, 80));
                al_draw_circle(cx, cy, 12, color_powerup, 2);
                
                for (j = 0; j < 8; j++)
                {
                    float angulo = j * ALLEGRO_PI / 4;
                    float x1 = cx + cos(angulo) * 4;
                    float y1 = cy + sin(angulo) * 4;
                    float x2 = cx + cos(angulo) * 8;
                    float y2 = cy + sin(angulo) * 8;
                    al_draw_line(x1, y1, x2, y2, color_powerup, 2);
                }
                
                if (*contador_parpadeo < 15)
                {
                    al_draw_text(fuente, al_map_rgb(255, 200, 100), cx, cy + 20, ALLEGRO_ALIGN_CENTER, "BOOM");
                }
            }
            else if (powerups[i].tipo == 4) // Misil
            {
                al_draw_filled_circle(cx, cy, 16, al_map_rgba(0, 255, 100, 50));
                al_draw_filled_circle(cx, cy, 12, al_map_rgba(0, 255, 100, 80));
                al_draw_circle(cx, cy, 12, color_powerup, 2);
                
                al_draw_line(cx - 6, cy, cx + 6, cy, color_powerup, 3);
                al_draw_line(cx + 6, cy, cx + 3, cy - 3, color_powerup, 2);
                al_draw_line(cx + 6, cy, cx + 3, cy + 3, color_powerup, 2);
                al_draw_line(cx - 6, cy - 2, cx - 9, cy - 4, color_powerup, 1);
                al_draw_line(cx - 6, cy + 2, cx - 9, cy + 4, color_powerup, 1);
                
                if (*contador_parpadeo < 15)
                {
                    al_draw_text(fuente, al_map_rgb(200, 255, 200), cx, cy + 20, ALLEGRO_ALIGN_CENTER, "MISIL");
                }
            }
        }
    }
}


bool detectar_colision_powerup(Nave nave, Powerup powerup)
{
    return detectar_colision_generica(nave.x, nave.y, nave.ancho, nave.largo, powerup.x, powerup.y, 30, 30);
}


void recoger_powerup(Nave *nave, Powerup *powerup, ColaMensajes *cola_mensajes)
{
    if (powerup->tipo == 0) // escudo
    {
        activar_escudo(&nave->escudo, 3);
        agregar_mensaje_cola(cola_mensajes, "Escudo Activado!", 2.5, al_map_rgb(0, 255, 255), true);
        agregar_mensaje_cola(cola_mensajes, "Resistencia a 3 impactos", 2.0, al_map_rgb(255, 255, 255), true);
    }
    else if (powerup->tipo == 1)
    {
        nave->vida += 25.0f;
        if (nave->vida > 100.f)
        {
            nave->vida = 100.f;
        }
        agregar_mensaje_cola(cola_mensajes, "Vida restaurada", 2.0, al_map_rgb(0, 255, 0), true);
        agregar_mensaje_cola(cola_mensajes, "Recuperaste 25 puntos de vida!", 1.5, al_map_rgb(255, 255, 255), true);
    }
    else if (powerup->tipo == 2) 
    {
        if (!nave->armas[Arma_laser].desbloqueado)
        {
            nave->armas[Arma_laser].desbloqueado = true;
            agregar_mensaje_cola(cola_mensajes, "NUEVO ARMA DESBLOQUEADA!", 3.0, al_map_rgb(255, 215, 0), true);
            agregar_mensaje_cola(cola_mensajes, "Láser Continuo", 2.5, al_map_rgb(0, 255, 255), true);
            agregar_mensaje_cola(cola_mensajes, "Presiona [2] para usar", 3.0, al_map_rgb(255, 255, 255), true);
            printf("Arma láser desbloqueada!\n");
        }
        else
        {
            agregar_mensaje_cola(cola_mensajes, "Arma Láser ya desbloqueada", 2.0, al_map_rgb(255, 255, 0), true);
        }
    }
    else if (powerup->tipo == 3) 
    {
        if (!nave->armas[Arma_explosiva].desbloqueado)
        {
            nave->armas[Arma_explosiva].desbloqueado = true;
            agregar_mensaje_cola(cola_mensajes, "NUEVO ARMA DESBLOQUEADA!", 3.0, al_map_rgb(255, 215, 0), true);
            agregar_mensaje_cola(cola_mensajes, "Cañón Explosivo", 2.5, al_map_rgb(255, 100, 0), true);
            agregar_mensaje_cola(cola_mensajes, "Presiona [3] para usar", 3.0, al_map_rgb(255, 255, 255), true);
            printf("Arma explosiva desbloqueada!\n");
        }
        else
        {
            agregar_mensaje_cola(cola_mensajes, "Arma Explosiva ya desbloqueada", 2.0, al_map_rgb(255, 255, 0), true);
        }
    }
    else if (powerup->tipo == 4) 
    {
        if (!nave->armas[Arma_misil].desbloqueado)
        {
            nave->armas[Arma_misil].desbloqueado = true;
            agregar_mensaje_cola(cola_mensajes, "NUEVO ARMA DESBLOQUEADA!", 3.0, al_map_rgb(255, 215, 0), true);
            agregar_mensaje_cola(cola_mensajes, "Misiles Teledirigidos", 2.5, al_map_rgb(255, 0, 255), true);
            agregar_mensaje_cola(cola_mensajes, "Presiona [4] para usar", 3.0, al_map_rgb(255, 255, 255), true);
            printf("Arma de misiles desbloqueada!\n");
        }
        else
        {
            agregar_mensaje_cola(cola_mensajes, "Arma Misil ya desbloqueada", 2.0, al_map_rgb(255, 255, 0), true);
        }
    }
    
    powerup->activo = false;
    printf("Powerup recogido y desactivado correctamente\n");
}


void init_escudo(Escudo* escudo)
{
    escudo->activo = false;
    escudo->hits_max = 0;
    escudo->hits_restantes = 0;
    escudo->tiempo_activacion = 0;
    escudo->color = al_map_rgb(0, 255, 255);
    escudo->intensidad = 1.0f;
}


void activar_escudo(Escudo* escudo, int hits_maximos)
{
    escudo->activo = true;
    escudo->hits_max = hits_maximos;
    escudo->hits_restantes = hits_maximos;
    escudo->tiempo_activacion = al_get_time();
    escudo->intensidad = 1.0f;
    escudo->color = al_map_rgb(0, 255, 255);
}


void actualizar_escudo(Escudo *escudo, double tiempo_actual)
{
    if (!escudo->activo) return;
    
    if (escudo->hits_restantes == 1)
    {
        escudo->intensidad = 0.7f + 0.3f * sin(tiempo_actual * 8);
    }
    else if (escudo->hits_restantes == 2) {
        escudo->intensidad = 0.8f + 0.2f * sin(tiempo_actual * 4);
    } 
    else 
    {
        escudo->intensidad = 0.0f;
    }
}


void dibujar_escudo(Nave nave)
{
    float centro_x = nave.x + nave.ancho / 2;
    float centro_y = nave.y + nave.largo / 2;
    float radio = (nave.ancho + nave.largo) / 3.0f + 10;
    unsigned char r, g, b, a;
    ALLEGRO_COLOR color_escudo;

    if (!nave.escudo.activo) return;
    
    al_unmap_rgba(nave.escudo.color, &r, &g, &b, &a);
    
    color_escudo = al_map_rgba(r, g, b, (int)(nave.escudo.intensidad * 150));
    
    al_draw_circle(centro_x, centro_y, radio, color_escudo, 3);
    al_draw_circle(centro_x, centro_y, radio - 2, color_escudo, 2);
}


bool escudo_activo(Nave nave)
{
    return nave.escudo.activo && nave.escudo.hits_restantes > 0;
}


bool escudo_recibir_dano(Escudo* escudo)
{
    if (!escudo->activo || escudo->hits_restantes <= 0) 
    {
        printf("Escudo no disponible para absorver dano\n");
        return false;
    }
    
    escudo->hits_restantes--;
    printf("Escudo recibió daño. Hits restantes: %d\n", escudo->hits_restantes);
    
    if (escudo->hits_restantes <= 0)
    {
        escudo->activo = false;
        printf("Escudo destruido!\n");
    }

    return true;
}


bool verificar_colision_nave_muro(float x, float y, float ancho, float largo, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS])
{
    int fila;
    int col;

    float centro_x = x + ancho / 2;
    float centro_y = y + largo / 2;
    float radio = (ancho * 0.6f) / 2.0f;
    float tile_centro_x;
    float tile_centro_y;
    float tile_radio;
    
    // Calcular qué tiles ocupa la nave
    int col_izquierda = (int)(x / TILE_ANCHO);
    int col_derecha = (int)((x + ancho - 1) / TILE_ANCHO);
    int fila_superior = (int)(y / TILE_ALTO);
    int fila_inferior = (int)((y + largo - 1) / TILE_ALTO);

    // Verificar todos los tiles que ocuparía la nave
    for (fila = fila_superior; fila <= fila_inferior; fila++)
    {
        for (col = col_izquierda; col <= col_derecha; col++)
        {
            // Verificar si el tile está dentro del mapa
            if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS)
            {
                // Verificar colisión con muros indestructibles (tipo 3), escudos (tipo 2) o asteroides fijos (tipo 1)
                if (tilemap[fila][col].tipo == 3 || tilemap[fila][col].tipo == 1)
                {
                    tile_centro_x = col * TILE_ANCHO + TILE_ANCHO / 2;
                    tile_centro_y = fila * TILE_ALTO + TILE_ALTO / 2;
                    tile_radio = TILE_ANCHO / 2.0f; // Radio del tile
                    
                    if (detectar_colision_circular(centro_x, centro_y, radio, tile_centro_x, tile_centro_y, tile_radio))
                    {
                        return true; // Hay colisión
                    }
                }
            }
        }
    }
    
    return false; // No hay colisión
}


void init_cola_mensajes(ColaMensajes *cola)
{
    cola->inicio = 0;
    cola->fin = 0;
    cola->cantidad = 0;
    cola->procesando = false;
    init_mensaje(&cola->mensaje_actual);
}


void agregar_mensaje_cola(ColaMensajes *cola, const char *texto, double duracion, ALLEGRO_COLOR color, bool centrado)
{
    if (cola->cantidad >= MAX_COLA_MENSAJES)
    {
        printf("Cola de mensajes llena! No se puede agregar el mensaje: %s\n", texto);
        return;
    }

    snprintf(cola->mensajes[cola->fin].texto, sizeof(cola->mensajes[cola->fin].texto), "%s", texto);

    cola->mensajes[cola->fin].duracion = duracion;
    cola->mensajes[cola->fin].color = color;
    cola->mensajes[cola->fin].centrado = centrado;

    cola->fin = (cola->fin + 1) % MAX_COLA_MENSAJES;
    cola->cantidad++;
    
    printf("Mensaje agregado a la cola: %s (Total en cola: %d)\n", texto, cola->cantidad);
}


void actualizar_cola_mensajes(ColaMensajes* cola, double tiempo_actual)
{
    static int debug_counter = 0;
    if (!cola)
    {
        return;
    }

    if (!cola->mensaje_actual.activo && cola->cantidad > 0) {
        MensajeEnCola siguiente = cola->mensajes[cola->inicio];
        
        if (siguiente.centrado) 
        {
            mostrar_mensaje_centrado(&cola->mensaje_actual, siguiente.texto, siguiente.duracion, siguiente.color);
        }
        else 
        {
            mostrar_mensaje(&cola->mensaje_actual, siguiente.texto, 200, 150, siguiente.duracion, siguiente.color);
        }
        
        // Remover mensaje de la cola
        cola->inicio = (cola->inicio + 1) % MAX_COLA_MENSAJES;
        cola->cantidad--;
        
        if (++debug_counter % 30 == 0) 
        {
            printf("Procesando mensaje: %s (Restantes en cola: %d)\n", siguiente.texto, cola->cantidad);
        }
    }
    
    // Actualizar mensaje actual
    if (cola->mensaje_actual.activo) {
        actualizar_mensaje(&cola->mensaje_actual, tiempo_actual);
        
        // Si el mensaje terminó, marcar como no procesando
        if (!cola->mensaje_actual.activo)
        {
            cola->procesando = false;
        }
    }
}


void dibujar_cola_mensajes(ColaMensajes cola, ALLEGRO_FONT* fuente)
{
    if (cola.mensaje_actual.activo)
    {
        dibujar_mensaje(cola.mensaje_actual, fuente);
    }
}


void mostrar_mensaje_centrado(Mensaje* mensaje, const char* texto, double duracion, ALLEGRO_COLOR color)
{
    snprintf(mensaje->texto, sizeof(mensaje->texto), "%s", texto);

    mensaje->x = 400; // Centro de la pantalla (800/2)
    mensaje->y = 200; // Posición vertical centrada
    mensaje->tiempo_inicio = al_get_time();
    mensaje->duracion = duracion;
    mensaje->activo = true;
    mensaje->color = color;
    mensaje->centrado = true;
}


/**
 * @brief Dibuja las hitboxes de todos los objetos para depuración.
 * 
 * @param nave Nave del jugador.
 * @param enemigos Arreglo de enemigos.
 * @param num_enemigos Número de enemigos.
 * @param disparos Disparos del jugador.
 * @param num_disparos Número de disparos del jugador.
 * @param disparos_enemigos Disparos de enemigos.
 * @param num_disparos_enemigos Número de disparos de enemigos.
 * @param asteroides Arreglo de asteroides.
 * @param num_asteroides Número de asteroides.
 * @param tilemap Mapa de tiles.
 */
void dibujar_hitboxes_debug(Nave nave, Enemigo enemigos[], int num_enemigos, Disparo disparos[], int num_disparos, Disparo disparos_enemigos[], int num_disparos_enemigos, Asteroide asteroides[], int num_asteroides, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], ALLEGRO_FONT *fuente)
{
    float centro_nave_x = nave.x + nave.ancho / 2;
    float centro_nave_y = nave.y + nave.largo / 2;
    obtener_centro_nave(nave, &centro_nave_x, &centro_nave_y);
    float radio_nave = obtener_radio_nave(nave);
    float radio_visual;
    char radio_texto[30];
    float radio_escudo;
    int i;
    ALLEGRO_COLOR color_enemigo;
    float centro_x;
    float centro_y;
    char tipo_texto[20];
    int fila;
    int col;
    float x;
    float y;
    ALLEGRO_COLOR color_tile;
    char vida_texto[12];
    
    // Leyenda en la esquina superior derecha
    int leyenda_x = 600;
    int leyenda_y = 10;

    // HITBOX REAL DE LA NAVE (más pequeña)
    al_draw_circle(centro_nave_x, centro_nave_y, radio_nave, al_map_rgb(0, 255, 0), 2);
    
    // MOSTRAR HITBOX VISUAL VS HITBOX REAL
    radio_visual = (nave.ancho * 0.7f) / 2.0f; // Hitbox visual anterior
    al_draw_circle(centro_nave_x, centro_nave_y, radio_visual, al_map_rgb(100, 255, 100), 1); // Más tenue
    
    // Centro de la nave
    al_draw_filled_circle(centro_nave_x, centro_nave_y, 3, al_map_rgb(0, 255, 0));
    
    sprintf(radio_texto, "R:%.1f", radio_nave);
    al_draw_text(fuente, al_map_rgb(0, 255, 0), centro_nave_x + radio_nave + 5, centro_nave_y - 10, ALLEGRO_ALIGN_LEFT, radio_texto);

    // Hitbox del escudo si está activo - Cian
    if (nave.escudo.activo)
    {
        radio_escudo = (nave.ancho + nave.largo) / 3.0f + 10;
        al_draw_circle(centro_nave_x, centro_nave_y, radio_escudo, al_map_rgb(0, 255, 255), 1);
    }

    // Hitboxes de enemigos - Rojo con diferentes tonos según tipo
    for (i = 0; i < num_enemigos; i++)
    {
        if (enemigos[i].activo)
        {
            switch (enemigos[i].tipo)
            {
                case 0: color_enemigo = al_map_rgb(255, 0, 0); break;     // Normal - rojo
                case 1: color_enemigo = al_map_rgb(255, 100, 0); break;   // Perseguidor - naranja
                case 2: color_enemigo = al_map_rgb(255, 0, 100); break;   // Francotirador - magenta
                case 3: color_enemigo = al_map_rgb(200, 0, 0); break;     // Tanque - rojo oscuro
                case 4: color_enemigo = al_map_rgb(255, 200, 0); break;   // Kamikaze - amarillo
                default: color_enemigo = al_map_rgb(255, 0, 0); break;
            }
            
            al_draw_rectangle(enemigos[i].x, enemigos[i].y, enemigos[i].x + enemigos[i].ancho, enemigos[i].y + enemigos[i].alto, color_enemigo, 2);
            
            // Centro del enemigo
            centro_x = enemigos[i].x + enemigos[i].ancho / 2;
            centro_y = enemigos[i].y + enemigos[i].alto / 2;
            al_draw_filled_circle(centro_x, centro_y, 2, color_enemigo);
            
            // Texto con tipo de enemigo
            sprintf(tipo_texto, "T%d", enemigos[i].tipo);
            al_draw_text(fuente, al_map_rgb(255, 255, 255), centro_x, centro_y - 15, ALLEGRO_ALIGN_CENTER, tipo_texto);
        }
    }

    // Hitboxes de disparos del jugador - Azul
    for (i = 0; i < num_disparos; i++)
    {
        if (disparos[i].activo)
        {
            al_draw_rectangle(disparos[i].x, disparos[i].y, disparos[i].x + 5, disparos[i].y + 10, al_map_rgb(0, 0, 255), 1);
        }
    }

    // Hitboxes de disparos de enemigos - Amarillo
    for (i = 0; i < num_disparos_enemigos; i++)
    {
        if (disparos_enemigos[i].activo)
        {
            al_draw_rectangle(disparos_enemigos[i].x, disparos_enemigos[i].y, disparos_enemigos[i].x + 4, disparos_enemigos[i].y + 8, al_map_rgb(255, 255, 0), 1);
        }
    }

    // Hitboxes de asteroides - Blanco
    for (i = 0; i < num_asteroides; i++)
    {
        al_draw_rectangle(asteroides[i].x, asteroides[i].y, asteroides[i].x + asteroides[i].ancho, asteroides[i].y + asteroides[i].alto, al_map_rgb(255, 255, 255), 1);
    }

    // Hitboxes del tilemap - Varios colores
    for (fila = 0; fila < MAPA_FILAS; fila++)
    {
        for (col = 0; col < MAPA_COLUMNAS; col++)
        {
            if (tilemap[fila][col].tipo > 0)
            {
                x = col * TILE_ANCHO;
                y = fila * TILE_ALTO;
                
                switch (tilemap[fila][col].tipo)
                {
                    case 1: color_tile = al_map_rgb(128, 128, 128); break; // Asteroide fijo - gris
                    case 2: color_tile = al_map_rgb(0, 128, 255); break;   // Escudo - azul
                    case 3: color_tile = al_map_rgb(80, 80, 80); break;    // Bloque sólido - gris oscuro
                    default: color_tile = al_map_rgb(255, 255, 255); break;
                }
                
                al_draw_rectangle(x, y, x + TILE_ANCHO, y + TILE_ALTO, color_tile, 1);
                
                // Mostrar vida del tile si es aplicable
                if (tilemap[fila][col].tipo == 2 && tilemap[fila][col].vida > 0)\
                {
                    sprintf(vida_texto, "%d", tilemap[fila][col].vida);
                    al_draw_text(fuente, al_map_rgb(255, 255, 255), x + TILE_ANCHO/2, y + TILE_ALTO/2, ALLEGRO_ALIGN_CENTER, vida_texto);
                    al_draw_text(fuente, al_map_rgb(150, 255, 150), x + TILE_ANCHO/2, y + TILE_ALTO/2 - 8, ALLEGRO_ALIGN_CENTER, "T");
                } 
                else if (tilemap[fila][col].tipo == 1 || tilemap[fila][col].tipo == 3) 
                {
                    al_draw_text(fuente, al_map_rgb(255, 255, 255), x + TILE_ANCHO/2, y + TILE_ALTO/2, ALLEGRO_ALIGN_CENTER, "S");
                }
            }
        }
    }
    
    al_draw_text(fuente, al_map_rgb(255, 255, 255), leyenda_x, leyenda_y, ALLEGRO_ALIGN_LEFT, "DEBUG HITBOXES:");
    al_draw_text(fuente, al_map_rgb(0, 255, 0), leyenda_x, leyenda_y + 15, ALLEGRO_ALIGN_LEFT, "Verde: Nave");
    al_draw_text(fuente, al_map_rgb(255, 0, 0), leyenda_x, leyenda_y + 30, ALLEGRO_ALIGN_LEFT, "Rojo: Enemigos");
    al_draw_text(fuente, al_map_rgb(0, 0, 255), leyenda_x, leyenda_y + 45, ALLEGRO_ALIGN_LEFT, "Azul: Disparos nave");
    al_draw_text(fuente, al_map_rgb(255, 255, 0), leyenda_x, leyenda_y + 60, ALLEGRO_ALIGN_LEFT, "Amarillo: Disparos enemigos");
    al_draw_text(fuente, al_map_rgb(255, 255, 255), leyenda_x, leyenda_y + 75, ALLEGRO_ALIGN_LEFT, "Blanco: Asteroides");
    al_draw_text(fuente, al_map_rgb(0, 128, 255), leyenda_x, leyenda_y + 90, ALLEGRO_ALIGN_LEFT, "Azul claro: Escudos");
}


/**
 * @brief Limpia recursos y memoria del juego.
 */
void limpiar_memoria_juego(Disparo disparos[], int num_disparos, Disparo disparos_enemigos[], int num_disparos_enemigos, Powerup powerups[], int max_powerups, Enemigo enemigos[], int num_enemigos, ColaMensajes* cola_mensajes)
{
    int i;

    // Limpiar disparos
    for (i = 0; i < num_disparos; i++) {
        disparos[i].activo = false;
    }
    
    for (i = 0; i < num_disparos_enemigos; i++) {
        disparos_enemigos[i].activo = false;
    }
    
    // Limpiar powerups
    for (i = 0; i < max_powerups; i++) {
        powerups[i].activo = false;
    }
    
    // Limpiar enemigos
    for (i = 0; i < num_enemigos; i++) {
        enemigos[i].activo = false;
    }
    
    // Limpiar cola de mensajes
    if (cola_mensajes)
    {
        cola_mensajes->cantidad = 0;
        cola_mensajes->inicio = 0;
        cola_mensajes->fin = 0;
        cola_mensajes->mensaje_actual.activo = false;
        cola_mensajes->procesando = false;
    }
    
    printf("Memoria del juego limpiada.\n");
}


/**
 * @brief Verifica si la nave está atravesando un escudo.
 */
bool nave_atravesando_escudo(float x, float y, float ancho, float largo, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS])
{
    int fila;
    int col;

    int col_izquierda = (int)(x / TILE_ANCHO);
    int col_derecha = (int)((x + ancho - 1) / TILE_ANCHO);
    int fila_superior = (int)(y / TILE_ALTO);
    int fila_inferior = (int)((y + largo - 1) / TILE_ALTO);

    for (fila = fila_superior; fila <= fila_inferior; fila++)
    {
        for (col = col_izquierda; col <= col_derecha; col++)
        {
            if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS)
            {
                if (tilemap[fila][col].tipo == 2 && tilemap[fila][col].vida > 0)
                {
                    return true;
                }
            }
        }
    }
    
    return false;
}


void crear_powerup_vida(Powerup powerups[], int max_powerups, float x, float y)
{
    int i;

    for (i = 0; i < max_powerups; i++)
    {
        if (!powerups[i].activo)
        {
            powerups[i].x = x;
            powerups[i].y = y;
            powerups[i].tipo = 1; // Tipo de powerup de vida
            powerups[i].activo = true;
            powerups[i].tiempo_aparicion = al_get_time();
            powerups[i].duracion_vida = 20.0;
            powerups[i].color = al_map_rgb(255, 0, 0); // Rojo para vida
            printf("Powerup de vida creado en (%.0f, %.0f)\n", x, y);
            break;

            printf("POWERUP DE VIDA CREADO EXITOSAMENTE:\n");
            printf("   - Slot: %d\n", i);
            printf("   - Posición: (%.1f, %.1f)\n", x, y);
            printf("   - Tipo: VIDA\n");
            printf("   - Color: ROJO\n");
            printf("   - Duración: %.1f segundos\n", powerups[i].duracion_vida);
            
            return;
        }
    }

    printf("ERROR: No hay slots disponibles para powerup de vida\n");
}


void crear_powerup_aleatorio(Powerup powerups[], int max_powerups, float x, float y)
{
    int probabilidad = rand() % 100;
    
    if (probabilidad < POWERUP_ESCUDO_PROB)
    {
        crear_powerup_escudo(powerups, max_powerups, x, y);
        printf("Powerup aleatorio: ESCUDO (probabilidad: %d%%)\n", probabilidad);
    } 
    else if (probabilidad < POWERUP_ESCUDO_PROB + POWERUP_VIDA_PROB)
    {
        crear_powerup_vida(powerups, max_powerups, x, y);
        printf("Powerup aleatorio: VIDA (probabilidad: %d%%)\n", probabilidad);
    }
    else if (probabilidad < POWERUP_ESCUDO_PROB + POWERUP_VIDA_PROB + POWERUP_LASER_PROB)
    {
        crear_powerup_laser(powerups, max_powerups, x, y);
        printf("Powerup aleatorio: LÁSER\n");
    }
    else if (probabilidad < POWERUP_ESCUDO_PROB + POWERUP_VIDA_PROB + POWERUP_LASER_PROB + 30)
    {
        crear_powerup_explosivo(powerups, max_powerups, x, y);
        printf("Powerup aleatorio: EXPLOSIVO\n");
    }
    else if (probabilidad < POWERUP_ESCUDO_PROB + POWERUP_VIDA_PROB + POWERUP_LASER_PROB + POWERUP_EXPLOSIVO_PROB + POWERUP_MISIL_PROB)
    {
        crear_powerup_misil(powerups, max_powerups, x, y);
        printf("Powerup aleatorio: MISIL\n");
    }
    else
    {
        // Por ahora, crear escudo como fallback
        crear_powerup_escudo(powerups, max_powerups, x, y);
        printf("Powerup aleatorio: ESCUDO (fallback, probabilidad: %d%%)\n", probabilidad);
    }
}


float obtener_radio_nave(Nave nave)
{
    return (nave.ancho * 0.7f) / 2.0f;
}


void obtener_centro_nave(Nave nave, float* centro_x, float* centro_y)
{
    *centro_x = nave.x + nave.ancho / 2;
    *centro_y = nave.y + nave.largo / 2;
}


void init_sistema_armas(Nave *nave)
{
    nave->armas[0] = (SistemaArma){Arma_normal, 1, 0, 0, true, 0.0, "Cañon normal", "Disparo en linea recta"};

    nave->armas[1] = (SistemaArma){Arma_laser, 1, 0, 10, false, 0.0, "Láser Continuo", "Rayo de energía sostenido"};
    nave->armas[2] = (SistemaArma){Arma_explosiva, 1, 0, 15, false, 0.0, "Cañón Explosivo", "Proyectiles con daño en área"};
    nave->armas[3] = (SistemaArma){Arma_misil, 1, 0, 20, false, 0.0, "Misiles Teledirigidos", "Misiles que persiguen enemigos"};

    nave->arma_actual = Arma_normal;
    nave->arma_seleccionada = 0;

    printf("Sistema de armas inicializado\n");
}


void cambiar_arma(Nave *nave, TipoArma nueva_arma)
{
    if (nueva_arma < 0 || nueva_arma > 3)
    {
        return;
    }
    
    if (nave->armas[nueva_arma].desbloqueado)
    {
        nave->arma_actual = nueva_arma;
        nave->arma_seleccionada = nueva_arma;
        printf("Arma cambiada a: %s\n", nave->armas[nueva_arma].nombre);
    }
    else
    {
        printf("Arma %s no desbloqueada aún.\n", nave->armas[nueva_arma].nombre);   
    }
}


/**
 * @brief Actualiza el progreso de mejora de un arma específica.
 * 
 * Incrementa el contador de kills para la mejora del arma especificada
 * y muestra el progreso actual.
 * 
 * @param nave Puntero a la nave del jugador.
 * @param tipo_arma Tipo de arma a actualizar (Arma_laser, Arma_explosiva, etc.).
 */
void actualizar_progreso_arma(Nave *nave, TipoArma tipo_arma)
{
    if (tipo_arma < 0 || tipo_arma > 3)
    {
        return;
    }
    
    SistemaArma *arma = &nave->armas[tipo_arma];
    arma->kills_mejora++;

    printf("Progreso arma %s: %d/%d kills\n", arma->nombre, arma->kills_mejora, arma->kills_necesarias);
}


/**
 * @brief Verifica si un arma puede ser mejorada y aplica la mejora.
 * 
 * Comprueba si el arma ha alcanzado el número de kills necesarios para
 * subir de nivel, aplica la mejora y muestra mensajes informativos.
 * 
 * @param nave Puntero a la nave del jugador.
 * @param tipo_arma Tipo de arma a verificar para mejora.
 * @param cola_mensajes Cola de mensajes para mostrar notificaciones.
 */
void verificar_mejora_arma(Nave *nave, TipoArma tipo_arma, ColaMensajes *cola_mensajes)
{
    char mensaje_principal[100];
    char mensaje_secundario[100];
    ALLEGRO_COLOR color_principal;
    int kills_para_siguiente;
    
    if (tipo_arma < 0 || tipo_arma > 3) return;
    
    SistemaArma *arma = &nave->armas[tipo_arma];
    
    // Verificar si puede subir de nivel
    if (arma->kills_mejora >= arma->kills_necesarias && arma->nivel < 3)
    {
        arma->nivel++;
        arma->kills_mejora = 0;
        
        kills_para_siguiente = (arma->nivel == 3) ? 0 : arma->kills_necesarias * arma->nivel;
        arma->kills_necesarias = kills_para_siguiente;
        
        // Mensajes específicos por arma
        switch (tipo_arma)
        {
            case Arma_laser:
                sprintf(mensaje_principal, "¡Láser mejorado a Nivel %d!", arma->nivel);
                sprintf(mensaje_secundario, "Mayor potencia y alcance");
                color_principal = al_map_rgb(0, 255, 255);
                break;
                
            case Arma_explosiva:
                sprintf(mensaje_principal, "¡Explosivos mejorados a Nivel %d!", arma->nivel);
                sprintf(mensaje_secundario, "Mayor radio de explosión");
                color_principal = al_map_rgb(255, 128, 0);
                break;
                
            case Arma_misil:
                sprintf(mensaje_principal, "¡Misiles mejorados a Nivel %d!", arma->nivel);
                sprintf(mensaje_secundario, "Mejor seguimiento y daño");
                color_principal = al_map_rgb(255, 0, 255);
                break;
                
            default:
                sprintf(mensaje_principal, "¡Arma mejorada a Nivel %d!", arma->nivel);
                sprintf(mensaje_secundario, "Mejor rendimiento");
                color_principal = al_map_rgb(255, 255, 0);
                break;
        }
        
        if (cola_mensajes)
        {
            agregar_mensaje_cola(cola_mensajes, mensaje_principal, 3.0, color_principal, true);
            agregar_mensaje_cola(cola_mensajes, mensaje_secundario, 2.5, al_map_rgb(255, 255, 255), true);
        }
        
        printf("Arma %s subió a nivel %d\n", arma->nombre, arma->nivel);
    }
}


/**
 * @brief Dibuja la información de las armas en pantalla con HUD mejorado.
 * 
 * Muestra el arma actual, su nivel, progreso de mejora y lista de armas
 * desbloqueadas con sus respectivas teclas de activación.
 * 
 * @param nave Nave del jugador con información de armas.
 * @param fuente Fuente de texto para renderizar la información.
 */
void dibujar_info_armas(Nave nave, ALLEGRO_FONT *fuente)
{
    // POSICIÓN BASE PARA TODO EL HUD DE ARMAS
    int hud_x = 580;  // Movido más a la izquierda para evitar solapamiento
    int hud_y = 450;  // Ajustado para dar más espacio

    // CUADRO DE ARMA ACTIVA - ARRIBA DE TODO
    int cuadro_x = hud_x;
    int cuadro_y = hud_y;
    int cuadro_ancho = 200;  // Aumentado
    int cuadro_alto = 60;    // Aumentado para dar más espacio

    char nombre_corto[15];
    char nivel_texto[10];

    float progreso;

    int barra_x;
    int barra_y;
    int barra_ancho;
    int barra_alto;

    char prog_texto[20];

    int slot_x;
    int slots_y;
    int slot_size;
    int slot_spacing;

    int i;

    SistemaArma arma;
    ALLEGRO_COLOR color_fondo, color_borde, color_texto;

    int centro_x;
    int centro_y;
    int punto_x;
    int punto_y;

    char tecla[3];
    
    SistemaArma arma_actual = nave.armas[nave.arma_seleccionada];

    int nivel;
    
    al_draw_filled_rectangle(cuadro_x, cuadro_y, cuadro_x + cuadro_ancho, cuadro_y + cuadro_alto, al_map_rgba(0, 0, 0, 150));
    al_draw_rectangle(cuadro_x, cuadro_y, cuadro_x + cuadro_ancho, cuadro_y + cuadro_alto, al_map_rgb(255, 215, 0), 2);
    
    // ✅ NOMBRE CORTO DEL ARMA ACTIVA - EN LA PARTE SUPERIOR
    switch (nave.arma_seleccionada)
    {
        case 0: strcpy(nombre_corto, "NORMAL"); break;
        case 1: strcpy(nombre_corto, "LÁSER"); break;
        case 2: strcpy(nombre_corto, "EXPLOSIVO"); break;
        case 3: strcpy(nombre_corto, "MISIL"); break;
        default: strcpy(nombre_corto, "DESCONOCIDO"); break;
    }
    
    al_draw_text(fuente, al_map_rgb(255, 255, 255), cuadro_x + 10, cuadro_y + 8, 
                ALLEGRO_ALIGN_LEFT, nombre_corto);
    
    // ✅ NIVEL DEL ARMA ACTIVA - AL LADO DEL NOMBRE
    sprintf(nivel_texto, "LV%d", arma_actual.nivel);
    al_draw_text(fuente, al_map_rgb(255, 215, 0), cuadro_x + 110, cuadro_y + 8, 
                ALLEGRO_ALIGN_LEFT, nivel_texto);
    
    // ✅ BARRA DE PROGRESO DEL ARMA ACTIVA - DEBAJO DEL NOMBRE (CÓDIGO COMPLETO IMPLEMENTADO)
    if (arma_actual.nivel < 3 && arma_actual.kills_necesarias > 0)
    {
        progreso = (float)arma_actual.kills_mejora / arma_actual.kills_necesarias;
        if (progreso > 1.0f) progreso = 1.0f;
        
        // Barra de progreso en el medio del cuadro
        barra_x = cuadro_x + 10;
        barra_y = cuadro_y + 28;  // Debajo del nombre
        barra_ancho = 180;
        barra_alto = 8;
        
        // Fondo de la barra
        al_draw_filled_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                               al_map_rgba(60, 60, 60, 180));
        
        // Progreso de la barra
        al_draw_filled_rectangle(barra_x, barra_y, barra_x + (barra_ancho * progreso), barra_y + barra_alto, 
                               al_map_rgb(0, 255, 100));
        
        // Borde de la barra
        al_draw_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                        al_map_rgb(150, 150, 150), 1);
        
        // Texto de progreso debajo de la barra
        sprintf(prog_texto, "%d/%d kills", arma_actual.kills_mejora, arma_actual.kills_necesarias);
        al_draw_text(fuente, al_map_rgb(200, 200, 200), cuadro_x + 10, barra_y + 12, 
                    ALLEGRO_ALIGN_LEFT, prog_texto);
    }
    else if (arma_actual.nivel == 3)
    {
        // Barra completa para nivel máximo
        barra_x = cuadro_x + 10;
        barra_y = cuadro_y + 28;
        barra_ancho = 180;
        barra_alto = 8;
        
        al_draw_filled_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                               al_map_rgb(255, 215, 0));
        al_draw_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                        al_map_rgb(255, 255, 255), 1);
        
        al_draw_text(fuente, al_map_rgb(255, 215, 0), cuadro_x + 10, barra_y + 12, 
                    ALLEGRO_ALIGN_LEFT, "NIVEL MÁXIMO");
    }
    
    // ✅ SLOTS DE ARMAS - DEBAJO DEL CUADRO PRINCIPAL
    slots_y = cuadro_y + cuadro_alto + 15; // 15 píxeles de separación
    slot_size = 45;  // Aumentado
    slot_spacing = 8; // Aumentado
    
    for (i = 0; i < 4; i++)
    {
        slot_x = hud_x + (i * (slot_size + slot_spacing));
        
        arma = nave.armas[i];
        
        // ✅ COLORES SEGÚN ESTADO DEL ARMA
        if (!arma.desbloqueado)
        {
            // Arma bloqueada - gris oscuro
            color_fondo = al_map_rgba(50, 50, 50, 150);
            color_borde = al_map_rgb(80, 80, 80);
            color_texto = al_map_rgb(100, 100, 100);
        }
        else if (nave.arma_seleccionada == i)
        {
            // Arma activa - dorado brillante
            color_fondo = al_map_rgba(255, 215, 0, 100);
            color_borde = al_map_rgb(255, 255, 255);
            color_texto = al_map_rgb(255, 255, 255);
        }
        else
        {
            // Arma disponible - azul suave
            color_fondo = al_map_rgba(0, 100, 200, 80);
            color_borde = al_map_rgb(150, 150, 255);
            color_texto = al_map_rgb(200, 200, 255);
        }
        
        // ✅ DIBUJAR SLOT
        al_draw_filled_rectangle(slot_x, slots_y, slot_x + slot_size, slots_y + slot_size, color_fondo);
        al_draw_rectangle(slot_x, slots_y, slot_x + slot_size, slots_y + slot_size, color_borde, 2);
        
        // ✅ NÚMERO DE TECLA - EN LA ESQUINA SUPERIOR IZQUIERDA
        sprintf(tecla, "%d", i + 1);
        al_draw_text(fuente, color_texto, slot_x + 5, slots_y + 3, ALLEGRO_ALIGN_LEFT, tecla);
        
        // ✅ ICONO VISUAL DEL ARMA - EN EL CENTRO (CÓDIGO COMPLETO IMPLEMENTADO)
        if (arma.desbloqueado)
        {
            centro_x = slot_x + slot_size/2;
            centro_y = slots_y + slot_size/2;
            
            switch (i)
            {
                case 0: // Normal - punto simple
                    al_draw_filled_circle(centro_x, centro_y, 4, color_texto);
                    break;
                    
                case 1: // Láser - línea horizontal
                    al_draw_line(centro_x - 10, centro_y, centro_x + 10, centro_y, color_texto, 4);
                    break;
                    
                case 2: // Explosivo - cruz
                    al_draw_line(centro_x - 8, centro_y, centro_x + 8, centro_y, color_texto, 3);
                    al_draw_line(centro_x, centro_y - 8, centro_x, centro_y + 8, color_texto, 3);
                    break;
                    
                case 3: // Misil - triángulo
                    al_draw_filled_triangle(centro_x, centro_y - 8, centro_x - 6, centro_y + 6, 
                                          centro_x + 6, centro_y + 6, color_texto);
                    break;
            }
        }
        
        // ✅ INDICADORES DE NIVEL - USANDO LAS FIGURAS EXISTENTES CORRECTAMENTE (CÓDIGO COMPLETO IMPLEMENTADO)
        if (arma.desbloqueado)
        {
            // Dibujar 3 puntos en la parte inferior para mostrar el nivel
            for (nivel = 1; nivel <= 3; nivel++)
            {
                punto_x = slot_x + 10 + ((nivel - 1) * 10);  // Distribuir en 3 columnas
                punto_y = slots_y + slot_size - 10;           // En la parte inferior
                
                if (nivel <= arma.nivel) 
                {
                    // Punto lleno para nivel alcanzado
                    al_draw_filled_circle(punto_x, punto_y, 3, al_map_rgb(0, 255, 0));
                    al_draw_circle(punto_x, punto_y, 3, al_map_rgb(255, 255, 255), 1);
                } 
                else 
                {
                    // Punto vacío para nivel no alcanzado
                    al_draw_circle(punto_x, punto_y, 3, al_map_rgb(100, 100, 100), 1);
                }
            }
        }
        else
        {
            // Para armas bloqueadas, mostrar puntos grises
            for (nivel = 1; nivel <= 3; nivel++)
            {
                punto_x = slot_x + 10 + ((nivel - 1) * 10);
                punto_y = slots_y + slot_size - 10;
                al_draw_circle(punto_x, punto_y, 3, al_map_rgb(60, 60, 60), 1);
            }
        }
    }
}


/**
 * @brief Dispara un láser continuo desde la nave.
 * 
 * Activa un láser que permanece activo mientras se mantenga presionada
 * la tecla, con propiedades que mejoran según el nivel del arma.
 * 
 * @param lasers Arreglo de láseres disponibles.
 * @param max_lasers Número máximo de láseres simultáneos.
 * @param nave Nave que dispara el láser.
 */
void disparar_laser(DisparoLaser lasers[], int max_lasers, Nave nave)
{
    double tiempo_actual = al_get_time();
    SistemaArma arma_laser = nave.armas[Arma_laser];
    int i;
    float centro_x;
    float centro_y;
    float punta_x;
    float punta_y;
    
    for (i = 0; i < max_lasers; i++)
    {
        if (!lasers[i].activo)
        {
            centro_x = nave.x + nave.ancho / 2.0f;
            centro_y = nave.y + nave.largo / 2.0f;
            punta_x = centro_x + cos(nave.angulo - ALLEGRO_PI / 2) * (nave.largo / 2.0f);
            punta_y = centro_y + sin(nave.angulo - ALLEGRO_PI / 2) * (nave.largo / 2.0f);

            lasers[i].x_nave = punta_x;
            lasers[i].y_nave = punta_y;
            lasers[i].x = punta_x;
            lasers[i].y = punta_y;
            lasers[i].ancho = 6 + (arma_laser.nivel * 2);
            lasers[i].alto = nave.y;
            lasers[i].angulo = nave.angulo - ALLEGRO_PI / 2;
            lasers[i].alcance = 600;
            lasers[i].activo = true;
            lasers[i].tiempo_inicio = tiempo_actual;
            lasers[i].ultimo_dano = 0.0;
            
            lasers[i].duracion_max = 0;
            lasers[i].poder = 1.0f + arma_laser.nivel;

            switch(arma_laser.nivel)
            {
                case 1:
                    lasers[i].dano_por_segundo = 15.0f;
                    lasers[i].color = al_map_rgba(255, 0, 0, 150); 
                    break;
                case 2:
                    lasers[i].dano_por_segundo = 25.0f;
                    lasers[i].color = al_map_rgba(255, 100, 0, 180); 
                    break;
                case 3:
                    lasers[i].dano_por_segundo = 40.0f; 
                    lasers[i].color = al_map_rgba(255, 255, 0, 220); 
                    break;
                default:
                    lasers[i].dano_por_segundo = 10.0f;
                    lasers[i].color = al_map_rgba(255, 0, 0, 150); 
                    break;
            }

            nave.armas[Arma_laser].ultimo_uso = tiempo_actual;
            printf("Láser disparado - Nivel %d, Poder %d, Duración %.1fs\n", arma_laser.nivel, lasers[i].poder, lasers[i].duracion_max);
            return;
        }   
    }

    printf("No hay espacio para más láseres activos.\n");
}


/**
 * @brief Actualiza todos los láseres activos.
 * 
 * Actualiza la posición, verifica colisiones con tilemap y enemigos,
 * aplica daño según intervalos y maneja el alcance limitado por obstáculos.
 * 
 * @param lasers Arreglo de láseres a actualizar.
 * @param max_lasers Número máximo de láseres.
 * @param enemigos Arreglo de enemigos para detectar colisiones.
 * @param num_enemigos Número de enemigos.
 * @param puntaje Puntero al puntaje del jugador.
 * @param nave Nave que dispara (para posicionamiento).
 * @param tilemap Mapa de tiles para detectar obstáculos.
 * @param contador_debug Contador para mensajes de debug.
 */
void actualizar_lasers(DisparoLaser lasers[], int max_lasers, Enemigo enemigos[], int num_enemigos, int *puntaje, Nave *nave, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], int *contador_debug, Powerup powerups[], int max_powerups, ColaMensajes *cola_mensajes)
{
    double tiempo_actual = al_get_time();
    int i;
    int j;
    int k;
    float centro_x;
    float centro_y;
    int lasers_activos;
    float alcance_real;
    float dano_aplicado;
    //double var_tiempo;
    float punta_x;
    float punta_y;
    //int prob_powerup;

    obtener_centro_nave(*nave, &centro_x, &centro_y);

    punta_x = centro_x + cos(nave->angulo - ALLEGRO_PI / 2) * (nave->largo / 2.0f);
    punta_y = centro_y + sin(nave->angulo - ALLEGRO_PI / 2) * (nave->largo / 2.0f);

    if (++(*contador_debug) % 300 == 0)
    {
        lasers_activos = 0;
        for (k = 0; k < max_lasers; k++)
        {
            if (lasers[k].activo) lasers_activos++;
        }
        printf("DEBUG: %d láseres activos\n", lasers_activos);
    }

    for (i = 0; i < max_lasers; i++)
    {
        if (!lasers[i].activo) continue;

        lasers[i].x_nave = punta_x;
        lasers[i].y_nave = punta_y;
        lasers[i].angulo = nave->angulo - ALLEGRO_PI/2;

        alcance_real = verificar_colision_laser_tilemap(lasers[i], tilemap);
        
        for (j = 0; j < num_enemigos; j++)
        {
            if (!enemigos[j].activo) continue;
            
            if (laser_intersecta_enemigo_limitado(lasers[i], enemigos[j], alcance_real))
            {
                if (tiempo_actual - lasers[i].ultimo_dano >= 0.1)
                {
                    dano_aplicado = lasers[i].poder;
                    enemigos[j].vida -= dano_aplicado;
                    *puntaje += 5;
                    lasers[i].ultimo_dano = tiempo_actual;

                    if (enemigos[j].vida <= 0)
                    {
                        actualizar_progreso_arma(nave, Arma_laser);
                        
                        if ((rand() % 100) < POWERUP_PROB)
                        {
                            crear_powerup_aleatorio(powerups, max_powerups, enemigos[j].x, enemigos[j].y);
                        }
                        enemigos[j].activo = false;
                        *puntaje += 10;
                        
                        verificar_mejora_arma(nave, Arma_laser, cola_mensajes);
                    }
                }
            }
        }
    }
}


/**
 * @brief Dibuja todos los láseres activos con efectos visuales.
 * 
 * Renderiza los láseres con alcance limitado por obstáculos, efectos
 * de destello en el origen y chispas en puntos de impacto.
 * 
 * @param lasers Arreglo de láseres a dibujar.
 * @param max_lasers Número máximo de láseres.
 * @param tilemap Mapa de tiles para calcular alcance real.
 */
void dibujar_lasers(DisparoLaser lasers[], int max_lasers, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS])
{
    int i;
    int p;
    float final_x;
    float final_y;
    float offset_x;
    float offset_y;
    ALLEGRO_COLOR color_centro;
    float alcance_real;

    for (i = 0; i < max_lasers; i++)
    {
        if (lasers[i].activo)
        {
            alcance_real = verificar_colision_laser_tilemap(lasers[i], tilemap);
            final_x = lasers[i].x_nave + cos(lasers[i].angulo) * alcance_real;
            final_y = lasers[i].y_nave + sin(lasers[i].angulo) * alcance_real;

            // Linea del laser con un alcance limitado
            al_draw_line(lasers[i].x_nave, lasers[i].y_nave, final_x, final_y, lasers[i].color, lasers[i].ancho);

            // Linea mas delgada en el centro del laser
            color_centro = al_map_rgba(255, 255, 255, 200);
            al_draw_line(lasers[i].x_nave, lasers[i].y_nave, final_x, final_y, color_centro, lasers[i].ancho / 3);
            
            // Efecto de destello en el origen del laser
            al_draw_filled_circle(lasers[i].x_nave, lasers[i].y_nave, lasers[i].ancho/2 + 3, al_map_rgba(255, 255, 255, 100));

            if (alcance_real < lasers[i].alcance)
            {
                for (p = 0; p < 5; p++)
                {
                    offset_x = (rand() % 20 - 10) * 0.5f;
                    offset_y = (rand() % 20 - 10) * 0.5f;
                    al_draw_filled_circle(final_x + offset_x, final_y + offset_y, 1, al_map_rgba(255, 100, 0, 150));
                }
            }
        }
    }
}


/**
 * @brief Crea un powerup de láser en la posición especificada.
 * 
 * Genera un powerup que desbloquea el arma láser cuando es recogido
 * por el jugador.
 * 
 * @param powerups Arreglo de powerups disponibles.
 * @param max_powerups Número máximo de powerups.
 * @param x Posición x donde crear el powerup.
 * @param y Posición y donde crear el powerup.
 */
void crear_powerup_laser(Powerup powerups[], int max_powerups, float x, float y)
{
    int i;

    for (i = 0; i < max_powerups; i++)
    {
        if (!powerups[i].activo)
        {
            powerups[i].x = x;
            powerups[i].y = y;
            powerups[i].tipo = 2;
            powerups[i].activo = true;
            powerups[i].tiempo_aparicion = al_get_time();
            powerups[i].duracion_vida = 20.0;
            powerups[i].color = al_map_rgb(255, 0, 0);

            printf("POWERUP DE LÁSER CREADO en (%.1f, %.1f)\n", x, y);
            return;
        }
    }
}


/**
 * @brief Dispara según el arma actual seleccionada.
 */
void disparar_segun_arma(Nave nave, Disparo disparos[], int num_disparos, DisparoLaser lasers[], int max_lasers, DisparoExplosivo explosivos[], int max_explosivos, MisilTeledirigido misiles[], int max_misiles, Enemigo enemigos[], int num_enemigos)
{
    switch (nave.arma_actual)
    {
        case Arma_normal:
            if (nave.nivel_disparo_radial > 0)
            {
                disparar_radial(disparos, num_disparos, nave);
            }
            else
            {
                disparar(disparos, num_disparos, nave);
            }
            break;
        case Arma_laser:
            disparar_laser(lasers, max_lasers, nave);
            break;
        case Arma_explosiva:
            disparar_explosivo(explosivos, max_explosivos, nave);
            break;
        case Arma_misil:
            disparar_misil(misiles, max_misiles, nave, enemigos, num_enemigos);
            break;
        default:
            disparar(disparos, num_disparos, nave);
            break;
    }
}


/**
 * @brief Crea un powerup de explosivos en la posición especificada.
 * 
 * Genera un powerup que desbloquea el arma explosiva cuando es recogido
 * por el jugador.
 * 
 * @param powerups Arreglo de powerups disponibles.
 * @param max_powerups Número máximo de powerups.
 * @param x Posición x donde crear el powerup.
 * @param y Posición y donde crear el powerup.
 */
void crear_powerup_explosivo(Powerup powerups[], int max_powerups, float x, float y)
{
    int i;

    for (i = 0; i < max_powerups; i++)
    {
        if (!powerups[i].activo)
        {
            powerups[i].x = x;
            powerups[i].y = y;
            powerups[i].tipo = 3; // Tipo de powerup explosivo
            powerups[i].activo = true;
            powerups[i].tiempo_aparicion = al_get_time();
            powerups[i].duracion_vida = 20.0;
            powerups[i].color = al_map_rgb(255, 165, 0); // Naranja para explosivo

            printf("POWERUP EXPLOSIVO CREADO en (%.1f, %.1f)\n", x, y);
            return;
        }
    }
}


/**
 * @brief Crea un powerup de misiles en la posición especificada.
 * 
 * Genera un powerup que desbloquea el arma de misiles teledirigidos
 * cuando es recogido por el jugador.
 * 
 * @param powerups Arreglo de powerups disponibles.
 * @param max_powerups Número máximo de powerups.
 * @param x Posición x donde crear el powerup.
 * @param y Posición y donde crear el powerup.
 */
void crear_powerup_misil(Powerup powerups[], int max_powerups, float x, float y)
{
    int i;

    for (i = 0; i < max_powerups; i++)
    {
        if (!powerups[i].activo)
        {
            powerups[i].x = x;
            powerups[i].y = y;
            powerups[i].tipo = 4; // Tipo misil
            powerups[i].activo = true;
            powerups[i].tiempo_aparicion = al_get_time();
            powerups[i].duracion_vida = 20.0;
            powerups[i].color = al_map_rgb(0, 255, 100); // Verde
            
            printf("POWERUP MISIL CREADO en (%.1f, %.1f)\n", x, y);
            return;
        }
    }
}


/**
 * @brief Dispara un proyectil explosivo desde la nave.
 * 
 * Crea un proyectil que explota al impactar con enemigos o obstáculos,
 * causando daño en área con propiedades mejoradas según el nivel del arma.
 * 
 * @param explosivos Arreglo de proyectiles explosivos.
 * @param max_explosivos Número máximo de explosivos simultáneos.
 * @param nave Nave que dispara el explosivo.
 */
void disparar_explosivo(DisparoExplosivo explosivos[], int max_explosivos, Nave nave)
{
    double tiempo_actual = al_get_time();
    SistemaArma arma_explosiva = nave.armas[Arma_explosiva];
    int i;
    float centro_x, centro_y;
    float punta_x, punta_y;
    float velocidad;

    
    // Cooldown entre disparos
    if (tiempo_actual - arma_explosiva.ultimo_uso < 0.5) return;
    
    for (i = 0; i < max_explosivos; i++)
    {
        if (!explosivos[i].activo)
        {
            obtener_centro_nave(nave, &centro_x, &centro_y);
            
            // Calcular posición de disparo desde la punta de la nave
            punta_x = centro_x + cos(nave.angulo - ALLEGRO_PI / 2) * (nave.largo / 2.0f);
            punta_y = centro_y + sin(nave.angulo - ALLEGRO_PI / 2) * (nave.largo / 2.0f);
            
            // ✅ INICIALIZAR COMPLETAMENTE EL EXPLOSIVO
            explosivos[i].x = punta_x;
            explosivos[i].y = punta_y;
            explosivos[i].ancho = 8;
            explosivos[i].alto = 12;
            explosivos[i].activo = true;
            explosivos[i].exploto = false;
            explosivos[i].dano_aplicado = false;
            
            // ✅ VELOCIDAD IGUAL A DISPARO NORMAL (450 píxeles por segundo)
            velocidad = 450.0f; // Misma velocidad que disparo normal
            explosivos[i].vx = cos(nave.angulo - ALLEGRO_PI / 2) * velocidad;
            explosivos[i].vy = sin(nave.angulo - ALLEGRO_PI / 2) * velocidad;
            
            // Propiedades según nivel del arma
            explosivos[i].radio_explosion = 50 + (arma_explosiva.nivel * 20); // Radio aumenta con nivel
            explosivos[i].dano_directo = 30 + (arma_explosiva.nivel * 15);    // Daño aumenta con nivel
            explosivos[i].dano_area = 20 + (arma_explosiva.nivel * 10);       // Daño de área aumenta
            explosivos[i].tiempo_vida = tiempo_actual + 3.0; // 3 segundos de vida máxima
            
            printf("Explosivo disparado: velocidad=450, radio=%d, daño_directo=%d, daño_área=%d\n", 
                   explosivos[i].radio_explosion, explosivos[i].dano_directo, explosivos[i].dano_area);
            
            // ✅ ACTUALIZAR TIEMPO DE ÚLTIMO USO
            nave.armas[Arma_explosiva].ultimo_uso = tiempo_actual;
            return;
        }
    }

    printf("No hay espacio para más explosivos\n");
}


/**
 * @brief Actualiza todos los proyectiles explosivos activos.
 * 
 * Maneja el movimiento, detección de colisiones, explosiones y daño en área
 * de todos los proyectiles explosivos activos.
 * 
 * @param explosivos Arreglo de explosivos a actualizar.
 * @param max_explosivos Número máximo de explosivos.
 * @param enemigos Arreglo de enemigos para detectar colisiones y daño.
 * @param num_enemigos Número de enemigos.
 * @param puntaje Puntero al puntaje del jugador.
 * @param tilemap Mapa de tiles para detectar colisiones con obstáculos.
 */
void actualizar_explosivos(DisparoExplosivo explosivos[], int max_explosivos, Enemigo enemigos[], int num_enemigos, int* puntaje, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Nave *nave, ColaMensajes *cola_mensajes)
{
    double tiempo_actual = al_get_time();
    int i;
    int j;
    int col_izq, col_der, fila_sup, fila_inf;
    int fila, col;
    float distancia;
    double tiempo_explosion;
    float factor_distancia;
    int dano_final;
    float tile_centro_x;
    float tile_centro_y;
    int dano_bloque;
    
    for (i = 0; i < max_explosivos; i++)
    {
        if (!explosivos[i].activo) continue;
        
        // ✅ SI NO HA EXPLOTADO AÚN, MOVER EL PROYECTIL
        if (!explosivos[i].exploto)
        {
            // Mover el proyectil
            explosivos[i].x += explosivos[i].vx * (1.0f / 60.0f); // Asumiendo 60 FPS
            explosivos[i].y += explosivos[i].vy * (1.0f / 60.0f);
            
            // ✅ VERIFICAR COLISIÓN CON ENEMIGOS
            for (j = 0; j < num_enemigos; j++)
            {
                if (enemigos[j].activo && detectar_colision_generica(
                    explosivos[i].x, explosivos[i].y, explosivos[i].ancho, explosivos[i].alto,
                    enemigos[j].x, enemigos[j].y, enemigos[j].ancho, enemigos[j].alto))
                {
                    printf("Explosivo impactó enemigo tipo %d\n", enemigos[j].tipo);
                    
                    // ✅ ACTIVAR EXPLOSIÓN
                    explosivos[i].exploto = true;
                    explosivos[i].tiempo_vida = tiempo_actual; // Marcar tiempo de explosión
                    explosivos[i].dano_aplicado = false; // Resetear para aplicar daño de área
                    goto colision_detectada;
                }
            }
            
            // VERIFICAR COLISIÓN CON BLOQUES DEL TILEMAP
            col_izq = (int)(explosivos[i].x / TILE_ANCHO);
            col_der = (int)((explosivos[i].x + explosivos[i].ancho - 1) / TILE_ANCHO);
            fila_sup = (int)(explosivos[i].y / TILE_ALTO);
            fila_inf = (int)((explosivos[i].y + explosivos[i].alto - 1) / TILE_ALTO);
            
            for (fila = fila_sup; fila <= fila_inf; fila++)
            {
                for (col = col_izq; col <= col_der; col++)
                {
                    if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS)
                    {
                        if (tilemap[fila][col].tipo == 1) // BLOQUE SÓLIDO (NO DESTRUCTIBLE)
                        {
                            printf("Explosivo impactó bloque SÓLIDO en (%d,%d) - Solo explota, no lo destruye\n", col, fila);
                            
                            // ACTIVAR EXPLOSIÓN PERO NO DAÑAR EL BLOQUE
                            explosivos[i].exploto = true;
                            explosivos[i].tiempo_vida = tiempo_actual;
                            explosivos[i].dano_aplicado = false;
                            goto colision_detectada;
                        }
                        else if (tilemap[fila][col].tipo == 2) // ESCUDO DESTRUCTIBLE
                        {
                            printf("Explosivo impactó escudo DESTRUCTIBLE en (%d,%d) - Explota y lo daña\n", col, fila);
                            
                            // ACTIVAR EXPLOSIÓN Y DAÑAR EL BLOQUE DESTRUCTIBLE
                            explosivos[i].exploto = true;
                            explosivos[i].tiempo_vida = tiempo_actual;
                            explosivos[i].dano_aplicado = false;
                            
                            // DAÑAR SOLO EL BLOQUE IMPACTADO DIRECTAMENTE
                            tilemap[fila][col].vida -= explosivos[i].dano_directo;
                            if (tilemap[fila][col].vida <= 0)
                            {
                                tilemap[fila][col].tipo = 0; // Destruir escudo
                                printf("Escudo destruido por impacto directo de explosivo\n");
                            }
                            goto colision_detectada;
                        }
                    }
                }
            }
            
            colision_detectada:
            
            // ✅ VERIFICAR SI SALIÓ DE LA PANTALLA
            if (explosivos[i].x < -20 || explosivos[i].x > 820 || 
                explosivos[i].y < -20 || explosivos[i].y > 620)
            {
                explosivos[i].activo = false;
                printf("Explosivo salió de pantalla\n");
                continue;
            }
            
            // ✅ VERIFICAR TIEMPO DE VIDA MÁXIMO
            if (tiempo_actual > explosivos[i].tiempo_vida)
            {
                printf("Explosivo explotó por tiempo límite\n");
                explosivos[i].exploto = true;
                explosivos[i].tiempo_vida = tiempo_actual;
                explosivos[i].dano_aplicado = false;
            }
        }
        else // ✅ SI YA EXPLOTÓ, MANEJAR LA ANIMACIÓN
        {
            tiempo_explosion = tiempo_actual - explosivos[i].tiempo_vida;
            
            // ✅ APLICAR DAÑO DE ÁREA UNA SOLA VEZ
            if (!explosivos[i].dano_aplicado)
            {
                printf("Aplicando daño de explosión en área\n");
                
                // Dañar enemigos en el radio de explosión
                for (j = 0; j < num_enemigos; j++)
                {
                    if (enemigos[j].activo)
                    {
                        distancia = sqrt((enemigos[j].x + enemigos[j].ancho/2 - explosivos[i].x) * (enemigos[j].x + enemigos[j].ancho/2 - explosivos[i].x) + (enemigos[j].y + enemigos[j].alto/2 - explosivos[i].y) * (enemigos[j].y + enemigos[j].alto/2 - explosivos[i].y));
                        
                        if (distancia <= explosivos[i].radio_explosion)
                        {
                            // ✅ VERIFICAR LÍNEA DE VISTA PARA EXPLOSIONES
                            if (verificar_linea_vista_explosion(explosivos[i].x, explosivos[i].y, enemigos[j].x + enemigos[j].ancho/2, enemigos[j].y + enemigos[j].alto/2, tilemap))
                            {
                                factor_distancia = 1.0f - (distancia / explosivos[i].radio_explosion);
                                dano_final = (int)(explosivos[i].dano_area * factor_distancia);
                                
                                enemigos[j].vida -= dano_final;
                                printf("Enemigo tipo %d recibió %d de daño por explosión (distancia: %.1f)\n", enemigos[j].tipo, dano_final, distancia);
                                
                                if (enemigos[j].vida <= 0)
                                {
                                    enemigos[j].activo = false;
                                    (*puntaje) += 15;
                                    
                                    // VERIFICAR MEJORA DEL ARMA EXPLOSIVA
                                    actualizar_progreso_arma(nave, Arma_explosiva);
                                    verificar_mejora_arma(nave, Arma_explosiva, cola_mensajes);
                                    
                                    printf("Enemigo eliminado por explosión\n");
                                }
                            }
                        }
                    }
                }
                
                // DAÑAR SOLO BLOQUES DESTRUCTIBLES EN EL RADIO DE EXPLOSIÓN
                for (fila = 0; fila < MAPA_FILAS; fila++)
                {
                    for (col = 0; col < MAPA_COLUMNAS; col++)
                    {
                        if (tilemap[fila][col].tipo == 2) // SOLO ESCUDOS DESTRUCTIBLES
                        {
                            tile_centro_x = col * TILE_ANCHO + TILE_ANCHO/2;
                            tile_centro_y = fila * TILE_ALTO + TILE_ALTO/2;
                            
                            distancia = sqrt((tile_centro_x - explosivos[i].x) * (tile_centro_x - explosivos[i].x) + (tile_centro_y - explosivos[i].y) * (tile_centro_y - explosivos[i].y));
                            
                            if (distancia <= explosivos[i].radio_explosion * 0.7f) // Radio menor para bloques
                            {
                                factor_distancia = 1.0f - (distancia / (explosivos[i].radio_explosion * 0.7f));
                                dano_bloque = (int)(explosivos[i].dano_area * factor_distancia * 0.5f);
                                
                                tilemap[fila][col].vida -= dano_bloque;
                                if (tilemap[fila][col].vida <= 0)
                                {
                                    tilemap[fila][col].tipo = 0; // Destruir bloque destructible
                                    printf("Escudo destructible en (%d,%d) destruido por explosión en área\n", col, fila);
                                }
                            }
                        }
                        // ✅ LOS BLOQUES SÓLIDOS (tipo 1) NO RECIBEN DAÑO DE ÁREA
                    }
                }
                
                explosivos[i].dano_aplicado = true;
            }
            
            // ✅ DESACTIVAR DESPUÉS DE LA ANIMACIÓN (0.5 segundos)
            if (tiempo_explosion > 0.5)
            {
                explosivos[i].activo = false;
                printf("Animación de explosión terminada\n");
            }
        }
    }
}


/**
 * @brief Dibuja todos los proyectiles explosivos y sus explosiones.
 * 
 * Renderiza los proyectiles en vuelo y las explosiones con efectos
 * visuales progresivos de expansión y desvanecimiento.
 * 
 * @param explosivos Arreglo de explosivos a dibujar.
 * @param max_explosivos Número máximo de explosivos.
 */
void dibujar_explosivos(DisparoExplosivo explosivos[], int max_explosivos)
{
    double tiempo_actual = al_get_time();
    int i;
    ALLEGRO_COLOR color_proyectil;
    double tiempo_explosion;
    float progreso;
    float radio_actual;
    float alpha;
    int num_circulos;
    int c;
    float factor_radio;
    float radio_circulo;
    float alpha_circulo;
    int r, g, b, a;
    ALLEGRO_COLOR color_explosion;
    int num_particulas;
    int p;
    float angulo;
    float distancia_particula;
    float px;
    float py;
    int size;
    ALLEGRO_COLOR color_particula;
    float alpha_destello;

    for (i = 0; i < max_explosivos; i++)
    {
        if (!explosivos[i].activo) continue;
        
        // ✅ SI NO HA EXPLOTADO, DIBUJAR PROYECTIL
        if (!explosivos[i].exploto)
        {
            color_proyectil = al_map_rgb(255, 100, 0); // Naranja
            
            // Dibujar proyectil con efecto de estela
            al_draw_filled_circle(explosivos[i].x + explosivos[i].ancho/2, explosivos[i].y + explosivos[i].alto/2, 4, color_proyectil);
            
            // Estela del proyectil
            al_draw_filled_circle(explosivos[i].x + explosivos[i].ancho/2 - explosivos[i].vx * 0.01f, explosivos[i].y + explosivos[i].alto/2 - explosivos[i].vy * 0.01f, 2, al_map_rgba(255, 50, 0, 128));
        }
        else // ✅ SI HA EXPLOTADO, DIBUJAR ANIMACIÓN DE EXPLOSIÓN
        {
            tiempo_explosion = tiempo_actual - explosivos[i].tiempo_vida;
            progreso = (float)(tiempo_explosion / 0.5); // 0.5 segundos de duración
            
            if (progreso > 1.0f) progreso = 1.0f;
            
            // ✅ ANIMACIÓN DE EXPANSIÓN Y DESVANECIMIENTO
            radio_actual = explosivos[i].radio_explosion * progreso;
            alpha = 1.0f - progreso; // Se desvanece con el tiempo
            
            // ✅ MÚLTIPLES CÍRCULOS PARA EFECTO REALISTA
            num_circulos = 5;
            for (c = 0; c < num_circulos; c++)
            {
                factor_radio = (c + 1) / (float)num_circulos;
                radio_circulo = radio_actual * factor_radio;
                alpha_circulo = alpha * (1.0f - factor_radio * 0.5f);
                
                // Colores que van del amarillo al rojo
                r = (int)(255 * alpha_circulo);
                g = (int)((255 - c * 30) * alpha_circulo);
                b = 0;
                a = (int)(255 * alpha_circulo);
                
                if (r < 0) r = 0;
                if (r > 255) r = 255;
                if (g < 0) g = 0;
                if (g > 255) g = 255;
                if (a < 0) a = 0;
                if (a > 255) a = 255;
                
                color_explosion = al_map_rgba(r, g, b, a);
                
                if (c == num_circulos - 1) // Círculo exterior
                {
                    al_draw_filled_circle(explosivos[i].x, explosivos[i].y, radio_circulo, color_explosion);
                }
                else // Círculos interiores
                {
                    al_draw_filled_circle(explosivos[i].x, explosivos[i].y, radio_circulo, color_explosion);
                }
            }
            
            // ✅ PARTÍCULAS DE EXPLOSIÓN
            num_particulas = 8 + (int)(progreso * 12); // Más partículas conforme avanza
            
            for (p = 0; p < num_particulas; p++)
            {
                angulo = (2.0f * ALLEGRO_PI * p) / num_particulas;
                distancia_particula = radio_actual * (0.8f + (rand() % 40) / 100.0f);
                
                px = explosivos[i].x + cos(angulo) * distancia_particula;
                py = explosivos[i].y + sin(angulo) * distancia_particula;
                
                size = 2 + rand() % 3;
                color_particula = al_map_rgba(255, 150 - (int)(progreso * 100), 0, (int)(alpha * 200));
                
                al_draw_filled_circle(px, py, size, color_particula);
            }
            
            // ✅ EFECTO DE DESTELLO EN EL CENTRO
            if (progreso < 0.3f) // Solo durante los primeros momentos
            {
                alpha_destello = (0.3f - progreso) / 0.3f;
                ALLEGRO_COLOR color_destello = al_map_rgba(255, 255, 255, (int)(alpha_destello * 255));
                
                al_draw_filled_circle(explosivos[i].x, explosivos[i].y, 8, color_destello);
            }
        }
    }
}


/**
 * @brief Dispara un misil teledirigido que busca automáticamente enemigos.
 * 
 * Crea un misil que puede cambiar de trayectoria para perseguir al enemigo
 * más cercano, con propiedades mejoradas según el nivel del arma.
 * 
 * @param misiles Arreglo de misiles teledirigidos.
 * @param max_misiles Número máximo de misiles simultáneos.
 * @param nave Nave que dispara el misil.
 * @param enemigos Arreglo de enemigos para seleccionar objetivo.
 * @param num_enemigos Número de enemigos disponibles.
 */
void disparar_misil(MisilTeledirigido misiles[], int max_misiles, Nave nave, Enemigo enemigos[], int num_enemigos)
{
    double tiempo_actual = al_get_time();
    SistemaArma arma_misil = nave.armas[Arma_misil];
    int i, j;
    //float dx, dy;
    //float distancia;
    float centro_x;
    float centro_y;
    float punta_x;
    float punta_y;

    // Buscar enemigo más cercano
    int enemigo_objetivo = -1;
    float distancia_minima = 1000000;
    
    // Cooldown entre disparos
    if (tiempo_actual - arma_misil.ultimo_uso < 1.0) return;
    
    for (j = 0; j < num_enemigos; j++)
    {
        if (enemigos[j].activo)
        {
            float dx = enemigos[j].x - nave.x;
            float dy = enemigos[j].y - nave.y;
            float distancia = sqrt(dx*dx + dy*dy);
            
            if (distancia < distancia_minima)
            {
                distancia_minima = distancia;
                enemigo_objetivo = j;
            }
        }
    }

    for (i = 0; i < max_misiles; i++)
    {
        if (!misiles[i].activo)
        {
            // Calcular posición de disparo
            centro_x = nave.x + nave.ancho / 2.0f;
            centro_y = nave.y + nave.largo / 2.0f;
            punta_x = centro_x + cos(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);
            punta_y = centro_y + sin(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);
            
            misiles[i].x = punta_x;
            misiles[i].y = punta_y;
            misiles[i].vx = cos(nave.angulo - ALLEGRO_PI/2) * 3.0f;
            misiles[i].vy = sin(nave.angulo - ALLEGRO_PI/2) * 3.0f;
            misiles[i].ancho = 6;
            misiles[i].alto = 10;
            misiles[i].activo = true;
            misiles[i].tiempo_vida = 0;
            misiles[i].enemigo_objetivo = enemigo_objetivo;
            misiles[i].tiene_objetivo = (enemigo_objetivo != -1);
            
            // Propiedades según nivel
            misiles[i].vel_max = 4.0f + (arma_misil.nivel * 1.0f);
            misiles[i].fuerza_giro = 0.1f + (arma_misil.nivel * 0.05f);
            misiles[i].dano = 4 + arma_misil.nivel;

            nave.armas[Arma_misil].ultimo_uso = tiempo_actual;

            printf("Misil disparado - Nivel %d, Objetivo: %d, Vel: %.1f\n", arma_misil.nivel, enemigo_objetivo, misiles[i].vel_max);
            break;
        }
    }
}


/**
 * @brief Actualiza todos los misiles teledirigidos activos.
 * 
 * Maneja el seguimiento de objetivos, cambio de trayectoria, búsqueda
 * de nuevos objetivos y detección de colisiones.
 * 
 * @param misiles Arreglo de misiles a actualizar.
 * @param max_misiles Número máximo de misiles.
 * @param enemigos Arreglo de enemigos para seguimiento.
 * @param num_enemigos Número de enemigos.
 * @param puntaje Puntero al puntaje del jugador.
 */
void actualizar_misiles(MisilTeledirigido misiles[], int max_misiles, Enemigo enemigos[], int num_enemigos, int* puntaje)
{
    int i;
    int j;
    float dx, dy;
    float distancia;
    float dir_x, dir_y;
    float vel_actual;
    float distancia_minima;


    for (i = 0; i < max_misiles; i++)
    {
        if (misiles[i].activo)
        {
            misiles[i].tiempo_vida += 0.016;

            // Si tiene objetivo válido, dirigirse hacia él
            if (misiles[i].tiene_objetivo && misiles[i].enemigo_objetivo != -1 && misiles[i].enemigo_objetivo < num_enemigos && enemigos[misiles[i].enemigo_objetivo].activo)
            {
                Enemigo *objetivo = &enemigos[misiles[i].enemigo_objetivo];
                
                // Calcular dirección hacia el objetivo
                dx = (objetivo->x + objetivo->ancho/2) - misiles[i].x;
                dy = (objetivo->y + objetivo->alto/2) - misiles[i].y;
                distancia = sqrt(dx*dx + dy*dy);
                
                if (distancia > 5.0f)
                {
                    // Normalizar y aplicar seguimiento
                    dir_x = dx / distancia;
                    dir_y = dy / distancia;
                    
                    // Interpolar suavemente hacia la nueva dirección
                    misiles[i].vx += dir_x * misiles[i].fuerza_giro;
                    misiles[i].vy += dir_y * misiles[i].fuerza_giro;
                    
                    // Limitar velocidad máxima
                    vel_actual = sqrt(misiles[i].vx*misiles[i].vx + misiles[i].vy*misiles[i].vy);
                    if (vel_actual > misiles[i].vel_max)
                    {
                        misiles[i].vx = (misiles[i].vx / vel_actual) * misiles[i].vel_max;
                        misiles[i].vy = (misiles[i].vy / vel_actual) * misiles[i].vel_max;
                    }
                }
            }
            else
            {
                // Sin objetivo válido, buscar nuevo objetivo
                misiles[i].tiene_objetivo = false;
                distancia_minima = 300; // Rango de búsqueda
                
                for (j = 0; j < num_enemigos; j++)
                {
                    if (enemigos[j].activo)
                    {
                        dx = enemigos[j].x - misiles[i].x;
                        dy = enemigos[j].y - misiles[i].y;
                        distancia = sqrt(dx*dx + dy*dy);
                        
                        if (distancia < distancia_minima)
                        {
                            distancia_minima = distancia;
                            misiles[i].enemigo_objetivo = j;
                            misiles[i].tiene_objetivo = true;
                        }
                    }
                }
            }
            
            // Mover misil
            misiles[i].x += misiles[i].vx;
            misiles[i].y += misiles[i].vy;
            
            // Verificar colisiones con enemigos
            for (j = 0; j < num_enemigos; j++)
            {
                if (enemigos[j].activo)
                {
                    if (detectar_colision_generica(misiles[i].x, misiles[i].y, misiles[i].ancho, misiles[i].alto, enemigos[j].x, enemigos[j].y, enemigos[j].ancho, enemigos[j].alto))
                    {
                        // Impacto
                        enemigos[j].vida -= misiles[i].dano;
                        printf("Misil impactó enemigo %d: -%d HP\n", j, misiles[i].dano);
                        
                        if (enemigos[j].vida <= 0)
                        {
                            enemigos[j].activo = false;
                            (*puntaje)++;
                            printf("Enemigo eliminado por misil\n");
                        }
                        
                        misiles[i].activo = false;
                        break;
                    }
                }
            }
            
            // Desactivar si sale de pantalla o tiempo excedido
            if (misiles[i].x < -10 || misiles[i].x > 810 || misiles[i].y < -10 || misiles[i].y > 610 || 
                misiles[i].tiempo_vida > 8.0)
            {
                misiles[i].activo = false;
            }
        }
    }
}


/**
 * @brief Dibuja todos los misiles teledirigidos con efectos visuales.
 * 
 * Renderiza los misiles con orientación dinámica según su velocidad,
 * incluyendo efectos de estela y punta direccional.
 * 
 * @param misiles Arreglo de misiles a dibujar.
 * @param max_misiles Número máximo de misiles.
 */
void dibujar_misiles(MisilTeledirigido misiles[], int max_misiles)
{
    int i;
    float angulo;
    float punta_x, punta_y;
    float cola_x, cola_y;

    for (i = 0; i < max_misiles; i++)
    {
        if (misiles[i].activo)
        {
            // Calcular ángulo de rotación del misil
            angulo = atan2(misiles[i].vy, misiles[i].vx);
            
            // Dibujar cuerpo del misil
            al_draw_filled_rectangle(misiles[i].x - misiles[i].ancho/2, misiles[i].y - misiles[i].alto/2, misiles[i].x + misiles[i].ancho/2, misiles[i].y + misiles[i].alto/2, al_map_rgb(0, 200, 100));

            // Dibujar punta del misil
            punta_x = misiles[i].x + cos(angulo) * (misiles[i].alto/2 + 3);
            punta_y = misiles[i].y + sin(angulo) * (misiles[i].alto/2 + 3);
            al_draw_filled_circle(punta_x, punta_y, 2, al_map_rgb(255, 255, 0));
            
            // Efecto de estela
            cola_x = misiles[i].x - cos(angulo) * (misiles[i].alto/2 + 5);
            cola_y = misiles[i].y - sin(angulo) * (misiles[i].alto/2 + 5);
            al_draw_filled_circle(cola_x, cola_y, 3, al_map_rgba(255, 100, 0, 150));
        }
    }
}


/**
 * @brief Verifica si un punto está sobre una línea con tolerancia.
 * 
 * Calcula si un punto específico está dentro de una distancia tolerable
 * de una línea definida por dos puntos.
 * 
 * @param x1 Coordenada x del primer punto de la línea.
 * @param y1 Coordenada y del primer punto de la línea.
 * @param x2 Coordenada x del segundo punto de la línea.
 * @param y2 Coordenada y del segundo punto de la línea.
 * @param px Coordenada x del punto a verificar.
 * @param py Coordenada y del punto a verificar.
 * @param tolerancia Distancia máxima permitida del punto a la línea.
 * @return true si el punto está sobre la línea dentro de la tolerancia.
 * @return false en caso contrario.
 */
bool punto_en_linea_laser(float x1, float y1, float x2, float y2, float px, float py, float tolerancia)
{
    float A = y2 - y1;
    float B = x1 - x2;
    float C = x2 * y1 - x1 * y2;
    float punto_1;
    float punto_2;

    float distancia = fabs(A * px + B * py + C) / sqrt(A * A + B * B);
    
    if (distancia > tolerancia)
    {
        return false;
    }
    
    punto_1 = (px - x1) * (x2 - x1) + (py - y1) * (y2 - y1);
    punto_2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);

    return (punto_1 >= 0 && punto_1 <= punto_2);
}


/**
 * @brief Verifica la interseccion del laser con un enemigo
 * 
 * @param laser 
 * @param enemigo 
 * @return true 
 * @return false 
 */
bool laser_intersecta_enemigo(DisparoLaser laser, Enemigo enemigo)
{
    float final_x = laser.x_nave + cos(laser.angulo) * laser.alcance;
    float final_y = laser.y_nave + sin(laser.angulo) * laser.alcance;
    float margen = 5.0f;
    float enemigo_x1 = enemigo.x - margen;
    float enemigo_y1 = enemigo.y - margen;
    float enemigo_x2 = enemigo.x + enemigo.ancho + margen;
    float enemigo_y2 = enemigo.y + enemigo.alto + margen;

    return linea_intersecta_rectangulo(laser.x_nave, laser.y_nave, final_x, final_y, enemigo_x1, enemigo_y1, enemigo_x2, enemigo_y2);
}


/**
 * @brief Verifica si una línea intersecta con un rectángulo.
 */
bool linea_intersecta_rectangulo(float x1, float y1, float x2, float y2, float rect_x1, float rect_y1, float rect_x2, float rect_y2)
{
    // Verificar si algún punto de la línea está dentro del rectángulo
    if ((x1 >= rect_x1 && x1 <= rect_x2 && y1 >= rect_y1 && y1 <= rect_y2) || (x2 >= rect_x1 && x2 <= rect_x2 && y2 >= rect_y1 && y2 <= rect_y2))
    {
        return true;
    }
    
    // Verificar intersección con cada lado del rectángulo
    return (linea_intersecta_linea(x1, y1, x2, y2, rect_x1, rect_y1, rect_x2, rect_y1) || linea_intersecta_linea(x1, y1, x2, y2, rect_x2, rect_y1, rect_x2, rect_y2) || linea_intersecta_linea(x1, y1, x2, y2, rect_x2, rect_y2, rect_x1, rect_y2) || linea_intersecta_linea(x1, y1, x2, y2, rect_x1, rect_y2, rect_x1, rect_y1));
}


/**
 * @brief Verifica si dos líneas se intersectan.
 */
bool linea_intersecta_linea(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    float t, u;
    if (fabs(denom) < 0.0001f)
    {
        return false; // Líneas paralelas
    }
    
    t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
    u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;
    
    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}


float verificar_colision_laser_tilemap(DisparoLaser laser, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS])
{
    float paso;
    float distancia_actual;
    float x_actual;
    float y_actual;
    int fila;
    int col;

    paso = laser.alcance > 400 ? 25.0f : laser.alcance > 200 ? 15.0f : 10.0f;
    distancia_actual = 0.0f;

    while (distancia_actual < laser.alcance)
    {
        x_actual = laser.x_nave + cos(laser.angulo) * distancia_actual;
        y_actual = laser.y_nave + sin(laser.angulo) * distancia_actual;
        
        col = (int)(x_actual / TILE_ANCHO);
        fila = (int)(y_actual / TILE_ALTO);

        if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS)
        {
            if (tilemap[fila][col].tipo == 1 || tilemap[fila][col].tipo == 3)
            {
                printf("Laser bloqueado por un tile solido\n");
                return distancia_actual;
            }
        }

        distancia_actual += paso;
    }

    return laser.alcance;
}


/**
 * @brief Verifica si el láser intersecta con un enemigo dentro del alcance limitado.
 */
bool laser_intersecta_enemigo_limitado(DisparoLaser laser, Enemigo enemigo, float alcance_real)
{
    float final_x = laser.x_nave + cos(laser.angulo) * alcance_real;
    float final_y = laser.y_nave + sin(laser.angulo) * alcance_real;
    
    float margen = 5.0f;
    float enemigo_x1 = enemigo.x - margen;
    float enemigo_y1 = enemigo.y - margen;
    float enemigo_x2 = enemigo.x + enemigo.ancho + margen;
    float enemigo_y2 = enemigo.y + enemigo.alto + margen;

    return linea_intersecta_rectangulo(laser.x_nave, laser.y_nave, final_x, final_y, enemigo_x1, enemigo_y1, enemigo_x2, enemigo_y2);
}


/**
 * @brief Verifica si hay línea de vista libre para explosiones.
 * 
 * Comprueba si existe una línea directa entre dos puntos sin obstáculos
 * sólidos en el tilemap, usado para determinar si una explosión puede
 * afectar a un objetivo.
 * 
 * @param x1 Coordenada x del punto origen.
 * @param y1 Coordenada y del punto origen.
 * @param x2 Coordenada x del punto destino.
 * @param y2 Coordenada y del punto destino.
 * @param tilemap Mapa de tiles para verificar obstáculos.
 * @return true si hay línea de vista libre.
 * @return false si hay obstáculos bloqueando.
 */
bool verificar_linea_vista_explosion(float x1, float y1, float x2, float y2, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS])
{    
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distancia = sqrt(dx * dx + dy * dy);
    float paso;
    float pasos;
    int i;
    float t;
    float x_check;
    float y_check;
    int col;
    int fila;
    
    if (distancia == 0)
    {
        return true;
    }
    
    paso = 5.0f; // Verificar cada 5 píxeles
    pasos = (int)(distancia / paso);
    
    for (i = 1; i < pasos; i++)
    {
        t = (float)i / pasos;
        x_check = x1 + dx * t;
        y_check = y1 + dy * t;
        
        col = (int)(x_check / TILE_ANCHO);
        fila = (int)(y_check / TILE_ALTO);
        
        if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS)
        {
            // Si hay un tile sólido, bloquea la explosión
            if (tilemap[fila][col].tipo == 3 || tilemap[fila][col].tipo == 1)
            {
                return false;
            }
        }
    }
    
    return true; // Línea de vista libre
}


/**
 * @brief Carga las imágenes específicas para cada tipo de enemigo.
 * 
 * @param imagenes_enemigos Array de punteros a las imágenes de cada tipo.
 * @return bool true si se cargaron correctamente, false en caso contrario.
 */
bool cargar_imagenes_enemigos(ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS])
{
    const char *rutas_imagenes[] = {
        "imagenes/enemigos/Enemigo1.png",     // Tipo 0: Normal
        "imagenes/enemigos/Enemigo2.png",     // Tipo 1: Perseguidor
        "imagenes/enemigos/Enemigo3.png",     // Tipo 2: Francotirador
        "imagenes/enemigos/Enemigo4.png",     // Tipo 3: Tanque
        "imagenes/enemigos/Enemigo5.png"      // Tipo 4: Kamikaze
    };

    int i;
    ALLEGRO_COLOR colores[] = {
                al_map_rgb(255, 100, 100),   // Tipo 0: Rojo claro
                al_map_rgb(255, 165, 0),     // Tipo 1: Naranja
                al_map_rgb(128, 0, 128),     // Tipo 2: Púrpura
                al_map_rgb(139, 69, 19),     // Tipo 3: Marrón
                al_map_rgb(255, 255, 0)      // Tipo 4: Amarillo
            };

    for (i = 0; i < NUM_TIPOS_ENEMIGOS; i++)
    {
        imagenes_enemigos[i] = al_load_bitmap(rutas_imagenes[i]);

        if (!imagenes_enemigos[i])
        {
            printf("No se pudo cargar %s, creando sprite placeholder\n", rutas_imagenes[i]);

            // Crear sprite de color como fallback
            imagenes_enemigos[i] = al_create_bitmap(40, 40);

            if (!imagenes_enemigos[i]) 
            {
                printf("ERROR: No se pudo crear bitmap placeholder\n");
                return false;
            }

            al_set_target_bitmap(imagenes_enemigos[i]);

            al_clear_to_color(colores[i]);
            
            // Agregar símbolo distintivo por tipo
            switch(i) {
                case 0: // Normal - círculo simple
                    al_draw_filled_circle(20, 20, 15, al_map_rgb(150, 50, 50));
                    break;
                case 1: // Perseguidor - triángulo
                    al_draw_filled_triangle(20, 5, 10, 35, 30, 35, al_map_rgb(200, 100, 0));
                    break;
                case 2: // Francotirador - línea larga
                    al_draw_filled_rectangle(18, 5, 22, 35, al_map_rgb(80, 0, 80));
                    break;
                case 3: // Tanque - rectángulo grueso
                    al_draw_filled_rectangle(5, 10, 35, 30, al_map_rgb(100, 50, 0));
                    break;
                case 4: // Kamikaze - estrella
                    al_draw_filled_circle(20, 20, 12, al_map_rgb(200, 200, 0));
                    al_draw_filled_circle(20, 20, 6, al_map_rgb(255, 255, 100));
                    break;
            }
            
            al_draw_rectangle(0, 0, 39, 39, al_map_rgb(0, 0, 0), 2);
            al_set_target_backbuffer(al_get_current_display());

            printf("Sprite placeholder creado para enemigo tipo %d\n", i);
        }
        else
        {
            printf("Imagen del enemigo tipo %d cargada: %s\n", i, rutas_imagenes[i]);
        }
    }

    return true;
}


/**
 * @brief Asigna la imagen correcta al enemigo según su tipo.
 * 
 * @param enemigo Puntero al enemigo.
 * @param imagenes_enemigos Array con las imágenes de cada tipo.
 */
void asignar_imagen_enemigo(Enemigo *enemigo, ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS])
{
    if (enemigo->tipo >= 0 && enemigo->tipo < NUM_TIPOS_ENEMIGOS)
    {
        if (!enemigo->imagen || enemigo->imagen != imagenes_enemigos[enemigo->tipo])
        {
            enemigo->imagen = imagenes_enemigos[enemigo->tipo];
            printf("Imagen asignada al enemigo tipo %d\n", enemigo->tipo);
        }
    }
    else
    {
        printf("ERROR: Tipo de enemigo inválido: %d\n", enemigo->tipo);
        enemigo->imagen = imagenes_enemigos[0]; // Usar imagen normal como fallback
    }
}


/**
 * @brief Libera la memoria de las imágenes de enemigos.
 * 
 * @param imagenes_enemigos Array con las imágenes de cada tipo.
 */
void liberar_imagenes_enemigos(ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS])
{
    int i;

    for (i = 0; i < NUM_TIPOS_ENEMIGOS; i++)
    {
        if (imagenes_enemigos[i])
        {
            al_destroy_bitmap(imagenes_enemigos[i]);
            imagenes_enemigos[i] = NULL;
        }
    }
    printf("Imágenes de enemigos liberadas correctamente\n");
}


/**
 * @brief Verifica si todas las imágenes de enemigos están cargadas correctamente.
 * 
 * @param imagenes_enemigos Array con las imágenes de cada tipo.
 * @return bool true si todas están cargadas, false en caso contrario.
 */
bool verificar_imagenes_enemigos(ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS])
{
    int i;

    for (i = 0; i < NUM_TIPOS_ENEMIGOS; i++)
    {
        if (!imagenes_enemigos[i])
        {
            printf("ERROR: Imagen del enemigo tipo %d no está cargada\n", i);
            return false;
        }
    }
    
    printf("Todas las imágenes de enemigos están cargadas correctamente\n");
    return true;
}


/**
 * @brief Inicializa un jefe según su tipo.
 * 
 * @param jefe Puntero al jefe a inicializar.
 * @param tipo Tipo de jefe (0: Destructor, 1: Supremo).
 * @param x Posición x inicial.
 * @param y Posición y inicial.
 * @param imagen Imagen del jefe.
 */
void init_jefe(Jefe *jefe, int tipo, float x, float y, ALLEGRO_BITMAP *imagen)
{
    int i;

    jefe->x = x;
    jefe->y = y;
    jefe->tipo = tipo;
    jefe->activo = true;
    jefe->imagen = imagen;

    for (i = 0; i < MAX_ATAQUES_JEFE; i++)
    {
        jefe->ataques[i].activo = false;
    }
    
    jefe->ultimo_ataque = 0.0;
    jefe->ultima_invocacion = 0.0;
    jefe->enemigos_invocados = 0;
    jefe->en_furia = false;
    jefe->tiempo_furia = 0.0;
    jefe->fase_ataque = 0;
    jefe->angulo_laser = 0.0;
    jefe->tiempo_animacion = 0.0;

    switch(tipo)
    {
        case 0:
            jefe->ancho = 120;
            jefe->alto = 80;
            jefe->vida = 150.0f;
            jefe->vida_max = 150.0f;
            jefe->intervalo_ataque = 2.5;
            jefe->max_enemigos_invocacion = 8;
            jefe->velocidad_movimiento = 1.0f;
            jefe->ataque_actual = Ataque_rafaga;
            break;

        case 1:
            jefe->ancho = 160;
            jefe->alto = 100;
            jefe->vida = 250.0f;
            jefe->vida_max = 250.0f;
            jefe->intervalo_ataque = 2.0;
            jefe->max_enemigos_invocacion = 12;
            jefe->velocidad_movimiento = 1.5f;
            jefe->ataque_actual = Ataque_laser_giratorio;
            break;
    }

        printf("Jefe tipo %d inicializado: Vida %.1f, Tamaño %.0fx%.0f\n", tipo, jefe->vida, jefe->ancho, jefe->alto);
}


/**
 * @brief Actualiza el comportamiento del jefe.
 * 
 * @param jefe Puntero al jefe a actualizar.
 * @param nave Nave del jugador (para ataques dirigidos).
 * @param enemigos Array de enemigos para invocaciones.
 * @param num_enemigos Puntero al número de enemigos activos.
 * @param imagenes_enemigos Imágenes de enemigos para invocaciones.
 * @param tiempo_actual Tiempo actual del juego.
 */
void actualizar_jefe(Jefe *jefe, Nave nave, Enemigo enemigos[], int *num_enemigos, ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS], double tiempo_actual)
{
    float velocidad;
    float centro_x;
    float centro_y;
    float radio;
    int i;
    float dx;
    float dy;
    float distancia;

    if (!jefe->activo)
    {
        return;
    }
    
    if (!jefe->en_furia && jefe->vida < (jefe->vida_max * 0.3f))
    {
        jefe->en_furia = true;
        jefe->tiempo_furia = tiempo_actual;
        jefe->velocidad_movimiento *= 0.6f; // Aumentar velocidad en furia
        printf("Jefe entra en FURIA! Velocidad aumentada a %.2f\n", jefe->velocidad_movimiento);
    }
    
    jefe->tiempo_animacion += 0.02;
    velocidad = jefe->en_furia ? jefe->velocidad_movimiento * 1.5f : jefe->velocidad_movimiento;

    if (jefe->tipo == 0)
    {
        jefe->x += sin(jefe->tiempo_animacion) * velocidad;
        if (jefe->x <= 0)
        {
            jefe->x = 0;
        }

        if (jefe->x >= 800 - jefe->ancho)
        {
            jefe->x = 800 - jefe->ancho;
        }
    }
    else
    {
        centro_x = 400;
        centro_y = 150;
        radio = 80;
        jefe->x = centro_x + cos(jefe->tiempo_animacion * 0.5f) * radio - jefe->ancho / 2;
        jefe->y = centro_y + sin(jefe->tiempo_animacion * 0.3f) * 40 - jefe->alto / 2;
    }

    if (tiempo_actual - jefe->ultimo_ataque >= jefe->intervalo_ataque)
    {
        jefe_atacar(jefe, nave, tiempo_actual);
        jefe->ultimo_ataque = tiempo_actual;
    }
    
    if (tiempo_actual - jefe->ultima_invocacion >= TIEMPO_INVOCACION_ENEMIGOS)
    {
        jefe_invocar_enemigos(jefe, enemigos, num_enemigos, imagenes_enemigos);
        jefe->ultima_invocacion = tiempo_actual;
    }
    
    for (i = 0; i < MAX_ATAQUES_JEFE; i++)
    {
        if (jefe->ataques[i].activo)
        {
            AtaqueJefe *ataque = &jefe->ataques[i];

            switch (ataque->tipo)
            {
                case Ataque_perseguidor:
                    dx = (nave.x + nave.ancho / 2) - ataque->x;
                    dy = (nave.y + nave.largo / 2) - ataque->y;
                    distancia = sqrt(dx * dx + dy * dy);

                    if (distancia > 0)
                    {
                        ataque->vx = (dx /distancia) * ataque->velocidad * 0.8f;
                        ataque->vy = (dy / distancia) * ataque->velocidad * 0.8f;
                    }
                
                    break;

                case Ataque_ondas:
                    ataque->velocidad += 0.2f;
                    break;
            
                default:
                    break;
            }

            ataque->x += ataque->vx;
            ataque->y += ataque->vy;
            ataque->angulo = 0.1f;

            if (ataque->x < -50 || ataque->x > 850 || ataque->y < -50 || ataque->y > 650 || (tiempo_actual - ataque->tiempo_vida) > 8.0)
            {
                ataque->activo = false;
            }
        }
    }
}


/**
 * @brief Ejecuta ataques del jefe según su tipo y fase.
 * 
 * @param jefe Puntero al jefe atacante.
 * @param nave Nave del jugador (para ataques dirigidos).
 * @param tiempo_actual Tiempo actual del juego.
 */
void jefe_atacar(Jefe *jefe, Nave nave, double tiempo_actual)
{
    int i;
    int j;
    int k;
    float angulo;
    float angulo_base;
    float nave_x;
    float nave_y;
    float dx;
    float dy;
    float distancia;
    float offset_x;
    float offset_y;

    if (jefe->tipo == 0)
    {
        switch (jefe->fase_ataque % 3)
        {
            case 0:
                for (i = 0; i < 8; i++)
                {
                    for (j = 0; j < MAX_ATAQUES_JEFE; j++)
                    {
                        if (!jefe->ataques[j].activo)
                        {
                            AtaqueJefe *ataque = &jefe->ataques[j];

                            angulo = (ALLEGRO_PI * 2 / 8) * i;
                            ataque->x = jefe->x + jefe->ancho / 2;
                            ataque->y = jefe->y + jefe->alto;
                            ataque->vx = cos(angulo) * 4.0f;
                            ataque->vy = sin(angulo) * 4.0f + 2.0f;
                            ataque->tipo = Ataque_rafaga;
                            ataque->activo = true;
                            ataque->tiempo_vida = tiempo_actual;
                            ataque->dano = 12;
                            ataque->velocidad = 4.0f;
                            ataque->color = al_map_rgb(255, 100, 100);
                            break;
                        }
                    }
                }
                break;

                case 1:
                    for (i = 0; i < 5; i++)
                    {
                        for (j = 0; j < MAX_ATAQUES_JEFE; j++)
                        {
                            if (!jefe->ataques[j].activo)
                            {
                                AtaqueJefe *ataque = &jefe->ataques[j];

                                ataque->x = jefe->x + (rand() % (int)jefe->ancho);
                                ataque->y = jefe->y + jefe->alto;
                                ataque->vx = (rand() % 200 - 100) / 50.0f; // -2 a 2
                                ataque->vy = 3.0f + (rand() % 100) / 100.0f; // 3 a 4
                                ataque->tipo = Ataque_lluvia;
                                ataque->activo = true;
                                ataque->tiempo_vida = tiempo_actual;
                                ataque->dano = 12;
                                ataque->color = al_map_rgb(100, 255, 100);
                                break;
                            }
                        }
                    }
                    break;
                
                case 2:
                    for (i = 0; i < 3; i++)
                    {
                        for (j = 0; j < MAX_ATAQUES_JEFE; j++)
                        {
                            if (!jefe->ataques[j].activo)
                            {
                                AtaqueJefe* ataque = &jefe->ataques[j];
                            
                                ataque->x = jefe->x + jefe->ancho / 2;
                                ataque->y = jefe->y + jefe->alto;
                                ataque->vx = 0;
                                ataque->vy = 1.0f;
                                ataque->tipo = Ataque_perseguidor;
                                ataque->activo = true;
                                ataque->tiempo_vida = tiempo_actual;
                                ataque->dano = 20;
                                ataque->velocidad = 3.0f;
                                ataque->color = al_map_rgb(255, 255, 100);
                                break;
                            }
                        }
                    }

                    break;
        }
    }
    else
    {
        switch (jefe->fase_ataque % 4)
        {
        case 0:
            for (k = 0; k < 3; k++)
            {
                for (j = 0; j < MAX_ATAQUES_JEFE; j++)
                {
                    if (!jefe->ataques[j].activo)
                    {
                        AtaqueJefe *ataque = &jefe->ataques[j];

                        angulo_base = jefe->angulo_laser + (k * ALLEGRO_PI * 2 / 3);
                        ataque->x = jefe->x + jefe->ancho / 2;
                        ataque->y = jefe->y + jefe->alto / 2;
                        ataque->vx = cos(angulo_base) * 5.0f;
                        ataque->vy = sin(angulo_base) * 5.0f;
                        ataque->tipo = Ataque_laser_giratorio;
                        ataque->activo = true;
                        ataque->tiempo_vida = tiempo_actual;
                        ataque->dano = 18;
                        ataque->color = al_map_rgb(100, 100, 255);
                        break;
                    }
                }
            }
            
            jefe->angulo_laser += 0.2f;
            break;
        
        case 1:
            for (i = 0; i < 12; i++)
            {
                for (j = 0; j < MAX_ATAQUES_JEFE; j++)
                {
                    if (!jefe->ataques[j].activo)
                    {
                        AtaqueJefe *ataque = &jefe->ataques[j];

                        angulo = (ALLEGRO_PI * 2 / 12) * i;
                        ataque->x = jefe->x + jefe->ancho / 2;
                        ataque->y = jefe->y + jefe->alto / 2;
                        ataque->vx = cos(angulo) * 2.0f;
                        ataque->vy = sin(angulo) * 2.0f;
                        ataque->tipo = Ataque_ondas;
                        ataque->activo = true;
                        ataque->tiempo_vida = tiempo_actual;
                        ataque->dano = 15;
                        ataque->velocidad = 2.0f;
                        ataque->color = al_map_rgb(255, 0, 255);
                        break;
                    }
                }
            }
            break;

        case 2:
            nave_x = nave.x + nave.ancho / 2;
            nave_y = nave.y + nave.largo / 2;

            for (i = 0; i < 6; i++)
            {
                for (j = 0; j < MAX_ATAQUES_JEFE; j++)
                {
                    if (!jefe->ataques[j].activo)
                    {
                        AtaqueJefe *ataque = &jefe->ataques[j];
                        dx = nave_x - (jefe->x + jefe->ancho / 2);
                        dy = nave_y - (jefe->y + jefe->alto / 2);
                        distancia = sqrt(dx * dx + dy * dy);

                        offset_x = (rand() % 100 - 50) / 10.0f;
                        offset_y = (rand() % 100 - 50) / 10.0f;

                        ataque->x = jefe->x + jefe->ancho / 2;
                        ataque->y = jefe->x + jefe->alto;

                        if (distancia > 0)
                        {
                            ataque->vx = ((dx / distancia) * 4.0f) + offset_x;
                            ataque->vy = ((dy / distancia) * 4.0f) + offset_y;
                        }
                        
                        ataque->tipo = Ataque_lluvia;
                        ataque->activo = true;
                        ataque->tiempo_vida = tiempo_actual;
                        ataque->dano = 16;
                        ataque->color = al_map_rgb(255, 150, 0);
                        break;
                    }
                }
            }
    
            break;

        case 3:
            for (i = 0; i < 5; i++)
            {
                for (j = 0; j < MAX_ATAQUES_JEFE; j++)
                {
                    if (!jefe->ataques[j].activo)
                    {
                        AtaqueJefe *ataque = &jefe->ataques[j];

                        ataque->x = jefe->x + (rand() % (int)jefe->ancho);
                        ataque->y = jefe->y + jefe->alto;
                        ataque->vx = 0;
                        ataque->vy = 2.0f;
                        ataque->tipo = Ataque_perseguidor;
                        ataque->activo = true;
                        ataque->tiempo_vida = tiempo_actual;
                        ataque->dano = 22;
                        ataque->velocidad = 3.5f;
                        ataque->color = al_map_rgb(255, 0, 100);
                        break;
                    }   
                }
            }
            
            break;
        }
    }

    jefe->fase_ataque++;

    if (jefe->en_furia)
    {
        printf("Jefe ataca en MODO FURIA - Fase %d\n", jefe->fase_ataque % (jefe->tipo == 0 ? 3 : 4));
    }   
}


/**
 * @brief Dibuja el jefe con efectos visuales.
 * 
 * @param jefe Jefe a dibujar.
 */
void dibujar_jefe(Jefe jefe)
{
    if (!jefe.activo) return;
    
    float intensidad;
    ALLEGRO_COLOR aura;
    ALLEGRO_COLOR color_centro;
    
    // Efecto de pulsación
    intensidad = 0.8f + 0.2f * sin(jefe.tiempo_animacion * 10);
    
    if (jefe.en_furia)
    {
        aura = al_map_rgba(255, 0, 0, (int)(intensidad * 100));
        color_centro = al_map_rgba(255, 100, 100, 200);
    }
    else
    {
        aura = al_map_rgba(150, 0, 150, (int)(intensidad * 80));
        color_centro = al_map_rgba(200, 100, 200, 180);
    }
    
    // Dibujar aura
    al_draw_filled_circle(jefe.x + jefe.ancho/2, jefe.y + jefe.alto/2, 
                         (jefe.ancho + jefe.alto)/3 + 10, aura);
    
    // Dibujar cuerpo principal
    if (jefe.imagen)
    {
        float escala_x = jefe.ancho / al_get_bitmap_width(jefe.imagen);
        float escala_y = jefe.alto / al_get_bitmap_height(jefe.imagen);
        
        al_draw_scaled_bitmap(jefe.imagen, 0, 0,
                             al_get_bitmap_width(jefe.imagen),
                             al_get_bitmap_height(jefe.imagen),
                             jefe.x, jefe.y, jefe.ancho, jefe.alto, 0);
    }
    else
    {
        al_draw_filled_rectangle(jefe.x, jefe.y, jefe.x + jefe.ancho, jefe.y + jefe.alto, color_centro);
    }
    
    // Borde
    al_draw_rectangle(jefe.x, jefe.y, jefe.x + jefe.ancho, jefe.y + jefe.alto, 
                      al_map_rgb(255, 255, 255), 3);
    
    // Barra de vida del jefe
    float vida_porcentaje = jefe.vida / jefe.vida_max;
    float barra_ancho = jefe.ancho;
    float barra_alto = 8;
    float barra_x = jefe.x;
    float barra_y = jefe.y - 15;
    
    ALLEGRO_COLOR color_vida = vida_porcentaje > 0.5f ? 
        al_map_rgb(255, 255, 0) : al_map_rgb(255, 0, 0);
    
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                            al_map_rgba(100, 0, 0, 150));
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + (barra_ancho * vida_porcentaje), 
                            barra_y + barra_alto, color_vida);
    al_draw_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                      al_map_rgb(255, 255, 255), 1);
}


/**
 * @brief Dibuja todos los ataques activos del jefe.
 * 
 * @param ataques Array de ataques del jefe.
 * @param max_ataques Número máximo de ataques.
 */
void dibujar_ataques_jefe(AtaqueJefe ataques[], int max_ataques)
{
    int i;

    for (i = 0; i < max_ataques; i++)
    {
        if (ataques[i].activo)
        {
            AtaqueJefe ataque = ataques[i];

            switch (ataque.tipo)
            {
                case Ataque_rafaga:
                    al_draw_filled_circle(ataque.x, ataque.y, 6, ataque.color);
                    al_draw_circle(ataque.x, ataque.y, 6, al_map_rgb(255, 255, 255), 1);
                    break;

                case Ataque_laser_giratorio:
                    // Proyectil tipo láser
                    al_draw_filled_circle(ataque.x, ataque.y, 8, ataque.color);
                    al_draw_filled_circle(ataque.x, ataque.y, 4, al_map_rgb(255, 255, 255));
                    // Estela
                    al_draw_line(ataque.x, ataque.y, 
                                ataque.x - ataque.vx * 2, ataque.y - ataque.vy * 2,
                                ataque.color, 3);
                    break;

                case Ataque_lluvia:
                    al_draw_filled_circle(ataque.x, ataque.y, 5, ataque.color);
                    // Pequeña estela
                    al_draw_line(ataque.x, ataque.y, 
                                ataque.x - ataque.vx, ataque.y - ataque.vy,
                                ataque.color, 2);
                    break;

                case Ataque_ondas:
                    // Anillo expansivo
                    al_draw_circle(ataque.x, ataque.y, 12, ataque.color, 4);
                    al_draw_circle(ataque.x, ataque.y, 8, ataque.color, 2);
                    break;

                case Ataque_perseguidor:
                    // Proyectil con forma de flecha
                    al_draw_filled_circle(ataque.x, ataque.y, 7, ataque.color);
                    al_draw_filled_triangle(ataque.x, ataque.y - 7,
                                           ataque.x - 5, ataque.y + 5,
                                           ataque.x + 5, ataque.y + 5,
                                           ataque.color);
                    al_draw_triangle(ataque.x, ataque.y - 7,
                                    ataque.x - 5, ataque.y + 5,
                                    ataque.x + 5, ataque.y + 5,
                                    al_map_rgb(255, 255, 255), 2);
                    break;
            }
        }
    }
}


/**
 * @brief Detecta colisión entre ataque de jefe y nave.
 * 
 * @param ataque Ataque del jefe.
 * @param nave Nave del jugador.
 * @return true si hay colisión.
 */
bool detectar_colision_ataque_jefe_nave(AtaqueJefe ataque, Nave nave)
{
    float radio_ataque = 8.0f;
    float centro_nave_x, centro_nave_y;
    obtener_centro_nave(nave, &centro_nave_x, &centro_nave_y);
    float radio_nave = obtener_radio_nave(nave);
    
    return detectar_colision_circular(ataque.x, ataque.y, radio_ataque, centro_nave_x, centro_nave_y, radio_nave);
}


/**
 * @brief Hace que el jefe invoque enemigos.
 * 
 * @param jefe Puntero al jefe.
 * @param enemigos Array de enemigos.
 * @param num_enemigos Puntero al número de enemigos activos.
 * @param imagenes_enemigos Imágenes de enemigos.
 */
void jefe_invocar_enemigos(Jefe* jefe, Enemigo enemigos[], int* num_enemigos, ALLEGRO_BITMAP* imagenes_enemigos[NUM_TIPOS_ENEMIGOS])
{
    int enemigos_a_invocar;
    int invocados;
    int i;
    float pos_x;
    float pos_y;
    int tipo_enemigo;

    if (jefe->enemigos_invocados >= jefe->max_enemigos_invocacion)
    {
        return;
    }
    
    enemigos_a_invocar = jefe->en_furia ? 4 : 2;
    invocados = 0;

    for (i = 0; i < NUM_ENEMIGOS && invocados < enemigos_a_invocar; i++)
    {
        if (!enemigos[i].activo)
        {
            pos_x = 50 + (rand() % 700);
            pos_y = 30 + (rand() % 100);

            if (jefe->tipo == 0)
            {
                tipo_enemigo = rand() % 2; // Normal o Perseguidor
            }
            else
            {
                tipo_enemigo = 1 + (rand() % 3); // Perseguidor, Francotirador o Tanque
            }

            init_enemigo_tipo(&enemigos[i], (int)(pos_x / TILE_ANCHO), (int)(pos_y / TILE_ALTO), tipo_enemigo, imagenes_enemigos[tipo_enemigo]);

            asignar_imagen_enemigo(&enemigos[i], imagenes_enemigos);

            enemigos[i].activo = true;
            invocados++;
            jefe->enemigos_invocados++;

            if (*num_enemigos < i + 1)
            {
                *num_enemigos = i + 1; // Asegurar que el número de enemigos activos se actualice
            }
        }
    }

    printf("Jefe invocó %d enemigos (Total invocados: %d/%d)\n", invocados, jefe->enemigos_invocados, jefe->max_enemigos_invocacion);
}


/**
 * @brief Aplica daño al jefe.
 * 
 * @param jefe Puntero al jefe.
 * @param dano Cantidad de daño a aplicar.
 * @param cola_mensajes Cola de mensajes para notificaciones.
 * @return true si el jefe sigue vivo, false si fue derrotado.
 */
bool jefe_recibir_dano(Jefe* jefe, float dano, ColaMensajes* cola_mensajes)
{
    char mensaje[100];
    jefe->vida -= dano;

    printf("Jefe recibió %.2f de daño. Vida restante: %.2f/%.2f\n", dano, jefe->vida, jefe->vida_max);
    
    if (jefe->vida <= 0.0f)
    {
        jefe->vida = 0.0f;
        jefe->activo = false;
        
        
        sprintf(mensaje, "JEFE %s DERROTADO!", jefe->tipo == 0 ? "DESTRUCTOR" : "SUPREMO");
        agregar_mensaje_cola(cola_mensajes, mensaje, 4.0, al_map_rgb(255, 255, 0), true);
        
        printf("Jefe derrotado!\n");
        return false;
    }
    
    return true;
}


/**
 * @brief Función wrapper para actualizar estado de nivel sin jefe.
 * 
 * Simplifica las llamadas a actualizar_estado_nivel para niveles que no tienen jefe,
 * evitando tener que pasar false y NULL explícitamente.
 * 
 * @param estado Puntero al estado del juego.
 * @param enemigos Arreglo de enemigos del nivel actual.
 * @param num_enemigos Número de enemigos en el arreglo.
 * @param tiempo_actual Tiempo actual del juego.
 */
void actualizar_estado_nivel_sin_jefe(EstadoJuego* estado, Enemigo enemigos[], int num_enemigos, double tiempo_actual)
{
    actualizar_estado_nivel(estado, enemigos, num_enemigos, tiempo_actual, false, NULL);
}


/**
 * @brief Dibuja un botón individual con el estilo elegante del menú principal.
 * 
 * @param boton Botón a dibujar.
 * @param fuente Fuente usada para el texto.
 * @param cursor_x Posición x del cursor.
 * @param cursor_y Posición y del cursor.
 */
void dibujar_boton_individual(Boton boton, ALLEGRO_FONT* fuente, int cursor_x, int cursor_y)
{
    ALLEGRO_COLOR color_borde;
    ALLEGRO_COLOR color_texto;
    float grosor_borde = 2.0f;
    
    // ✅ VERIFICAR SI EL CURSOR ESTÁ SOBRE EL BOTÓN
    if (cursor_sobre_boton(boton, cursor_x, cursor_y))
    {
        // Botón con hover - más brillante
        color_borde = al_map_rgb(255, 255, 255);    // Borde blanco brillante
        color_texto = al_map_rgb(255, 255, 0);      // Texto amarillo
        grosor_borde = 3.0f;                        // Borde más grueso
    }
    else
    {
        // Botón normal
        color_borde = al_map_rgb(150, 150, 150);    // Borde gris
        color_texto = al_map_rgb(255, 255, 255);    // Texto blanco
    }
    
    // ✅ SOLO DIBUJAR EL BORDE (SIN FONDO NEGRO)
    al_draw_rectangle(boton.x, boton.y, 
                     boton.x + boton.ancho, 
                     boton.y + boton.alto, 
                     color_borde, grosor_borde);
    
    // ✅ DIBUJAR EL TEXTO CENTRADO
    al_draw_text(fuente, color_texto, 
                boton.x + boton.ancho / 2, 
                boton.y + boton.alto / 2 - al_get_font_line_height(fuente) / 2, 
                ALLEGRO_ALIGN_CENTER, boton.texto);
}


/**
 * @brief Dibuja la información del escudo en el HUD.
 * 
 * Muestra el estado del escudo, hits restantes y barra de resistencia
 * de manera visual e informativa.
 * 
 * @param nave Nave del jugador con información del escudo.
 * @param fuente Fuente de texto para renderizar la información.
 */
void dibujar_info_escudo(Nave nave, ALLEGRO_FONT *fuente)
{
    int escudo_x = 10;
    int escudo_y = 500;
    int cuadro_ancho = 200;
    int cuadro_alto = 50;
    int barra_x;
    int barra_y;
    int barra_ancho;
    int barra_alto;
    float porcentaje_escudo;
    ALLEGRO_COLOR color_barra;
    int i;
    int punto_x;
    int punto_y;
    ALLEGRO_COLOR color_punto;
    ALLEGRO_COLOR color_fondo_inactivo;
    
    if (nave.escudo.activo && nave.escudo.hits_restantes > 0)
    {
        // ✅ FONDO DEL CUADRO DE ESCUDO
        ALLEGRO_COLOR color_fondo = al_map_rgba(0, 100, 150, 120);
        al_draw_filled_rectangle(escudo_x, escudo_y, escudo_x + cuadro_ancho, escudo_y + cuadro_alto, color_fondo);
        al_draw_rectangle(escudo_x, escudo_y, escudo_x + cuadro_ancho, escudo_y + cuadro_alto, 
                         al_map_rgb(0, 255, 255), 2);
        
        // ✅ TÍTULO DEL ESCUDO
        al_draw_text(fuente, al_map_rgb(0, 255, 255), escudo_x + 10, escudo_y + 64, 
                    ALLEGRO_ALIGN_LEFT, "ESCUDO ACTIVO");
        
        // ✅ BARRA DE RESISTENCIA VISUAL
        barra_x = escudo_x + 10;
        barra_y = escudo_y + 25;
        barra_ancho = cuadro_ancho - 20;
        barra_alto = 8;
        
        porcentaje_escudo = (float)nave.escudo.hits_restantes / nave.escudo.hits_max;
        
        // Color de la barra según la resistencia restante
        if (porcentaje_escudo > 0.66f)
            color_barra = al_map_rgb(0, 255, 100);      // Verde - escudo fuerte
        else if (porcentaje_escudo > 0.33f)
            color_barra = al_map_rgb(255, 255, 0);      // Amarillo - escudo medio
        else
            color_barra = al_map_rgb(255, 100, 0);      // Naranja - escudo débil
        
        // Fondo de la barra
        al_draw_filled_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                               al_map_rgba(60, 60, 60, 150));
        
        // Progreso de la barra
        al_draw_filled_rectangle(barra_x, barra_y, barra_x + (barra_ancho * porcentaje_escudo), barra_y + barra_alto, 
                               color_barra);
        
        // Borde de la barra
        al_draw_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                        al_map_rgb(150, 150, 150), 1);
        
        // ✅ INDICADORES VISUALES DE HITS (puntos individuales)
        for (i = 0; i < nave.escudo.hits_max; i++)
        {
            punto_x = barra_x + 5 + (i * 20);
            punto_y = barra_y + barra_alto + 8;
            
            color_punto = (i < nave.escudo.hits_restantes) ? al_map_rgb(0, 255, 255) : al_map_rgba(100, 100, 100, 100);
            
            al_draw_filled_circle(punto_x, punto_y, 4, color_punto);
            al_draw_circle(punto_x, punto_y, 4, al_map_rgb(200, 200, 200), 1);
        }
    }
    else
    {
        // ✅ MOSTRAR ESCUDO INACTIVO (más sutil)
        color_fondo_inactivo = al_map_rgba(60, 60, 60, 80);
        al_draw_filled_rectangle(escudo_x, escudo_y, escudo_x + cuadro_ancho, escudo_y + 35, color_fondo_inactivo);
        al_draw_rectangle(escudo_x, escudo_y, escudo_x + cuadro_ancho, escudo_y + 35, 
                         al_map_rgb(100, 100, 100), 1);
        
        al_draw_text(fuente, al_map_rgb(150, 150, 150), escudo_x + 10, escudo_y + 48, 
                    ALLEGRO_ALIGN_LEFT, "Sin escudo activo");
    }
}


/**
 * @brief Inicializa la configuración de controles del juego.
 * 
 * @param config Puntero a la configuración de control.
 */
void init_configuracion_control(ConfiguracionControl *config)
{
    config->tipo_control = CONTROL_TECLADO;
    config->joystick = NULL;
    config->joystick_disponible = false;
    config->numero_joystick = -1;
    strcpy(config->nombre_joystick, "No detectado");
    
    // ✅ DETECTAR JOYSTICKS AL INICIALIZAR (AHORA ALLEGRO YA ESTÁ LISTO)
    bool joystick_detectado = detectar_joysticks(config);
    printf("Configuración inicial: Joystick %s\n", joystick_detectado ? "detectado" : "no detectado");
}

/**
 * @brief Detecta joysticks conectados al sistema.
 * 
 * @param config Puntero a la configuración de control.
 * @return true si se encontró al menos un joystick, false en caso contrario.
 */
bool detectar_joysticks(ConfiguracionControl *config)
{
    al_reconfigure_joysticks(); // ✅ Actualizar lista de joysticks
    int num_joysticks = al_get_num_joysticks();
    
    printf("Detectando joysticks... Encontrados: %d\n", num_joysticks);
    
    if (num_joysticks > 0)
    {
        config->joystick = al_get_joystick(0);
        if (config->joystick)
        {
            config->joystick_disponible = true;
            config->numero_joystick = 0;
            
            const char *nombre = al_get_joystick_name(config->joystick);
            if (nombre)
            {
                strncpy(config->nombre_joystick, nombre, 99);
                config->nombre_joystick[99] = '\0';
            }
            else
            {
                strcpy(config->nombre_joystick, "Controlador genérico");
            }
            
            printf("Joystick detectado: %s\n", config->nombre_joystick);
            return true;
        }
    }
    
    config->joystick_disponible = false; // ✅ MARCAR COMO NO DISPONIBLE
    printf("No se encontraron joysticks compatibles.\n");
    return false;
}

/**
 * @brief Muestra el menú de selección de control optimizado.
 * 
 * @param fuente Fuente para el texto.
 * @param config Configuración de control.
 * @param cola_eventos Cola de eventos.
 */
void mostrar_menu_seleccion_control(ALLEGRO_FONT *fuente, ConfiguracionControl *config, ALLEGRO_EVENT_QUEUE *cola_eventos)
{
    bool seleccionando = true;
    int opcion_seleccionada = 0; // 0 = Teclado, 1 = Joystick
    ALLEGRO_EVENT evento;
    bool necesita_redibujo = true;
    
    printf("Iniciando menú de selección de control...\n");
    
    // ✅ CREAR TIMER DEDICADO PARA EL MENÚ
    ALLEGRO_TIMER *timer_menu = al_create_timer(1.0/60.0); // 60 FPS
    if (!timer_menu)
    {
        printf("Error creando timer del menú\n");
        return;
    }
    
    // ✅ REGISTRAR TIMER EN LA COLA DE EVENTOS
    al_register_event_source(cola_eventos, al_get_timer_event_source(timer_menu));
    al_start_timer(timer_menu);
    
    // Registrar eventos de joystick si está disponible
    if (config->joystick_disponible && config->joystick)
    {
        al_register_event_source(cola_eventos, al_get_joystick_event_source());
        printf("Eventos de joystick registrados\n");
    }
    
    // ✅ VARIABLES DE CONTROL DE ENTRADA
    double ultimo_input_joystick = 0;
    const double delay_input = 0.3; // 300ms entre inputs
    
    // ✅ DIBUJO INICIAL
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 150, ALLEGRO_ALIGN_CENTER, "SELECCIONA EL TIPO DE CONTROL");
    
    if (config->joystick_disponible)
    {
        char info_joystick[150];
        sprintf(info_joystick, "Controlador detectado: %s", config->nombre_joystick);
        al_draw_text(fuente, al_map_rgb(0, 255, 0), 400, 180, ALLEGRO_ALIGN_CENTER, info_joystick);
    }
    
    al_flip_display();
    
    while (seleccionando)
    {
        al_wait_for_event(cola_eventos, &evento);
        
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            seleccionando = false;
            break;
        }
        
        // ✅ NAVEGACIÓN CON TECLADO (OPTIMIZADA)
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (evento.keyboard.keycode)
            {
                case ALLEGRO_KEY_UP:
                case ALLEGRO_KEY_W:
                    opcion_seleccionada = 0;
                    necesita_redibujo = true;
                    break;
                    
                case ALLEGRO_KEY_DOWN:
                case ALLEGRO_KEY_S:
                    opcion_seleccionada = config->joystick_disponible ? 1 : 0;
                    necesita_redibujo = true;
                    break;
                    
                case ALLEGRO_KEY_ENTER:
                case ALLEGRO_KEY_SPACE:
                    if (opcion_seleccionada == 0)
                    {
                        config->tipo_control = CONTROL_TECLADO;
                        printf("Control seleccionado: Teclado\n");
                    }
                    else if (opcion_seleccionada == 1 && config->joystick_disponible)
                    {
                        config->tipo_control = CONTROL_JOYSTICK;
                        printf("Control seleccionado: Joystick (%s)\n", config->nombre_joystick);
                    }
                    seleccionando = false;
                    break;
                    
                case ALLEGRO_KEY_ESCAPE:
                    config->tipo_control = CONTROL_TECLADO; // Por defecto
                    seleccionando = false;
                    break;
            }
        }
        
        // ✅ NAVEGACIÓN CON JOYSTICK (OPTIMIZADA CON THROTTLING)
        if (evento.type == ALLEGRO_EVENT_JOYSTICK_AXIS && config->joystick_disponible)
        {
            double tiempo_actual = al_get_time();
            
            if (evento.joystick.axis == 1 && tiempo_actual - ultimo_input_joystick > delay_input)
            {
                if (evento.joystick.pos < -DEADZONE_JOYSTICK)
                {
                    opcion_seleccionada = 0;
                    ultimo_input_joystick = tiempo_actual;
                    necesita_redibujo = true;
                }
                else if (evento.joystick.pos > DEADZONE_JOYSTICK)
                {
                    opcion_seleccionada = config->joystick_disponible ? 1 : 0;
                    ultimo_input_joystick = tiempo_actual;
                    necesita_redibujo = true;
                }
            }
        }
        
        // ✅ SELECCIÓN CON BOTÓN DE JOYSTICK (OPTIMIZADA)
        if (evento.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN && config->joystick_disponible)
        {
            if (evento.joystick.button == 0) // Botón A/X
            {
                if (opcion_seleccionada == 0)
                {
                    config->tipo_control = CONTROL_TECLADO;
                    printf("Control seleccionado: Teclado\n");
                }
                else if (opcion_seleccionada == 1)
                {
                    config->tipo_control = CONTROL_JOYSTICK;
                    printf("Control seleccionado: Joystick (%s)\n", config->nombre_joystick);
                }
                seleccionando = false;
            }
            else if (evento.joystick.button == 1) // Botón B/Circle (Cancelar)
            {
                config->tipo_control = CONTROL_TECLADO;
                seleccionando = false;
            }
        }
        
        // ✅ REDIBUJADO SOLO CUANDO ES NECESARIO
        if (evento.type == ALLEGRO_EVENT_TIMER && necesita_redibujo)
        {
            necesita_redibujo = false;
            
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            // Título
            al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 150, ALLEGRO_ALIGN_CENTER, "SELECCIONA EL TIPO DE CONTROL");
            
            // Información del joystick detectado
            if (config->joystick_disponible)
            {
                char info_joystick[150];
                sprintf(info_joystick, "Controlador detectado: %s", config->nombre_joystick);
                al_draw_text(fuente, al_map_rgb(0, 255, 0), 400, 180, ALLEGRO_ALIGN_CENTER, info_joystick);
            }
            
            // Opción Teclado
            ALLEGRO_COLOR color_teclado = (opcion_seleccionada == 0) ? 
                al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
            al_draw_text(fuente, color_teclado, 400, 250, ALLEGRO_ALIGN_CENTER, 
                        "1. TECLADO");
            
            // Opción Joystick
            ALLEGRO_COLOR color_joystick;
            char texto_joystick[150];
            
            if (config->joystick_disponible)
            {
                color_joystick = (opcion_seleccionada == 1) ? 
                    al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
                sprintf(texto_joystick, "2. CONTROLADOR (%s)", config->nombre_joystick);
            }
            else
            {
                color_joystick = al_map_rgb(100, 100, 100);
                strcpy(texto_joystick, "2. CONTROLADOR (No detectado)");
            }
            
            al_draw_text(fuente, color_joystick, 400, 300, ALLEGRO_ALIGN_CENTER, texto_joystick);
            
            // Indicador de selección
            if (opcion_seleccionada == 0)
            {
                al_draw_text(fuente, al_map_rgb(255, 255, 0), 350, 250, ALLEGRO_ALIGN_CENTER, ">");
            }
            else if (config->joystick_disponible && opcion_seleccionada == 1)
            {
                al_draw_text(fuente, al_map_rgb(255, 255, 0), 350, 300, ALLEGRO_ALIGN_CENTER, ">");
            }
            
            // Instrucciones
            al_draw_text(fuente, al_map_rgb(200, 200, 200), 400, 380, ALLEGRO_ALIGN_CENTER, 
                        "Usa las flechas o stick para navegar");
            al_draw_text(fuente, al_map_rgb(200, 200, 200), 400, 410, ALLEGRO_ALIGN_CENTER, 
                        "Presiona ENTER, ESPACIO o botón X/Cuadrado para seleccionar");
            al_draw_text(fuente, al_map_rgb(200, 200, 200), 400, 440, ALLEGRO_ALIGN_CENTER, 
                        "ESC o botón Círculo para usar teclado por defecto");
            
            al_flip_display();
        }
    }
    
    // ✅ LIMPIEZA DE RECURSOS
    al_stop_timer(timer_menu);
    al_unregister_event_source(cola_eventos, al_get_timer_event_source(timer_menu));
    al_destroy_timer(timer_menu);
    
    printf("Menú de selección cerrado. Control elegido: %s\n", config->tipo_control == CONTROL_JOYSTICK ? "Joystick" : "Teclado");
}

/**
 * @brief Maneja los eventos de joystick para el movimiento de la nave.
 * 
 * @param evento Evento de joystick.
 * @param nave Puntero a la nave.
 * @param teclas Arreglo de estados de teclas (para compatibilidad).
 */
void manejar_eventos_joystick(ALLEGRO_EVENT evento, Nave* nave, bool teclas[])
{
    // Esta función procesa eventos de joystick, pero el movimiento real
    // se maneja en actualizar_nave_joystick()
    
    if (evento.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
    {
        printf("Botón de joystick presionado: %d\n", evento.joystick.button);
    }
}

/**
 * @brief Actualiza el movimiento de la nave usando joystick.
 * 
 * @param nave Puntero a la nave.
 * @param joystick Puntero al joystick.
 * @param tilemap Mapa de tiles para colisiones.
 */
void actualizar_nave_joystick(Nave* nave, ALLEGRO_JOYSTICK *joystick, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS])
{
    ALLEGRO_JOYSTICK_STATE estado_joystick;
    
    if (!joystick) return;
    
    // ✅ OBTENER EL ESTADO ACTUAL DEL JOYSTICK CORRECTAMENTE
    al_get_joystick_state(joystick, &estado_joystick);
    
    float nueva_x = nave->x;
    float nueva_y = nave->y;
    
    if (nave->tipo == 0) // Movimiento Space Invaders
    {
        // ✅ STICK IZQUIERDO PARA MOVIMIENTO: ARRIBA, IZQUIERDA, DERECHA (NO ABAJO)
        float stick_x = estado_joystick.stick[0].axis[0]; // Horizontal del stick izquierdo
        float stick_y = estado_joystick.stick[0].axis[1]; // Vertical del stick izquierdo
        
        // Movimiento horizontal (izquierda/derecha)
        if (fabs(stick_x) > DEADZONE_JOYSTICK)
        {
            nueva_x += stick_x * 5.0f;
        }
        
        // Verificar límites de pantalla
        if (nueva_x >= 0 && nueva_x <= 800 - nave->ancho)
        {
            nave->x = nueva_x;
        }
        
        if (nueva_y >= 0 && nueva_y <= 600 - nave->largo)
        {
            nave->y = nueva_y;
        }
    }
    else // Movimiento con rotación
    {
        const float velocidad_rotacion = 0.08f;
        const float velocidad_movimiento = 4.0f;
        
        // ✅ STICK DERECHO PARA ROTACIÓN (CORRECCIÓN PRINCIPAL)
        float stick_rx = estado_joystick.stick[1].axis[0]; // ✅ STICK DERECHO HORIZONTAL (era el error)
        
        // Botones de hombro como alternativa para rotación
        bool boton_L = false;
        bool boton_R = false;
        
        // Verificar si hay suficientes botones antes de acceder
        if (al_get_joystick_num_buttons(joystick) > 4)
        {
            boton_L = estado_joystick.button[4]; // L1/LB
        }
        if (al_get_joystick_num_buttons(joystick) > 5)
        {
            boton_R = estado_joystick.button[5]; // R1/RB
        }
        
        // ✅ ROTACIÓN CON STICK DERECHO (PRIORIDAD ALTA)
        if (fabs(stick_rx) > DEADZONE_JOYSTICK)
        {
            nave->angulo += stick_rx * velocidad_rotacion;
            printf("Rotando con stick derecho: %.2f\n", stick_rx); // Debug
        }
        // Botones de hombro como backup
        else if (boton_L)
        {
            nave->angulo -= velocidad_rotacion;
            printf("Rotando con L1\n"); // Debug
        }
        else if (boton_R)
        {
            nave->angulo += velocidad_rotacion;
            printf("Rotando con R1\n"); // Debug
        }
        
        // ✅ STICK IZQUIERDO PARA MOVIMIENTO (SOLO HACIA ADELANTE)
        float stick_y = estado_joystick.stick[0].axis[1]; // Stick izquierdo vertical
        
        // Solo permitir movimiento hacia adelante (valores negativos)
        if (stick_y < -DEADZONE_JOYSTICK)
        {
            nueva_x = nave->x + cos(nave->angulo - ALLEGRO_PI/2) * (-stick_y * velocidad_movimiento);
            nueva_y = nave->y + sin(nave->angulo - ALLEGRO_PI/2) * (-stick_y * velocidad_movimiento);
            printf("Moviendo hacia adelante: %.2f\n", stick_y); // Debug
        }
        
        // Verificar colisiones y límites
        if (!verificar_colision_nave_muro(nueva_x, nueva_y, nave->ancho, nave->largo, tilemap))
        {
            if (nueva_x >= 0 && nueva_x <= 800 - nave->ancho &&
                nueva_y >= 0 && nueva_y <= 600 - nave->largo)
            {
                nave->x = nueva_x;
                nave->y = nueva_y;
            }
        }
    }
}


/**
 * @brief Verifica si se está presionando el botón de disparar en el joystick.
 * 
 * @param joystick Puntero al joystick.
 * @return true si se está presionando el botón de disparo.
 */
bool obtener_boton_joystick_disparar(ALLEGRO_JOYSTICK *joystick)
{
    ALLEGRO_JOYSTICK_STATE estado_joystick;
    
    if (!joystick) return false;
    
    al_get_joystick_state(joystick, &estado_joystick);
    
    // ✅ BOTÓN X/CUADRADO (botón 2 en PlayStation, botón 0 en Xbox)
    bool boton_x = false;
    
    // Verificar botón X/Cuadrado según el tipo de controlador
    if (al_get_joystick_num_buttons(joystick) > 2)
    {
        boton_x = estado_joystick.button[2]; // PlayStation: X/Cuadrado
    }
    if (al_get_joystick_num_buttons(joystick) > 0)
    {
        boton_x = boton_x || estado_joystick.button[0]; // Xbox: A
    }
    
    return boton_x;
}

/**
 * @brief Maneja el cambio de armas con joystick (VERSIÓN MEJORADA).
 * 
 * @param nave Puntero a la nave.
 * @param joystick Puntero al joystick.
 */
void cambiar_arma_joystick(Nave *nave, ALLEGRO_JOYSTICK *joystick)
{
    ALLEGRO_JOYSTICK_STATE estado_joystick;
    static double ultimo_cambio = 0;
    static int ultimo_dpad_estado = 0; // Para detectar transiciones
    double tiempo_actual = al_get_time();
    
    if (!joystick) return;
    
    al_get_joystick_state(joystick, &estado_joystick);
    
    // ✅ THROTTLING: Evitar cambios muy rápidos
    if (tiempo_actual - ultimo_cambio < 0.5) return; // 500ms entre cambios
    
    bool cambio_realizado = false;
    int dpad_estado_actual = 0;
    
    // ✅ MÉTODO UNIFICADO: D-PAD COMO EJES (PlayStation) O BOTONES (Xbox)
    int num_ejes = al_get_joystick_num_sticks(joystick);
    int num_botones = al_get_joystick_num_buttons(joystick);
    
    printf("🎮 Controlador: %d sticks, %d botones\n", num_ejes, num_botones);
    
    // Intentar D-pad como ejes adicionales (PlayStation)
    if (num_ejes >= 2 && al_get_joystick_num_axes(joystick, 0) >= 8)
    {
        float dpad_x = estado_joystick.stick[0].axis[6]; // D-pad horizontal
        float dpad_y = estado_joystick.stick[0].axis[7]; // D-pad vertical
        
        if (fabs(dpad_y) > 0.5f || fabs(dpad_x) > 0.5f)
        {
            if (dpad_y < -0.5f) dpad_estado_actual = 1; // Arriba
            else if (dpad_x > 0.5f) dpad_estado_actual = 2; // Derecha
            else if (dpad_y > 0.5f) dpad_estado_actual = 3; // Abajo
            else if (dpad_x < -0.5f) dpad_estado_actual = 4; // Izquierda
        }
    }
    // Intentar D-pad como botones (Xbox)
    else if (num_botones >= 16)
    {
        if (estado_joystick.button[12]) dpad_estado_actual = 1; // Arriba
        else if (estado_joystick.button[15]) dpad_estado_actual = 2; // Derecha
        else if (estado_joystick.button[13]) dpad_estado_actual = 3; // Abajo
        else if (estado_joystick.button[14]) dpad_estado_actual = 4; // Izquierda
    }
    // Método alternativo: Botones principales
    else if (num_botones >= 4)
    {
        if (estado_joystick.button[3]) dpad_estado_actual = 1; // Triangle/Y -> Normal
        else if (estado_joystick.button[1]) dpad_estado_actual = 2; // Circle/B -> Láser
        else if (estado_joystick.button[0]) dpad_estado_actual = 3; // X/A -> Explosiva
        else if (estado_joystick.button[2]) dpad_estado_actual = 4; // Square/X -> Misil
    }
    
    // ✅ DETECTAR TRANSICIÓN: Solo cambiar cuando se presiona (no mantener presionado)
    if (dpad_estado_actual != 0 && dpad_estado_actual != ultimo_dpad_estado)
    {
        switch (dpad_estado_actual)
        {
            case 1: // Arriba
                cambiar_arma(nave, Arma_normal);
                cambio_realizado = true;
                printf("✅ Arma cambiada a Normal (D-pad Arriba)\n");
                break;
                
            case 2: // Derecha
                cambiar_arma(nave, Arma_laser);
                cambio_realizado = true;
                printf("✅ Arma cambiada a Láser (D-pad Derecha)\n");
                break;
                
            case 3: // Abajo
                cambiar_arma(nave, Arma_explosiva);
                cambio_realizado = true;
                printf("✅ Arma cambiada a Explosiva (D-pad Abajo)\n");
                break;
                
            case 4: // Izquierda
                cambiar_arma(nave, Arma_misil);
                cambio_realizado = true;
                printf("✅ Arma cambiada a Misil (D-pad Izquierda)\n");
                break;
        }
    }
    
    // Actualizar estados
    ultimo_dpad_estado = dpad_estado_actual;
    
    if (cambio_realizado)
    {
        ultimo_cambio = tiempo_actual;
        printf("🔫 Arma actual: %s (Nivel %d)\n", 
               nave->armas[nave->arma_seleccionada].nombre, 
               nave->armas[nave->arma_seleccionada].nivel);
    }
}

/**
 * @brief Dibuja un indicador del tipo de control activo.
 * 
 * @param config Configuración de control.
 * @param fuente Fuente para el texto.
 */
void dibujar_indicador_control(ConfiguracionControl config, ALLEGRO_FONT *fuente)
{
    char texto_control[100];
    ALLEGRO_COLOR color = al_map_rgb(200, 200, 200);
    
    if (config.tipo_control == CONTROL_TECLADO)
    {
        strcpy(texto_control, "Control: Teclado");
    }
    else
    {
        sprintf(texto_control, "Control: %s", config.nombre_joystick);
    }
    
    al_draw_text(fuente, color, 10, 580, ALLEGRO_ALIGN_LEFT, texto_control);
}


/**
 * @brief Función de debug para mostrar estado del joystick.
 */
void debug_joystick_estado(ALLEGRO_JOYSTICK *joystick)
{
    if (!joystick) return;
    
    ALLEGRO_JOYSTICK_STATE estado;
    al_get_joystick_state(joystick, &estado);
    
    static double ultimo_debug = 0;
    double tiempo_actual = al_get_time();
    
    if (tiempo_actual - ultimo_debug > 1.0) // Cada segundo
    {
        printf("🎮 DEBUG JOYSTICK:\n");
        printf("   Sticks: %d\n", al_get_joystick_num_sticks(joystick));
        printf("   Botones: %d\n", al_get_joystick_num_buttons(joystick));
        
        // Mostrar botones presionados
        for (int i = 0; i < al_get_joystick_num_buttons(joystick) && i < 16; i++)
        {
            if (estado.button[i])
            {
                printf("   Botón %d: PRESIONADO\n", i);
            }
        }
        
        // Mostrar ejes si hay suficientes
        if (al_get_joystick_num_axes(joystick, 0) >= 8)
        {
            printf("   D-pad X: %.2f, Y: %.2f\n", estado.stick[0].axis[6], estado.stick[0].axis[7]);
        }
        
        ultimo_debug = tiempo_actual;
    }
}