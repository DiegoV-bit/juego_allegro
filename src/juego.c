#include "juego.h"

/**
 * @file juego.c
 * @brief Este archivo contiene todas las funciones referentes al juego.
 *
*/

/**
 *@brief Inicializa la nave.
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
    for (int i = 0; i < num_asteroides; i++)
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
    asteroide->y += asteroide->velocidad;

    // Verifica colisión con la nave
    float centro_nave_x = nave->x + nave->ancho / 2;
    float centro_nave_y = nave->y + nave->largo / 2;
    float radio_nave = nave->ancho / 2.0f;
    float centro_asteroide_x = asteroide->x + asteroide->ancho / 2;
    float centro_asteroide_y = asteroide->y + asteroide->alto / 2;
    float radio_asteroide = asteroide->ancho / 2.0f;

    if (detectar_colision_circular(centro_nave_x, centro_nave_y, radio_nave, centro_asteroide_x, centro_asteroide_y, radio_asteroide)) {
        // Asteroide impacta la nave: desaparece
        asteroide->y = -asteroide->alto;
        asteroide->x = rand() % (800 - (int)asteroide->ancho);
        nave->vida -= 10;
        return;
    }

    // Verifica colisión con escudos del tilemap (MEJORADO)
    int col_izquierda = (int)(asteroide->x / TILE_ANCHO);
    int col_derecha = (int)((asteroide->x + asteroide->ancho - 1) / TILE_ANCHO);
    int fila_superior = (int)(asteroide->y / TILE_ALTO);
    int fila_inferior = (int)((asteroide->y + asteroide->alto - 1) / TILE_ALTO);

    // Verifica TODAS las celdas que ocupa el asteroide
    for (int fila = fila_superior; fila <= fila_inferior; fila++) {
        for (int col = col_izquierda; col <= col_derecha; col++) {
            if (fila >= 0 && fila < MAPA_FILAS && col >= 0 && col < MAPA_COLUMNAS) {
                if (tilemap[fila][col].tipo == 2 && tilemap[fila][col].vida > 0) {
                    tilemap[fila][col].vida--;
                    if (tilemap[fila][col].vida <= 0) {
                        tilemap[fila][col].tipo = 0; // El escudo se destruye
                    }
                    // Asteroide desaparece
                    asteroide->y = -asteroide->alto;
                    asteroide->x = rand() % (800 - (int)asteroide->ancho);
                    return;
                }
                
                // Colisión con asteroide fijo (tipo 1)
                if (tilemap[fila][col].tipo == 1) {
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
            disparar(disparos, num_disparos, *nave);
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
void dibujar_juego(Nave nave, Asteroide asteroides[], int num_asteroides)
{
    // al_draw_bitmap(imagen_fondo, 0, 0, 0);

    float cx = al_get_bitmap_width(nave.imagen) / 2.0f;
    float cy = al_get_bitmap_height(nave.imagen) / 2.0f;
    float escala_x = nave.ancho / al_get_bitmap_width(nave.imagen);
    float escala_y = nave.largo / al_get_bitmap_height(nave.imagen);
    al_draw_scaled_rotated_bitmap(nave.imagen, cx, cy, nave.x + nave.ancho / 2, nave.y + nave.largo / 2, escala_x, escala_y, nave.angulo, 0);

    for (int i = 0; i < num_asteroides; i++)
    {
        al_draw_scaled_bitmap(asteroides[i].imagen, 0, 0, al_get_bitmap_width(asteroides[i].imagen), al_get_bitmap_height(asteroides[i].imagen),
                              asteroides[i].x, asteroides[i].y, asteroides[i].ancho, asteroides[i].alto, 0);
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
    // Rotación de la nave
    if(teclas[2]) nave->angulo -= 0.07f; // Izquierda
    if(teclas[3]) nave->angulo += 0.07f; // Derecha

    if (teclas[0]) // Arriba (avanzar)
    {
        nave->x += cos(nave->angulo - ALLEGRO_PI/2) * 5;
        nave->y += sin(nave->angulo - ALLEGRO_PI/2) * 5;
    }
    if (teclas[1]) // Abajo (retroceder, opcional)
    {
        nave->x -= cos(nave->angulo - ALLEGRO_PI/2) * 2.5; // Retrocede más lento
        nave->y -= sin(nave->angulo - ALLEGRO_PI/2) * 2.5;
    }

    // Limitar el movimiento de la nave dentro de la ventana
    if (nave->x < 0) nave->x = 0;
    if (nave->x > 800 - nave->ancho) nave->x = 800 - nave->ancho;
    if (nave->y < 0) nave->y = 0;
    if (nave->y > 600 - nave->largo) nave->y = 600 - nave->largo;

    for (int i = 0; i < NUM_ASTEROIDES; i++)
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
    for (int i = 0; i < num_disparos; i++)
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
    for (int i = 0; i < num_disparos; i++)
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
    for (int i = 0; i < num_disparos; i++)
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
void actualizar_juego(Nave* nave, bool teclas[], Asteroide asteroides[], int num_asteroides, Disparo disparos[], int num_disparos, int* puntaje, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos[], int num_enemigos, Disparo disparos_enemigos[], int num_disparos_enemigos)
{
    double tiempo_actual = al_get_time();

    actualizar_nave(nave, teclas, asteroides, tiempo_actual);
    actualizar_disparos(disparos, num_disparos);
    actualizar_enemigos(enemigos, num_enemigos, disparos_enemigos, num_disparos_enemigos, *nave, tiempo_actual);
    actualizar_disparos_enemigos(disparos_enemigos, num_disparos_enemigos);

    for (int i = 0; i < num_asteroides; i++)
    {
        actualizar_asteroide(&asteroides[i], tilemap, nave);
        for (int j = 0; j < num_disparos; j++)
        {
            if (disparos[j].activo && detectar_colision_disparo(asteroides[i], disparos[j]))
            {
                disparos[j].activo = false;
                asteroides[i].y = -asteroides[i].alto;
                asteroides[i].x = rand() % (800 - (int)asteroides[i].ancho);
                (*puntaje)++;
            }
        }
        //detectar_colision(nave, asteroides[i]);
    }

    // Colisiones con enemigos
    for (int i = 0; i < num_enemigos; i++)
    {
        if (!enemigos[i].activo) continue;
        
        // Disparos del jugador vs enemigos (USA LA NUEVA FUNCIÓN)
        for (int j = 0; j < num_disparos; j++)
        {
            if (disparos[j].activo && detectar_colision_disparo_enemigo(disparos[j], enemigos[i]))
            {
                disparos[j].activo = false;
                enemigos[i].vida--;
                if (enemigos[i].vida <= 0)
                {
                    enemigos[i].activo = false;
                    (*puntaje) += 5; // Más puntos por destruir enemigo
                }
                break;
            }
        }

        // Nave vs enemigos (USA LA NUEVA FUNCIÓN)
        if (detectar_colision_nave_enemigo(*nave, enemigos[i]))
        {
            nave->vida -= 20; // Daño por tocar enemigo
            enemigos[i].activo = false; // El enemigo también se destruye
        }
    }

    // Disparos de enemigos vs nave (USA LA NUEVA FUNCIÓN)
    for (int i = 0; i < num_disparos_enemigos; i++)
    {
        if (disparos_enemigos[i].activo && detectar_colision_disparo_enemigo_nave(*nave, disparos_enemigos[i]))
        {
            disparos_enemigos[i].activo = false;
            nave->vida -= 15; // Los enemigos hacen más daño
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
 * @brief Carga un tilemap desde un archivo de texto.
 * 
 * @param filename Nombre del archivo del nivel.
 * @param tilemap Matriz de tiles a llenar.
 */
