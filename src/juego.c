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
Nave init_nave(float x, float y, float ancho, float largo, int vida, double tiempo_invulnerable, ALLEGRO_BITMAP* imagen_nave)
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
 */
void actualizar_asteroide(Asteroide* asteroide, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Nave* nave)
{
    // Verifica colisión con la nave
    float centro_nave_x = nave->x + nave->ancho / 2;
    float centro_nave_y = nave->y + nave->largo / 2;
    float radio_nave = nave->ancho / 2.0f;
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

    asteroide->y += asteroide->velocidad;

    if (detectar_colision_circular(centro_nave_x, centro_nave_y, radio_nave, centro_asteroide_x, centro_asteroide_y, radio_asteroide))
    {
        // Asteroide impacta la nave: desaparece
        asteroide->y = -asteroide->alto;
        asteroide->x = rand() % (800 - (int)asteroide->ancho);
        nave->vida -= 10;
        return;
    }

    // Verifica TODAS las celdas que ocupa el asteroide
    for (fila = fila_superior; fila <= fila_inferior; fila++)
    {
        for (col = col_izquierda; col <= col_derecha; col++)
        {
            if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS)
            {
                if ((tilemap[fila][col].tipo == 2 && tilemap[fila][col].vida > 0) || tilemap[fila][col].tipo == 3 || tilemap[fila][col].tipo == 1)
            {
                    if (tilemap[fila][col].tipo == 2)
                    {
                        tilemap[fila][col].vida--;
                        if (tilemap[fila][col].vida <= 0)
                        {
                            tilemap[fila][col].tipo = 0;
                        }
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
 * @brief Detecta colisiones entre la nave y un asteroide.
 *
 * Esta función verifica si la nave y el asteroide se están tocando.
 *
 * @param nave Puntero a la nave a la que se verifica la colisión.
 * @param asteroide El asteroide a verificar.
 * @return true si hay colisión, false en caso contrario.
 */
bool detectar_colision(Nave* nave, Asteroide asteroide)
{
    float centro_nave_x = nave->x + nave->ancho / 2;
    float centro_nave_y = nave->y + nave->largo / 2;
    float radio_nave = nave->ancho / 2.0f;

    float centro_asteroide_x = asteroide.x + asteroide.ancho / 2;
    float centro_asteroide_y = asteroide.y + asteroide.alto / 2;
    float radio_asteroide = asteroide.ancho / 2.0f;

    return detectar_colision_circular(centro_nave_x, centro_nave_y, radio_nave, centro_asteroide_x, centro_asteroide_y, radio_asteroide);
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
void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave, bool teclas[], Disparo disparos[], int num_disparos)
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
            disparar_radial(disparos, num_disparos, *nave);
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
void dibujar_juego(Nave nave, Asteroide asteroides[], int num_asteroides, int nivel_actual)
{
    // al_draw_bitmap(imagen_fondo, 0, 0, 0);
    float cx = al_get_bitmap_width(nave.imagen) / 2.0f;
    float cy = al_get_bitmap_height(nave.imagen) / 2.0f;
    float escala_x = nave.ancho / al_get_bitmap_width(nave.imagen);
    float escala_y = nave.largo / al_get_bitmap_height(nave.imagen);
    int i;

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
void actualizar_nave(Nave* nave, bool teclas[], Asteroide asteroides[], double tiempo_actual)
{
    int i;
    float nueva_x = nave->x;
    float nueva_y = nave->y;

    if(nave->tipo == 0)
    {
        // Movilidad tipo Space Invaders: solo izquierda/derecha
        if (teclas[2]) nueva_x -= 5; // Izquierda
        if (teclas[3]) nueva_x += 5; // Derecha

        // Limitar el movimiento de la nave dentro de la ventana
        if (nueva_x < 0) nueva_x = 0;
        if (nueva_x > 800 - nave->ancho) nueva_x = 800 - nave->ancho;

        if (!verificar_colision_nave_muro(nueva_x, nave->y, nave->ancho, nave->largo))
        {
            nave->x = nueva_x;
        }
        
        // No permitir movimiento vertical ni rotación
        nave->angulo = 0.0f;
    }
    else
    {
        // Rotación de la nave
        if(teclas[2]) nave->angulo -= 0.07f; // Izquierda
        if(teclas[3]) nave->angulo += 0.07f; // Derecha

        if (teclas[0]) // Arriba (avanzar)
        {
            nueva_x += cos(nave->angulo - ALLEGRO_PI/2) * 5;
            nueva_y += sin(nave->angulo - ALLEGRO_PI/2) * 5;
        }

        // Limitar el movimiento de la nave dentro de la ventana
        if (nueva_x < 0) nueva_x = 0;
        if (nueva_x > 800 - nave->ancho) nueva_x = 800 - nave->ancho;
        if (nueva_y < 0) nueva_y = 0;
        if (nueva_y > 600 - nave->largo) nueva_y = 600 - nave->largo;

        if (!verificar_colision_nave_muro(nueva_x, nueva_y, nave->ancho, nave->largo))
        {
            nave->x = nueva_x;
            nave->y = nueva_y;
        }
    }

    for (i = 0; i < NUM_ASTEROIDES; i++)
    {
        if (detectar_colision(nave, asteroides[i]))
        {
            printf("Colisión detectada con el asteroide %d\n", i);
            nave->vida -= 5;
            nave->tiempo_invulnerable = tiempo_actual + 10;
            if (nave->vida <= 0)
            {
                printf("Nave destruida\n");
                // implementar codigo para finalizar el juego
            }
        }        
    }
}


/**
 * @brief Dibuja la barra de vida de la nave.
 * 
 * @param nave Nave a la que se le va a dibujar la barra de vida.
 */
void dibujar_barra_vida(Nave nave)
{
    float porcentaje_vida = (float)nave.vida / 100.0;
    al_draw_filled_rectangle(10, 10, 10 + (200 * porcentaje_vida), 30, al_map_rgb(255, 0, 0));
    al_draw_rectangle(10, 10, 210, 30, al_map_rgb(255, 255, 255), 2);
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
    for (int i = 0; i < num_disparos; i++)
    {
        if (!disparos[i].activo)
        {
            // Calcula el centro de la nave
            float centro_x = nave.x + nave.ancho / 2.0f;
            float centro_y = nave.y + nave.largo / 2.0f;
            // Calcula la punta de la nave desde el centro, usando el ángulo y la mitad del largo
            float punta_x = centro_x + cos(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);
            float punta_y = centro_y + sin(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);

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
    return disparo.x < asteroide.x + asteroide.ancho &&
           disparo.x + 5 > asteroide.x &&
           disparo.y < asteroide.y + asteroide.alto &&
           disparo.y + 10 > asteroide.y;
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

    if (estado_nivel->mostrar_transicion)
    {
        actualizar_estado_nivel(estado_nivel, enemigos, num_enemigos, tiempo_actual);
        return;
    }

    actualizar_nave(nave, teclas, asteroides, tiempo_actual);
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
        if (disparos[i].activo)
        {
            int col_disparo = (int)(disparos[i].x / TILE_ANCHO);
            int fila_disparo = (int)(disparos[i].y / TILE_ALTO);

            if (fila_disparo >= 0 && fila_disparo < MAPA_FILAS && col_disparo >= 0 && col_disparo < MAPA_COLUMNAS)
            {
                Tile *tile = &tilemap[fila_disparo][col_disparo];

                if (tile->tipo == 3)
                {
                    disparos[i].activo = false;
                    printf("Disparo rebotó en bloque sólido en (%d, %d)\n", col_disparo, fila_disparo);
                    continue;
                }
                
                if (tile->tipo == 2 && tile->vida > 0)
                {
                    disparos[i].activo = false;
                    tile->vida--;
                    
                    if (tile->vida <= 0)
                    {
                        tile->tipo = 0;
                        printf("Escudo destruido por disparo jugador en (%d, %d)\n", col_disparo, fila_disparo);
                    }
                    else
                    {
                        printf("Escudo dañado por disparo jugador en (%d, %d). Vida restante: %d\n", col_disparo, fila_disparo, tile->vida);
                    }
                    continue;
                }
            }
        }
    }

    if (asteroides_activados(estado_nivel->nivel_actual))
    {
        for (i = 0; i < num_asteroides; i++)
        {
            actualizar_asteroide(&asteroides[i], tilemap, nave);
        
            // bool disparo_impacto = false;
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

        //bool enemigo_impactado = false;
        
        // Disparos del jugador vs enemigos (USA LA NUEVA FUNCIÓN)
        for (j = 0; j < num_disparos; j++)
        {
            if (disparos[j].activo && detectar_colision_disparo_enemigo(disparos[j], enemigos[i]))
            {
                disparos[j].activo = false;
                enemigos[i].vida--;
                if (enemigos[i].vida <= 0)
                {
                    enemigos[i].activo = false;
                    (*puntaje) += 5; // Más puntos por destruir enemigo
                    nave->kills_para_mejora++;
                    verificar_mejora_disparo_radial(nave, cola_mensajes);

                    if (rand() % 100 < POWERUP_PROB)
                    {
                        crear_powerup_escudo(powerups, max_powerups, enemigos[i].x, enemigos[i].y);
                    }
                }
            }
        }

        // Nave vs enemigos (USA LA NUEVA FUNCIÓN)
        if (detectar_colision_nave_enemigo(*nave, enemigos[i]))
        {
            if (!escudo_recibir_dano(&nave->escudo))
            {
                nave->vida -= 20;
                printf("La nave recibio 20 de daño, Vida restante: %d\n", nave->vida);
            }
            enemigos[i].activo = false;
        }
    }

    actualizar_estado_nivel(estado_nivel, enemigos, num_enemigos, tiempo_actual);

    // Disparos de enemigos vs nave (USA LA NUEVA FUNCIÓN)
    for (i = 0; i < num_disparos_enemigos; i++)
    {
        if (disparos_enemigos[i].activo && detectar_colision_disparo_enemigo_nave(*nave, disparos_enemigos[i]))
        {
            disparos_enemigos[i].activo = false;
            
           if (!escudo_recibir_dano(&nave->escudo))
           {
                int dano = 15;
                if (disparos_enemigos[i].velocidad <= 2.5f)
                {
                    dano = 25; // Tanques hacen más daño
                }
                else if (disparos_enemigos[i].velocidad >= 4.0f)
                {
                    dano = 20; // Francotiradores hacen daño medio-alto
                }
                
                nave->vida -= dano;
                printf("Nave recibio %d de daño por disparo enemigo. vida restante: %d\n", dano, nave->vida);
           }
        }
    }

    for (i = 0; i < num_disparos_enemigos; i++)
    {
        if (disparos_enemigos[i].activo)
        {
            bool disparo_procesado = false;
            for (int fila = 0; fila < MAPA_FILAS && !disparo_procesado; fila++)
            {
                for (int col = 0; col < MAPA_COLUMNAS && !disparo_procesado; col++)
                {
                    if (tilemap[fila][col].tipo == 2 && tilemap[fila][col].vida > 0)
                    {
                        float tile_x = col * TILE_ANCHO;
                        float tile_y = fila * TILE_ALTO;

                        if (detectar_colision_disparo_enemigo_escudo(disparos_enemigos[i], tile_x, tile_y))
                        {
                            disparos_enemigos[i].activo = false;
                            tilemap[fila][col].vida--;

                            printf("¡Disparo enemigo impactó escudo en (%d, %d)! Vida restante: %d\n", col, fila, tilemap[fila][col].vida);

                            if (tilemap[fila][col].vida <= 0)
                            {
                                tilemap[fila][col].tipo = 0; // El escudo se destruye
                                printf("¡Escudo en (%d, %d) destruido completamente!\n", col, fila);
                            }

                            disparo_procesado = true;
                        }
                    }
                    else if (tilemap[fila][col].tipo == 3)
                    {
                        float tile_x = col * TILE_ANCHO;
                        float tile_y = fila * TILE_ALTO;

                        if (detectar_colision_disparo_enemigo_escudo(disparos_enemigos[i], tile_x, tile_y))
                        {
                            disparos_enemigos[i].activo = false;
                            printf("¡Disparo enemigo rebotó en muro indestructible en (%d, %d)!\n", col, fila);
                            disparo_procesado = true;
                        }
                    }
                    else if (tilemap[fila][col].tipo == 1)
                    {
                        float tile_x = col * TILE_ANCHO;
                        float tile_y = fila * TILE_ALTO;

                        if (detectar_colision_disparo_enemigo_escudo(disparos_enemigos[i], tile_x, tile_y))
                        {
                            disparos_enemigos[i].activo = false;
                            printf("¡Disparo enemigo impactó asteroide fijo en (%d, %d)!\n", col, fila);
                            disparo_procesado = true;
                        }
                    }
                }
            }
        }
    }

    actualizar_estado_nivel(estado_nivel, enemigos, num_enemigos, tiempo_actual);
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
    for (int i = 0; i < num_botones; i++)
    {
        ALLEGRO_COLOR color = cursor_sobre_boton(botones[i], cursor_x, cursor_y) ? al_map_rgb(255, 255, 0) : al_map_rgb(0, 255, 0);
        al_draw_filled_rectangle(botones[i].x, botones[i].y, botones[i].x + botones[i].ancho, botones[i].y + botones[i].alto, al_map_rgb(0, 0, 0));
        al_draw_text(fuente, color, botones[i].x + botones[i].ancho / 2, botones[i].y + botones[i].alto / 2, ALLEGRO_ALIGN_CENTER, botones[i].texto);
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
    for (int i = 0; i < num_botones; i++)
    {
        if (x >= botones[i].x && x <= botones[i].x + botones[i].ancho &&
            y >= botones[i].y && y <= botones[i].y + botones[i].alto)
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
    FILE* archivo = fopen("ranking.txt", "a");
    if (archivo)
    {
        fprintf(archivo, "%s %d\n", nombre, puntaje);
        fclose(archivo);
    }
    else
    {
        fprintf(stderr, "Error: no se pudo abrir el archivo de ranking.\n");
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
    if (archivo)
    {
        while (fscanf(archivo, "%s %d", ranking[*num_jugadores].nombre, &ranking[*num_jugadores].puntaje) != EOF && *num_jugadores < MAX_JUGADORES)
        {
            (*num_jugadores)++;
        }
        fclose(archivo);

        // Ordenar el ranking de mayor a menor puntaje
        qsort(ranking, *num_jugadores, sizeof(Jugador), comparar_puntajes);
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
    bool mostrar = true;
    ALLEGRO_EVENT_QUEUE* cola_eventos = al_create_event_queue();
    al_register_event_source(cola_eventos, al_get_mouse_event_source());

    Boton boton_volver;
    strcpy(boton_volver.texto, "Volver");
    boton_volver.x = 350;
    boton_volver.y = 500;
    boton_volver.ancho = 100;
    boton_volver.alto = 50;

    while (mostrar)
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        for (int i = 0; i < num_jugadores; i++)
        {
            char texto[100];
            sprintf(texto, "%d. %s - %d", i + 1, ranking[i].nombre, ranking[i].puntaje);
            al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 100 + 50 * i, ALLEGRO_ALIGN_CENTER, texto);
        }
        
        al_draw_filled_rectangle(boton_volver.x, boton_volver.y, boton_volver.x + boton_volver.ancho, boton_volver.y + boton_volver.alto, al_map_rgb(0, 0, 0));
        al_draw_text(fuente, al_map_rgb(0, 255, 0), boton_volver.x + boton_volver.ancho / 2, boton_volver.y + boton_volver.alto / 2, ALLEGRO_ALIGN_CENTER, boton_volver.texto);

        al_flip_display();

        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (cursor_sobre_boton(boton_volver, evento.mouse.x, evento.mouse.y))
            {
                mostrar = false;
                *volver_menu = true;
            }
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

    while (!terminado)
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if ((evento.keyboard.unichar >= 'a' && evento.keyboard.unichar <= 'z') ||
                (evento.keyboard.unichar >= 'A' && evento.keyboard.unichar <= 'Z') ||
                (evento.keyboard.unichar >= '0' && evento.keyboard.unichar <= '9') ||
                evento.keyboard.unichar == '-' || evento.keyboard.unichar == '_')
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
    return x >= boton.x && x <= boton.x + boton.ancho &&
           y >= boton.y && y <= boton.y + boton.alto;
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
    bool nave_encontrada = false;
    char linea[50];

    printf("=== CARGANDO: %s ===\n", filename);

    for (int fila = 0; fila < MAPA_FILAS; fila++)
    {
        // Leer línea completa
        if (fgets(linea, sizeof(linea), archivo) == NULL)
        {
            printf("ERROR: No se pudo leer la fila %d\n", fila);
            break;
        }

        printf("Fila %d: %.10s... (primeros 10 chars)\n", fila, linea);

        // Procesar cada columna de la línea
        int longitud_linea = (int)strlen(linea);
        for (int col = 0; col < MAPA_COLUMNAS && col < longitud_linea; col++)
        {
            char c = linea[col];

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
void dibujar_tilemap(Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], ALLEGRO_BITMAP* imagen_asteroide) {
    for (int fila = 0; fila < MAPA_FILAS; fila++)
    {
        for (int col = 0; col < MAPA_COLUMNAS; col++)
        {
            if (tilemap[fila][col].tipo == 1)
            {
                al_draw_scaled_bitmap(imagen_asteroide, 0, 0, al_get_bitmap_width(imagen_asteroide), al_get_bitmap_height(imagen_asteroide), col * TILE_ANCHO, fila * TILE_ALTO, TILE_ANCHO, TILE_ALTO, 0);
            }
            else if (tilemap[fila][col].tipo == 2) 
            {
                // Dibuja el escudo como un rectángulo azul (puedes usar una imagen si prefieres)
                ALLEGRO_COLOR color = al_map_rgb(0, 128, 255);
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
    for (int i = 0; i < num_enemigos; i++)
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
 * @brief Actualiza la posición y comportamiento de todos los enemigos.
 * 
 * Los enemigos normales (tipo 0) se mueven horizontalmente rebotando en los bordes
 * y disparan hacia la nave. Los enemigos perseguidos (tipo 1) siguen a la nave
 * cuando está dentro de su rango de visión.
 * 
 * @param enemigos Arreglo de enemigos a actualizar.
 * @param num_enemigos Número de enemigos en el arreglo.
 * @param disparos_enemigos Arreglo de disparos de enemigos.
 * @param num_disparos_enemigos Número máximo de disparos de enemigos.
 * @param tiempo_actual Tiempo actual del juego en segundos.
 * @param nave Nave del jugador (para persecución y disparos).
 */
void actualizar_enemigos(Enemigo enemigos[], int num_enemigos, Disparo disparos_enemigos[], int num_disparos_enemigos, double tiempo_actual,Nave nave)
{
    int i;

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
                    float dx = nave.x + nave.ancho/2 - (enemigos[i].x + enemigos[i].ancho/2);
                    float dy = nave.y + nave.largo/2 - (enemigos[i].y + enemigos[i].alto/2);
                    float distancia = sqrt(dx*dx + dy*dy);
                    float rango_vision = 250.0f;
                    
                    if (distancia < rango_vision)
                    {
                        float norm = sqrt(dx*dx + dy*dy);
                        if (norm > 0.1f) 
                        {
                            float velocidad_persecucion = enemigos[i].velocidad * 1.5f;
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
                    float dx = nave.x + nave.ancho/2 - (enemigos[i].x + enemigos[i].ancho/2);
                    float dy = nave.y + nave.largo/2 - (enemigos[i].y + enemigos[i].alto/2);
                    float distancia = sqrt(dx*dx + dy*dy);
                    
                    if (distancia > 10.0f) {
                        float velocidad_kamikaze = enemigos[i].velocidad * 2.0f;
                        enemigos[i].x += (dx / distancia) * velocidad_kamikaze;
                        enemigos[i].y += (dy / distancia) * velocidad_kamikaze;
                    } else {
                        nave.vida -= 35;
                        enemigos[i].activo = false;
                        printf("¡Enemigo kamikaze impactó! Vida restante: %d\n", nave.vida);
                    }
                }
                break;
        }
    }
}


void dibujar_enemigos(Enemigo enemigos[], int num_enemigos)
{
    for (int i = 0; i < num_enemigos; i++)
    {
        if (enemigos[i].activo)
        {
            // Dibujar enemigo base
            al_draw_scaled_bitmap(enemigos[i].imagen, 0, 0, 
                                al_get_bitmap_width(enemigos[i].imagen), 
                                al_get_bitmap_height(enemigos[i].imagen),
                                enemigos[i].x, enemigos[i].y, 
                                enemigos[i].ancho, enemigos[i].alto, 0);
            
            // Overlay de color según tipo de enemigo
            ALLEGRO_COLOR color_overlay;
            bool aplicar_overlay = true;
            
            switch (enemigos[i].tipo) {
                case 0: // Normal - sin overlay
                    aplicar_overlay = false;
                    break;
                case 1: // Perseguidor - tinte azul
                    color_overlay = al_map_rgba(100, 150, 255, 120);
                    break;
                case 2: // Francotirador - tinte rojo
                    color_overlay = al_map_rgba(255, 100, 100, 120);
                    break;
                case 3: // Tanque - tinte verde
                    color_overlay = al_map_rgba(100, 255, 100, 120);
                    break;
                case 4: // Kamikaze - tinte amarillo parpadeante
                    {
                        static int parpadeo = 0;
                        parpadeo = (parpadeo + 1) % 30;
                        if (parpadeo < 15) {
                            color_overlay = al_map_rgba(255, 255, 100, 150);
                        } else {
                            color_overlay = al_map_rgba(255, 200, 0, 150);
                        }
                    }
                    break;
                default:
                    aplicar_overlay = false;
                    break;
            }
            
            // Aplicar overlay de color
            if (aplicar_overlay) {
                al_draw_filled_rectangle(enemigos[i].x, enemigos[i].y, 
                                       enemigos[i].x + enemigos[i].ancho, 
                                       enemigos[i].y + enemigos[i].alto, 
                                       color_overlay);
            }
            
            // Mostrar barra de vida para tanques
            if (enemigos[i].tipo == 3) {
                float porcentaje_vida = (float)enemigos[i].vida / 6.0f;
                
                // Fondo de la barra
                al_draw_filled_rectangle(enemigos[i].x, enemigos[i].y - 8, 
                                       enemigos[i].x + enemigos[i].ancho, enemigos[i].y - 4, 
                                       al_map_rgb(100, 0, 0));
                
                // Vida actual
                al_draw_filled_rectangle(enemigos[i].x, enemigos[i].y - 8, 
                                       enemigos[i].x + (enemigos[i].ancho * porcentaje_vida), 
                                       enemigos[i].y - 4, 
                                       al_map_rgb(255, 0, 0));
                
                // Borde
                al_draw_rectangle(enemigos[i].x, enemigos[i].y - 8, 
                                enemigos[i].x + enemigos[i].ancho, enemigos[i].y - 4, 
                                al_map_rgb(255, 255, 255), 1);
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
    for(int i = 0; i < num_disparos; i++)
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
    for (int i = 0; i < num_disparos; i++)
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
    for(int i = 0; i < num_disparos; i++)
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
    if (nave.nivel_disparo_radial == 0) 
    {
        disparar(disparos, num_disparos, nave);
        return;
    }

    // Calcular el centro de la nave
    float centro_x = nave.x + nave.ancho / 2.0f;
    float centro_y = nave.y + nave.largo / 2.0f;

    int num_disparos_radiales;
    float separacion_angular;

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

    float angulos[5];

    if (num_disparos_radiales == 3)
    {
        // 3 disparos: izquierda, centro, derecha
        angulos[0] = nave.angulo - ALLEGRO_PI/2 - separacion_angular; // Izquierda
        angulos[1] = nave.angulo - ALLEGRO_PI/2;                     // Centro
        angulos[2] = nave.angulo - ALLEGRO_PI/2 + separacion_angular; // Derecha
    }
    else
    {
        for (int j = 0; j < 5; j++)
        {
            int offset = j - 2;
            angulos[j] = nave.angulo - ALLEGRO_PI/2 + offset * separacion_angular;
        }
    }

    // Calcular posición de disparo desde la punta de la nave
    float punta_x = centro_x + cos(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);
    float punta_y = centro_y + sin(nave.angulo - ALLEGRO_PI/2) * (nave.largo / 2.0f);

    int indice_angulo = 0;
    for (int i = 0; i < num_disparos && indice_angulo < num_disparos_radiales; i++)
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
    if (nave->kills_para_mejora >= 16 && nave->nivel_disparo_radial < 2) 
    {
        nave->nivel_disparo_radial++;
        nave->kills_para_mejora = 0;
        
        // Mostrar mensaje de mejora
        char texto_mejora[100];
        sprintf(texto_mejora, "¡Disparo Radial Nivel %d Desbloqueado!", nave->nivel_disparo_radial);
        
        agregar_mensaje_cola(cola_mensajes, texto_mejora, 3.5, al_map_rgb(255, 255, 0), true);

        if (nave->nivel_disparo_radial == 1)
        {
            agregar_mensaje_cola(cola_mensajes, "Ahora disparas 3 proyectiles", 2.5, al_map_rgb(255, 255, 255), true);
        }
        else if (nave->nivel_disparo_radial == 2)
        {
            agregar_mensaje_cola(cola_mensajes, "Ahora disparas 5 proyectiles", 2.5, al_map_rgb(255, 255, 255), true);
        }
        
        printf("¡Mejora de disparo radial! Nivel actual: %d\n", nave->nivel_disparo_radial);
    }
}



void dibujar_nivel_powerup(Nave nave, ALLEGRO_FONT* fuente)
{
    char texto_powerup[50];
    if (nave.nivel_disparo_radial == 0) {
        sprintf(texto_powerup, "Radial: Desactivado (%d/16)", nave.kills_para_mejora);
    } else {
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
    strncpy(mensaje->texto, texto, sizeof(mensaje->texto) - 1);
    mensaje->texto[sizeof(mensaje->texto) - 1] = '\0';
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
    ALLEGRO_COLOR color_alpha = al_map_rgba(r, g, b, (int)(alpha * 255));

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
bool verificar_nivel_completado(Enemigo enemigos[], int num_enemigos)
{
    int i;

    for (i = 0; i < num_enemigos; i++)
    {
        if (enemigos[i].activo) 
        {
            return false; // Si hay al menos un enemigo activo, el nivel no está completo
        }
    }

    return true; // Si no hay enemigos activos, el nivel está completo
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
    // Fondo oscuro con efecto de fade
    float alpha = 200.0f;
    al_draw_filled_rectangle(0, 0, 800, 600, al_map_rgba(0, 0, 0, (int)alpha));
    
    // Calcular el progreso de la animación (0.0 a 1.0)
    float progreso = (float)(tiempo_transcurrido / duracion_total);
    if (progreso > 1.0f) progreso = 1.0f;
    
    // Efecto de aparición/desaparición del texto
    float alpha_texto = 255.0f;
    if (progreso < 0.2f) {
        // Fade in en los primeros 20%
        alpha_texto = 255.0f * (progreso / 0.2f);
    } else if (progreso > 0.8f) {
        // Fade out en los últimos 20%
        alpha_texto = 255.0f * ((1.0f - progreso) / 0.2f);
    }
    
    // Colores con transparencia animada
    ALLEGRO_COLOR color_titulo = al_map_rgba(255, 255, 0, (int)alpha_texto);
    ALLEGRO_COLOR color_subtitulo = al_map_rgba(0, 255, 0, (int)alpha_texto);
    ALLEGRO_COLOR color_progreso = al_map_rgba(255, 255, 255, (int)alpha_texto);
    
    // Textos de transición
    char texto_titulo[100];
    char texto_subtitulo[100];
    char texto_progreso[100];
    
    if (siguiente_nivel <= 3) {
        sprintf(texto_titulo, "¡NIVEL %d COMPLETADO!", nivel_completado);
        sprintf(texto_subtitulo, "Preparando Nivel %d...", siguiente_nivel);
    } else {
        sprintf(texto_titulo, "¡FELICIDADES!");
        sprintf(texto_subtitulo, "¡Has completado todos los niveles!");
    }
    
    sprintf(texto_progreso, "%.0f%%", progreso * 100);
    
    // Dibujar textos centrados
    al_draw_text(fuente, color_titulo, 400, 250, ALLEGRO_ALIGN_CENTER, texto_titulo);
    al_draw_text(fuente, color_subtitulo, 400, 300, ALLEGRO_ALIGN_CENTER, texto_subtitulo);
    
    // Barra de progreso
    float barra_ancho = 300;
    float barra_alto = 20;
    float barra_x = 400 - barra_ancho / 2;
    float barra_y = 350;
    
    // Fondo de la barra
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, al_map_rgba(100, 100, 100, (int)alpha_texto));
    
    // Progreso de la barra
    float progreso_barra = barra_ancho * progreso;
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
    sprintf(nombre_archivo, "Nivel%d.txt", nivel);
    
    // Verificar si el archivo existe
    FILE* test = fopen(nombre_archivo, "r");
    if (!test) {
        printf("No se encontró el archivo %s. Fin del juego.\n", nombre_archivo);
        return false;
    }
    fclose(test);
    
    // Limpiar el tilemap anterior
    for (int f = 0; f < MAPA_FILAS; f++) {
        for (int c = 0; c < MAPA_COLUMNAS; c++) {
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
void actualizar_estado_nivel(EstadoJuego* estado, Enemigo enemigos[], int num_enemigos, double tiempo_actual) {
    // Verificar si todos los enemigos han sido eliminados
    if (!estado->todos_enemigos_eliminados && verificar_nivel_completado(enemigos, num_enemigos)) 
    {
        estado->todos_enemigos_eliminados = true;
        estado->mostrar_transicion = true;
        estado->tiempo_inicio_transicion = tiempo_actual;
        
        printf("¡Nivel %d completado! Iniciando transición...\n", estado->nivel_actual);
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
 * @brief Inicializa un enemigo según su tipo.
 * 
 * @param enemigo Puntero al enemigo a inicializar.
 * @param col Columna en el mapa.
 * @param fila Fila en el mapa.
 * @param tipo Tipo de enemigo (0-4).
 * @param imagen_enemigo Imagen del enemigo.
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
            enemigo->vida = 2;
            enemigo->intervalo_disparo = 2.0 + (rand() % 100) / 100.0;
            break;
            
        case 1: // Perseguidor
            enemigo->ancho = 45;
            enemigo->alto = 35;
            enemigo->velocidad = 0.8f;
            enemigo->vida = 2;
            enemigo->intervalo_disparo = 2.5;
            break;
            
        case 2: // Francotirador
            enemigo->ancho = 40;
            enemigo->alto = 30;
            enemigo->velocidad = 0; // Inmóvil
            enemigo->vida = 1;
            enemigo->intervalo_disparo = 1.5;
            break;
            
        case 3: // Tanque
            enemigo->ancho = 70;
            enemigo->alto = 50;
            enemigo->velocidad = 0.3f;
            enemigo->vida = 6;
            enemigo->intervalo_disparo = 3.0;
            break;
            
        case 4: // Kamikaze
            enemigo->ancho = 35;
            enemigo->alto = 30;
            enemigo->velocidad = 1.5f;
            enemigo->vida = 1;
            enemigo->intervalo_disparo = 999; // No dispara
            break;
            
        default:
            // Valores por defecto
            enemigo->ancho = 50;
            enemigo->alto = 40;
            enemigo->velocidad = 1.0f;
            enemigo->vida = 1;
            enemigo->intervalo_disparo = 2.0;
            break;
    }
}


/**
 * @brief Disparo especializado del francotirador - apunta directamente a la nave.
 * 
 * @param disparos Arreglo de disparos de enemigos.
 * @param num_disparos Número total de disparos.
 * @param enemigo Enemigo francotirador que dispara.
 * @param nave Nave objetivo.
 */
void francotirador_disparar(Disparo disparos[], int num_disparos, Enemigo enemigo, Nave nave)
{
    for(int i = 0; i < num_disparos; i++)
    {
        if (!disparos[i].activo)
        {
            // Calcular ángulo hacia la nave
            float dx = nave.x + nave.ancho/2 - (enemigo.x + enemigo.ancho/2);
            float dy = nave.y + nave.largo/2 - (enemigo.y + enemigo.alto/2);
            float angulo_hacia_nave = atan2(dy, dx);
            
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
 * @brief Disparo especializado del tanque - más lento pero potente.
 * 
 * @param disparos Arreglo de disparos de enemigos.
 * @param num_disparos Número total de disparos.
 * @param enemigo Enemigo tanque que dispara.
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
    for(int i = 0; i < num_disparos && disparos_creados < 3; i++)
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
    return (disparo.x >= tile_x && disparo.x <= tile_x + TILE_ANCHO && 
            disparo.y >= tile_y && disparo.y <= tile_y + TILE_ALTO);
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
            powerups[i].duracion_vida = 15.0;
            powerups[i].color = al_map_rgb(0, 255, 255);
            printf("Powerup de escudo creado en (%.0f, %.0f)\n", x, y);
            break;
        }
    }
}


void actualizar_powerups(Powerup powerups[], int max_powerups, double tiempo_actual)
{
    int i;

    for (i = 0; i < max_powerups; i++)
    {
        if (powerups[i].activo)
        {
            powerups[i].y += 2.0f;

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
        }
    }
}


void dibujar_powerups(Powerup powerups[], int max_powerups)
{
    int i;

    for (i = 0; i < max_powerups; i++)
    {
        if (powerups[i].activo)
        {
            static int parpadeo = 0;
            parpadeo = (parpadeo + 1) % 60;

            ALLEGRO_COLOR color_powerup = powerups[i].color;
            if (parpadeo < 30)
            {
                color_powerup = al_map_rgb(50, 255, 255);
            }
            
            if (powerups[i].tipo == 0)
            {
                float cx = powerups[i].x + 15;
                float cy = powerups[i].y + 15;

                al_draw_filled_circle(cx, cy, 15, al_map_rgba(0, 255, 255, 100)); // Dibujar círculo para el powerup de escudo
                al_draw_circle(cx, cy, 15, color_powerup, 3);

                float hex_x[6];
                float hex_y[6];
                int j;

                for (j = 0; j < 6; j++)
                {
                    hex_x[j] = cx + cos(j * ALLEGRO_PI / 3) * 8;
                    hex_y[j] = cy + sin(j * ALLEGRO_PI / 3) * 8;
                }
                
                for (j = 0; j < 6; j++)
                {
                    al_draw_line(hex_x[j], hex_y[j], hex_x[(j + 1) % 6], hex_y[(j + 1) % 6], color_powerup, 2);
                }
                
                al_draw_line(cx - 4, cy, cx + 4, cy, color_powerup, 2);
                al_draw_line(cx, cy - 4, cx, cy + 4, color_powerup, 2);
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
    if (powerup->tipo == 0) // Escudo
    {
        activar_escudo(&nave->escudo, 3);
        agregar_mensaje_cola(cola_mensajes, "Escudo Activado!", 2.0, al_map_rgb(0, 255, 255), true);
        agregar_mensaje_cola(cola_mensajes,"Resistencia: 3 impactos", 2.0, al_map_rgb(255, 255, 255), true);
        printf("¡Escudo activado activo hasta 3 disparos!\n");
    }
    
    powerup->activo = false;
}


void init_escudo(Escudo* escudo)
{
    escudo->activo = false;
    escudo->hits_max = 0;
    escudo->hits_restantes = 0;
    escudo->tiempo_activacion = 0;
    escudo->color = al_map_rgba(0, 255, 255, 150);
    escudo->intensidad = 1.0f;
}


void activar_escudo(Escudo* escudo, int hits_maximos)
{
    escudo->activo = true;
    escudo->hits_max = hits_maximos;
    escudo->hits_restantes = hits_maximos;
    escudo->tiempo_activacion = al_get_time();
    escudo->intensidad = 1.0f;
}


void actualizar_escudo(Escudo *escudo, double tiempo_actual)
{
    if (escudo->activo)
    {
        if (escudo->hits_restantes <= 0)
        {
            escudo->activo = false;
            printf("Escudo desactivado\n");
        }
        else
        {
            if (escudo->hits_restantes == 1)
            {
                escudo->intensidad = 0.3f + 0.7f * sin(tiempo_actual * 15.0f);
            }
            else if (escudo->hits_restantes == 2)
            {
                escudo->intensidad = 0.5f + 0.3f * sin(tiempo_actual * 8.0f);
            }
            else
            {
                escudo->intensidad = 0.8f + 0.2f * sin(tiempo_actual * 3.0f);
            }
        }
    }
}


void dibujar_escudo(Nave nave)
{
    if (nave.escudo.activo)
    {
        float cx = nave.x + nave.ancho / 2;
        float cy = nave.y + nave.largo / 2;
        float radio = (nave.ancho + nave.largo) / 3.0f + 10;
        
        // cambio de color en base al daño que recibe el escudo
        ALLEGRO_COLOR color_base;
        if (nave.escudo.hits_restantes >= 3)
        {
            color_base = al_map_rgb(0, 255, 255);      // Celeste (Completo)
        }
        else if (nave.escudo.hits_restantes == 2)
        {
            color_base = al_map_rgb(255, 255, 0);      // Amarillo (medio)
        }
        else
        {
            color_base = al_map_rgb(255, 100, 100);    // Rojo (crítico)
        }
        
        // CALCULAR COLOR CON INTENSIDAD
        int alpha = (int)(255 * nave.escudo.intensidad * 0.6f);
        
        ALLEGRO_COLOR color_escudo;
        if (nave.escudo.hits_restantes >= 3) {
            // Cian con transparencia
            color_escudo = al_map_rgba(0, 255, 255, alpha);
        } else if (nave.escudo.hits_restantes == 2) {
            // Amarillo con transparencia
            color_escudo = al_map_rgba(255, 255, 0, alpha);
        } else {
            // Rojo con transparencia
            color_escudo = al_map_rgba(255, 100, 100, alpha);
        }
        
        // Círculo principal del escudo
        al_draw_circle(cx, cy, radio, color_escudo, 3);
        al_draw_circle(cx, cy, radio - 3, al_map_rgba(100, 200, 255, alpha/2), 1);
        
        // Efecto de hexágono rotatorio
        float hex_x[6], hex_y[6];
        for (int i = 0; i < 6; i++) 
        {
            hex_x[i] = cx + cos(i * ALLEGRO_PI / 3 + al_get_time()) * (radio - 5);
            hex_y[i] = cy + sin(i * ALLEGRO_PI / 3 + al_get_time()) * (radio - 5);
        }
        
        ALLEGRO_COLOR color_hexagono;
        if (nave.escudo.hits_restantes >= 3) {
            color_hexagono = al_map_rgba(0, 255, 255, alpha/3);
        } else if (nave.escudo.hits_restantes == 2) {
            color_hexagono = al_map_rgba(255, 255, 0, alpha/3);
        } else {
            color_hexagono = al_map_rgba(255, 100, 100, alpha/3);
        }
        
        for (int i = 0; i < 6; i++) {
            al_draw_line(hex_x[i], hex_y[i], hex_x[(i+1)%6], hex_y[(i+1)%6], color_hexagono, 2);
        }
        
        // MOSTRAR HITS RESTANTES EN LUGAR DE TIEMPO
        float barra_ancho = 60;
        float barra_alto = 8;
        float barra_x = cx - barra_ancho / 2;
        float barra_y = cy + radio + 15;
        
        // Dibujar hits como segmentos
        float segment_width = barra_ancho / nave.escudo.hits_max;
        
        // Fondo de la barra
        al_draw_filled_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                                al_map_rgba(0, 0, 0, 150));
        
        // Dibujar segmentos activos
        for (int i = 0; i < nave.escudo.hits_restantes; i++) {
            float seg_x = barra_x + (i * segment_width);
            al_draw_filled_rectangle(seg_x + 1, barra_y + 1, seg_x + segment_width - 1, barra_y + barra_alto - 1, 
                                    color_base);
        }
        
        // Borde
        al_draw_rectangle(barra_x, barra_y, barra_x + barra_ancho, barra_y + barra_alto, 
                         al_map_rgb(255, 255, 255), 1);
    }
}


bool escudo_activo(Nave nave)
{
    return nave.escudo.activo;
}


bool escudo_recibir_dano(Escudo* escudo)
{
    if (!escudo->activo) return false;
    
    escudo->hits_restantes--;
    printf("¡Escudo impactado! Hits restantes: %d\n", escudo->hits_restantes);
    
    if (escudo->hits_restantes <= 0)
    {
        escudo->activo = false;
        printf("¡Escudo destruido!\n");
    }
    
    return true; // Indica que el escudo absorbió el daño
}


bool verificar_colision_nave_muro(float x, float y, float ancho, float largo)
{
    // Obtener el tilemap global (necesitaremos pasarlo como parámetro)
    extern Tile tilemap_global[MAPA_FILAS][MAPA_COLUMNAS];
    
    // Calcular qué tiles ocupa la nave
    int col_izquierda = (int)(x / TILE_ANCHO);
    int col_derecha = (int)((x + ancho - 1) / TILE_ANCHO);
    int fila_superior = (int)(y / TILE_ALTO);
    int fila_inferior = (int)((y + largo - 1) / TILE_ALTO);

    // Verificar todos los tiles que ocuparía la nave
    for (int fila = fila_superior; fila <= fila_inferior; fila++)
    {
        for (int col = col_izquierda; col <= col_derecha; col++)
        {
            // Verificar si el tile está dentro del mapa
            if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS)
            {
                // Verificar colisión con muros indestructibles (tipo 3), escudos (tipo 2) o asteroides fijos (tipo 1)
                if (tilemap_global[fila][col].tipo == 3 || 
                    (tilemap_global[fila][col].tipo == 2 && tilemap_global[fila][col].vida > 0) ||
                    tilemap_global[fila][col].tipo == 1)
                {
                    printf("¡Colisión detectada con tile tipo %d en (%d, %d)!\n", tilemap_global[fila][col].tipo, col, fila);
                    return true; // Hay colisión
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
        printf("¡Cola de mensajes llena! No se puede agregar el mensaje: %s\n", texto);
        return;
    }

    size_t max_len = sizeof(cola->mensajes[cola->fin].texto) - 1;
    strncpy(cola->mensajes[cola->fin].texto, texto, max_len);
    cola->mensajes[cola->fin].texto[max_len] = '\0';
    cola->mensajes[cola->fin].duracion = duracion;
    cola->mensajes[cola->fin].color = color;
    cola->mensajes[cola->fin].centrado = centrado;

    cola->fin = (cola->fin + 1) % MAX_COLA_MENSAJES;
    cola->cantidad++;
    
    printf("Mensaje agregado a la cola: %s (Total en cola: %d)\n", texto, cola->cantidad);
}


void actualizar_cola_mensajes(ColaMensajes* cola, double tiempo_actual)
{
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
        
        static int debug_counter = 0;
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
    strncpy(mensaje->texto, texto, sizeof(mensaje->texto) - 1);
    mensaje->texto[sizeof(mensaje->texto) - 1] = '\0';
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
void dibujar_hitboxes_debug(Nave nave, Enemigo enemigos[], int num_enemigos, Disparo disparos[], int num_disparos, Disparo disparos_enemigos[], int num_disparos_enemigos, Asteroide asteroides[], int num_asteroides, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS])
{
    static ALLEGRO_FONT* fuente_debug = NULL;
    if (!fuente_debug) 
    {
        fuente_debug = al_create_builtin_font();
    }

    // Hitbox de la nave - Verde
    al_draw_rectangle(nave.x, nave.y, nave.x + nave.ancho, nave.y + nave.largo, al_map_rgb(0, 255, 0), 2);
    
    // Centro de la nave - punto verde
    float centro_nave_x = nave.x + nave.ancho / 2;
    float centro_nave_y = nave.y + nave.largo / 2;
    al_draw_filled_circle(centro_nave_x, centro_nave_y, 3, al_map_rgb(0, 255, 0));
    
    // Hitbox del escudo si está activo - Cian
    if (nave.escudo.activo)
    {
        float radio_escudo = (nave.ancho + nave.largo) / 3.0f + 10;
        al_draw_circle(centro_nave_x, centro_nave_y, radio_escudo, al_map_rgb(0, 255, 255), 1);
    }

    // Hitboxes de enemigos - Rojo con diferentes tonos según tipo
    for (int i = 0; i < num_enemigos; i++)
    {
        if (enemigos[i].activo)
        {
            ALLEGRO_COLOR color_enemigo;
            switch (enemigos[i].tipo)
            {
                case 0: color_enemigo = al_map_rgb(255, 0, 0); break;     // Normal - rojo
                case 1: color_enemigo = al_map_rgb(255, 100, 0); break;   // Perseguidor - naranja
                case 2: color_enemigo = al_map_rgb(255, 0, 100); break;   // Francotirador - magenta
                case 3: color_enemigo = al_map_rgb(200, 0, 0); break;     // Tanque - rojo oscuro
                case 4: color_enemigo = al_map_rgb(255, 200, 0); break;   // Kamikaze - amarillo
                default: color_enemigo = al_map_rgb(255, 0, 0); break;
            }
            
            al_draw_rectangle(enemigos[i].x, enemigos[i].y, 
                            enemigos[i].x + enemigos[i].ancho, enemigos[i].y + enemigos[i].alto, 
                            color_enemigo, 2);
            
            // Centro del enemigo
            float centro_x = enemigos[i].x + enemigos[i].ancho / 2;
            float centro_y = enemigos[i].y + enemigos[i].alto / 2;
            al_draw_filled_circle(centro_x, centro_y, 2, color_enemigo);
            
            // Texto con tipo de enemigo
            char tipo_texto[20];
            sprintf(tipo_texto, "T%d", enemigos[i].tipo);
            al_draw_text(fuente_debug, al_map_rgb(255, 255, 255), 
                        centro_x, centro_y - 15, ALLEGRO_ALIGN_CENTER, tipo_texto);
        }
    }

    // Hitboxes de disparos del jugador - Azul
    for (int i = 0; i < num_disparos; i++)
    {
        if (disparos[i].activo)
        {
            al_draw_rectangle(disparos[i].x, disparos[i].y, 
                            disparos[i].x + 5, disparos[i].y + 10, 
                            al_map_rgb(0, 0, 255), 1);
        }
    }

    // Hitboxes de disparos de enemigos - Amarillo
    for (int i = 0; i < num_disparos_enemigos; i++)
    {
        if (disparos_enemigos[i].activo)
        {
            al_draw_rectangle(disparos_enemigos[i].x, disparos_enemigos[i].y, 
                            disparos_enemigos[i].x + 4, disparos_enemigos[i].y + 8, 
                            al_map_rgb(255, 255, 0), 1);
        }
    }

    // Hitboxes de asteroides - Blanco
    for (int i = 0; i < num_asteroides; i++)
    {
        al_draw_rectangle(asteroides[i].x, asteroides[i].y, 
                        asteroides[i].x + asteroides[i].ancho, asteroides[i].y + asteroides[i].alto, 
                        al_map_rgb(255, 255, 255), 1);
    }

    // Hitboxes del tilemap - Varios colores
    for (int fila = 0; fila < MAPA_FILAS; fila++)
    {
        for (int col = 0; col < MAPA_COLUMNAS; col++)
        {
            if (tilemap[fila][col].tipo > 0)
            {
                float x = col * TILE_ANCHO;
                float y = fila * TILE_ALTO;
                ALLEGRO_COLOR color_tile;
                
                switch (tilemap[fila][col].tipo)
                {
                    case 1: color_tile = al_map_rgb(128, 128, 128); break; // Asteroide fijo - gris
                    case 2: color_tile = al_map_rgb(0, 128, 255); break;   // Escudo - azul
                    case 3: color_tile = al_map_rgb(80, 80, 80); break;    // Bloque sólido - gris oscuro
                    default: color_tile = al_map_rgb(255, 255, 255); break;
                }
                
                al_draw_rectangle(x, y, x + TILE_ANCHO, y + TILE_ALTO, color_tile, 1);
                
                // Mostrar vida del tile si es aplicable
                if (tilemap[fila][col].tipo == 2 && tilemap[fila][col].vida > 0)
                {
                    char vida_texto[12]; // Buffer más grande
                    sprintf(vida_texto, "%d", tilemap[fila][col].vida);
                    al_draw_text(fuente_debug, al_map_rgb(255, 255, 255), 
                               x + TILE_ANCHO/2, y + TILE_ALTO/2, ALLEGRO_ALIGN_CENTER, vida_texto);
                }
            }
        }
    }

    // Leyenda en la esquina superior derecha
    int leyenda_x = 600;
    int leyenda_y = 10;
    
    al_draw_text(fuente_debug, al_map_rgb(255, 255, 255), leyenda_x, leyenda_y, ALLEGRO_ALIGN_LEFT, "DEBUG HITBOXES:");
    al_draw_text(fuente_debug, al_map_rgb(0, 255, 0), leyenda_x, leyenda_y + 15, ALLEGRO_ALIGN_LEFT, "Verde: Nave");
    al_draw_text(fuente_debug, al_map_rgb(255, 0, 0), leyenda_x, leyenda_y + 30, ALLEGRO_ALIGN_LEFT, "Rojo: Enemigos");
    al_draw_text(fuente_debug, al_map_rgb(0, 0, 255), leyenda_x, leyenda_y + 45, ALLEGRO_ALIGN_LEFT, "Azul: Disparos nave");
    al_draw_text(fuente_debug, al_map_rgb(255, 255, 0), leyenda_x, leyenda_y + 60, ALLEGRO_ALIGN_LEFT, "Amarillo: Disparos enemigos");
    al_draw_text(fuente_debug, al_map_rgb(255, 255, 255), leyenda_x, leyenda_y + 75, ALLEGRO_ALIGN_LEFT, "Blanco: Asteroides");
    al_draw_text(fuente_debug, al_map_rgb(0, 128, 255), leyenda_x, leyenda_y + 90, ALLEGRO_ALIGN_LEFT, "Azul claro: Escudos");
}


/**
 * @brief Limpia recursos y memoria del juego.
 */
void limpiar_memoria_juego(Disparo disparos[], int num_disparos, Disparo disparos_enemigos[], int num_disparos_enemigos, Powerup powerups[], int max_powerups, Enemigo enemigos[], int num_enemigos, ColaMensajes* cola_mensajes)
{
    // Limpiar disparos
    for (int i = 0; i < num_disparos; i++) {
        disparos[i].activo = false;
    }
    
    for (int i = 0; i < num_disparos_enemigos; i++) {
        disparos_enemigos[i].activo = false;
    }
    
    // Limpiar powerups
    for (int i = 0; i < max_powerups; i++) {
        powerups[i].activo = false;
    }
    
    // Limpiar enemigos
    for (int i = 0; i < num_enemigos; i++) {
        enemigos[i].activo = false;
    }
    
    // Limpiar cola de mensajes
    if (cola_mensajes) {
        cola_mensajes->cantidad = 0;
        cola_mensajes->inicio = 0;
        cola_mensajes->fin = 0;
        cola_mensajes->mensaje_actual.activo = false;
        cola_mensajes->procesando = false;
    }
    
    printf("Memoria del juego limpiada.\n");
}