void cargar_tilemap(const char* filename, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos[], int* num_enemigos, ALLEGRO_BITMAP* imagen_enemigo) {
    FILE* archivo = fopen(filename, "r");
    if (!archivo) {
        fprintf(stderr, "No se pudo abrir el archivo del tilemap.\n");
        *num_enemigos = 0;
        return;
    }
    char linea[MAPA_COLUMNAS + 2]; // +2 por '\n' y '\0'
    *num_enemigos = 0;

    for (int fila = 0; fila < MAPA_FILAS; fila++) {
        if (fgets(linea, sizeof(linea), archivo)) {
            for (int col = 0; col < MAPA_COLUMNAS; col++) {
                int tipo = linea[col] - '0';

                if(tipo == 3)
                {
                    if (*num_enemigos < NUM_ENEMIGOS) {
                        enemigos[*num_enemigos].x = col * TILE_ANCHO;
                        enemigos[*num_enemigos].y = fila * TILE_ALTO;
                        enemigos[*num_enemigos].ancho = 60;
                        enemigos[*num_enemigos].alto = 40;
                        enemigos[*num_enemigos].velocidad = 0.5f + (rand() % 100) / 100.0f;
                        enemigos[*num_enemigos].vida = 2;
                        enemigos[*num_enemigos].activo = true;
                        enemigos[*num_enemigos].ultimo_disparo = 0;
                        enemigos[*num_enemigos].intervalo_disparo = 1.5 + (rand() % 200) / 100.0;
                        enemigos[*num_enemigos].imagen = imagen_enemigo;
                        (*num_enemigos)++;
                    }                    

                    // En el tilemap, marcamos como vacío (el enemigo ya está en el arreglo)
                    tilemap[fila][col].tipo = 0;
                    tilemap[fila][col].vida = 0;
                }
                else
                {
                    // Procesar otros tipos de tiles
                    tilemap[fila][col].tipo = tipo;
                    if (tilemap[fila][col].tipo == 2) 
                    {
                        tilemap[fila][col].vida = 3; // Escudos con 3 vidas
                    }
                    else
                    {
                        tilemap[fila][col].vida = 0;
                    }
                }
            }
        }
    }
    fclose(archivo);
}


/**
 * @brief Dibuja el tilemap en pantalla.
 * 
 * @param tilemap Matriz de tiles.
 * @param imagen_asteroide Imagen para los tiles de tipo asteroide.
 */
void dibujar_tilemap(Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], ALLEGRO_BITMAP* imagen_asteroide) {
    for (int fila = 0; fila < MAPA_FILAS; fila++) {
        for (int col = 0; col < MAPA_COLUMNAS; col++) {
            if (tilemap[fila][col].tipo == 1) {
                al_draw_scaled_bitmap(
                    imagen_asteroide,
                    0, 0,
                    al_get_bitmap_width(imagen_asteroide), al_get_bitmap_height(imagen_asteroide),
                    col * TILE_ANCHO, fila * TILE_ALTO,
                    TILE_ANCHO, TILE_ALTO,
                    0
                );
            } else if (tilemap[fila][col].tipo == 2) {
                // Dibuja el escudo como un rectángulo azul (puedes usar una imagen si prefieres)
                ALLEGRO_COLOR color = al_map_rgb(0, 128, 255);
                if (tilemap[fila][col].vida == 2) color = al_map_rgb(0, 200, 255);
                if (tilemap[fila][col].vida == 1) color = al_map_rgb(100, 100, 255);
                al_draw_filled_rectangle(
                    col * TILE_ANCHO, fila * TILE_ALTO,
                    (col + 1) * TILE_ANCHO, (fila + 1) * TILE_ALTO,
                    color
                );
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


void actualizar_enemigos(Enemigo enemigos[], int num_enemigos, Disparo disparos_enemigos[], int num_disparos_enemigos, Nave nave, double tiempo_actual)
{
    for (int i = 0; i < num_enemigos; i++)
    {
        if (!enemigos[i].activo) continue;
        
        // Movimiento horizontal (de lado a lado)
        enemigos[i].x += enemigos[i].velocidad;
        
        // Rebota en los bordes
        if (enemigos[i].x <= 0 || enemigos[i].x >= 800 - enemigos[i].ancho)
        {
            enemigos[i].velocidad *= -1;
        }
        
        // Disparar hacia la nave
        if (tiempo_actual - enemigos[i].ultimo_disparo >= enemigos[i].intervalo_disparo)
        {
            enemigo_disparar(disparos_enemigos, num_disparos_enemigos, enemigos[i]);
            enemigos[i].ultimo_disparo = tiempo_actual;
        }
    }
}



void dibujar_enemigos(Enemigo enemigos[], int num_enemigos)
{
    for (int i = 0; i < num_enemigos; i++)
    {
        if (enemigos[i].activo)
        {
            al_draw_scaled_bitmap(enemigos[i].imagen, 0, 0, 
                                al_get_bitmap_width(enemigos[i].imagen), 
                                al_get_bitmap_height(enemigos[i].imagen),
                                enemigos[i].x, enemigos[i].y, 
                                enemigos[i].ancho, enemigos[i].alto, 0);
        }
    }
}


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
            disparos[i].velocidad = 3.0f;
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


/**
 * @brief Inicializa todos los elementos del juego.
 * 
 * @param nave Puntero a la nave
 * @param asteroides Arreglo de asteroides
 * @param disparos Arreglo de disparos del jugador
 * @param enemigos Arreglo de enemigos
 * @param disparos_enemigos Arreglo de disparos de enemigos
 * @param enemigos_mapa Enemigos cargados del mapa
 * @param num_enemigos_cargados Número de enemigos cargados
 * @param imagen_nave Imagen de la nave
 * @param imagen_asteroide Imagen del asteroide
 */
void inicializar_elementos_juego(Nave* nave, Asteroide asteroides[], Disparo disparos[], 
                                Enemigo enemigos[], Disparo disparos_enemigos[],
                                Enemigo enemigos_mapa[], int num_enemigos_cargados,
                                ALLEGRO_BITMAP* imagen_nave, ALLEGRO_BITMAP* imagen_asteroide)
{
    // Inicializar asteroides
    init_asteroides(asteroides, NUM_ASTEROIDES, 800, imagen_asteroide);

    // Inicializar nave
    *nave = init_nave(400, 500, 50, 50, 100, 0.1, imagen_nave);

    // Inicializar disparos del jugador
    init_disparos(disparos, 10);

    // Inicializar enemigos desde el tilemap
    for (int i = 0; i < num_enemigos_cargados && i < NUM_ENEMIGOS; i++) {
        enemigos[i] = enemigos_mapa[i];
    }
    for (int i = num_enemigos_cargados; i < NUM_ENEMIGOS; i++) {
        enemigos[i].activo = false;
    }
    
    // Inicializar disparos de enemigos
    init_disparos(disparos_enemigos, NUM_DISPAROS_ENEMIGOS);
}

/**
 * @brief Maneja el menú principal del juego.
 * 
 * @param en_menu Puntero a la variable que controla si está en el menú
 * @param jugando Puntero a la variable que controla si está jugando
 * @param mostrarRanking Puntero a la variable que controla si muestra el ranking
 * @param cola_eventos Cola de eventos
 * @param fuente Fuente para el texto
 */
void manejar_menu(bool* en_menu, bool* jugando, bool* mostrarRanking, 
                  ALLEGRO_EVENT_QUEUE* cola_eventos, ALLEGRO_FONT* fuente)
{
    Boton botones[3];
    init_botones(botones);
    
    int cursor_x = 0;
    int cursor_y = 0;

    while (*en_menu)
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            *en_menu = false;
            *jugando = false;
            *mostrarRanking = false;
            break;
        }
    
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            int boton_clicado = detectar_click(botones, 3, evento.mouse.x, evento.mouse.y);
            if (boton_clicado == 0) // Jugar
            {
                *en_menu = false;
                *jugando = true;
            }
            else if (boton_clicado == 1) // Ranking
            {
                *en_menu = false;
                *mostrarRanking = true;
            }
            else if (boton_clicado == 2) // Salir
            {
                *en_menu = false;
                *jugando = false;
                *mostrarRanking = false;
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
}

/**
 * @brief Ejecuta el bucle principal del juego.
 * 
 * @param jugando Puntero a la variable que controla el estado del juego
 * @param volver_menu Puntero a la variable que indica si debe volver al menú
 * @param cola_eventos Cola de eventos
 * @param fuente Fuente para el texto
 * @param tilemap Matriz del tilemap
 * @param enemigos_mapa Enemigos cargados del mapa
 * @param num_enemigos_cargados Número de enemigos cargados
 * @param imagen_nave Imagen de la nave
 * @param imagen_asteroide Imagen del asteroide
 */
void ejecutar_juego(bool* jugando, bool* volver_menu, ALLEGRO_EVENT_QUEUE* cola_eventos, 
                   ALLEGRO_FONT* fuente, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS],
                   Enemigo enemigos_mapa[], int num_enemigos_cargados,
                   ALLEGRO_BITMAP* imagen_nave, ALLEGRO_BITMAP* imagen_asteroide)
{
    bool teclas[ALLEGRO_KEY_MAX] = {false};
    
    // Variables del juego
    Asteroide asteroides[NUM_ASTEROIDES];
    Nave nave;
    Disparo disparos[10];
    Enemigo enemigos[NUM_ENEMIGOS];
    Disparo disparos_enemigos[NUM_DISPAROS_ENEMIGOS];
    int puntaje = 0;

    // Inicializar elementos del juego
    inicializar_elementos_juego(&nave, asteroides, disparos, enemigos, disparos_enemigos,
                               enemigos_mapa, num_enemigos_cargados, imagen_nave, imagen_asteroide);

    // Bucle principal del juego
    while (*jugando) 
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            *jugando = false;
            *volver_menu = false;
            break;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN || evento.type == ALLEGRO_EVENT_KEY_UP)
        {
            manejar_eventos(evento, &nave, teclas, disparos, 10);
        }

        if (evento.type == ALLEGRO_EVENT_TIMER)
        {
            // Actualizar juego
            actualizar_juego(&nave, teclas, asteroides, NUM_ASTEROIDES, disparos, 10, &puntaje, 
                           tilemap, enemigos, num_enemigos_cargados, disparos_enemigos, NUM_DISPAROS_ENEMIGOS);
            
            // Dibujar todo
            al_clear_to_color(al_map_rgb(0, 0, 0));
            dibujar_tilemap(tilemap, imagen_asteroide);
            dibujar_juego(nave, asteroides, NUM_ASTEROIDES);
            dibujar_disparos(disparos, 10);
            dibujar_enemigos(enemigos, num_enemigos_cargados);
            dibujar_disparos_enemigos(disparos_enemigos, NUM_DISPAROS_ENEMIGOS);
            dibujar_puntaje(puntaje, fuente);
            dibujar_barra_vida(nave);
            al_flip_display();

            // Verificar fin del juego
            if (nave.vida <= 0)
            {
                *jugando = false;
            }
        }
    }
    
    // Si la nave murió, capturar nombre y guardar puntaje
    if (nave.vida <= 0)
    {
        char nombre_jugador[MAX_NOMBRE];
        capturar_nombre(fuente, nombre_jugador, cola_eventos);
        guardar_puntaje(nombre_jugador, puntaje);
        *volver_menu = true;
    }
}

/**
 * @brief Maneja la pantalla de ranking.
 * 
 * @param mostrarRanking Puntero a la variable que controla si muestra el ranking
 * @param en_menu Puntero a la variable que controla si está en el menú
 * @param fuente Fuente para el texto
 */
void manejar_ranking(bool* mostrarRanking, bool* en_menu, ALLEGRO_FONT* fuente)
{
    Jugador ranking[MAX_JUGADORES];
    int num_jugadores;
    bool volver_menu_ranking = false;
    
    cargar_ranking(ranking, &num_jugadores);
    mostrar_ranking(fuente, ranking, num_jugadores, &volver_menu_ranking);
    
    if (volver_menu_ranking)
    {
        *mostrarRanking = false;
        *en_menu = true;
    }
}