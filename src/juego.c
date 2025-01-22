#include "juego.h"

/**
 * @file juego.c
 * @brief Este archivo contiene todas las funciones referentes al juego.
 *
*/

/**
 *@brief Inicializa la nave.
 *
 * Esta funcion Inicializa la nave dandole una posicion en los ejes "x" e "y", al igual que le da un tamano
 * 
 * @param x Posicion en el eje x.
 * @param y posicion en el eje y.
 * @param ancho Anchura de la nave.
 * @param largo Largo de la nave. 
 * @param vida Vida de la nave.
 * @param tiempo_invulnerable Tiempo que la nave es invulnerable.
 * @return Nave inicializada.
 */
Nave init_nave(float x, float y, float ancho, float largo, int vida, double tiempo_invulnerable)
{
    Nave nave;
    nave.x = x;
    nave.y = y;
    nave.ancho = ancho;
    nave.largo = largo;
    nave.vida = vida;
    nave.tiempo_invulnerable = tiempo_invulnerable;
    nave.tiempo_ultimo_dano = -nave.tiempo_invulnerable;
    return nave;
}

/**
 * @brief Inicializa los asteroides
 * 
 * Esta funcion inicializa los asteroides dandole una posicion en los ejes "x" e "y", al igual que le da un tamano
 * 
 * @param asteroides Arreglo de asteroides a inicializar.
 * @param num_asteroides Numero de asteroides en el arreglo.
 * @param ancho Anchura de los asteroides.
 */ 
void init_asteroides(Asteroide asteroides[], int num_asteroides, int ancho_ventana)
{
    for (int i = 0; i < num_asteroides; i++)
    {
        asteroides[i].x = rand() % (ancho_ventana - 50);
        asteroides[i].y = rand() % 600 - 600;
        asteroides[i].velocidad = 1.5;
        asteroides[i].ancho = 50;
        asteroides[i].alto = 50;
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
void actualizar_asteroide(Asteroide* asteroide)
{
    asteroide->y += asteroide->velocidad;

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
    double tiempo_actual = al_get_time();

    if (tiempo_actual - nave->tiempo_invulnerable < 10)
    {
        return false;
    }
    
    bool colision = nave->x < asteroide.x + asteroide.ancho &&
                    nave->x + nave->ancho > asteroide.x &&
                    nave->y < asteroide.y + asteroide.alto &&
                    nave->y + nave->largo > asteroide.y;
    
    if (colision)
    {
        nave->vida -= 100; //para acabar rapido el juego el valor normal de daño es 10
        nave->tiempo_invulnerable = tiempo_actual;
        nave->tiempo_ultimo_dano = tiempo_actual;
    }

    return colision;
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
 * @param nave La nave a dibujar.
 * @param asteroides Arreglo de asteroides a dibujar.
 * @param num_asteroides Número de asteroides en el arreglo.
 */
void dibujar_juego(Nave nave, Asteroide asteroides[], int num_asteroides)
{
    al_draw_filled_rectangle(nave.x, nave.y, nave.x + nave.ancho, nave.y + nave.largo, al_map_rgb(0, 255, 0));

    for (int i = 0; i < num_asteroides; i++)
    {
        al_draw_filled_rectangle(
            asteroides[i].x, asteroides[i].y,
            asteroides[i].x + asteroides[i].ancho, asteroides[i].y + asteroides[i].alto,
            al_map_rgb(255, 0, 0));
    }
}

/**
 * @brief Permite actualizar la posición de la nave y detectar colisiones con los asteroides.
 * 
 * @param nave Puntero a la nave que se va a mover
 * @param teclas Arreglo de teclas presionadas
 * @param asteroides Arreglo de asteroides
 * @param num_asteroides Número de asteroides en el arreglo
 * @param tiempo_actual Tiempo actual en segundos
 */
void actualizar_nave(Nave* nave, bool teclas[], Asteroide asteroides[], double tiempo_actual)
{
    if (teclas[0]) nave->y -= 5;
    if (teclas[1]) nave->y += 5;
    if (teclas[2]) nave->x -= 5;
    if (teclas[3]) nave->x += 5;

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
 * @brief 
 * 
 * @param disparos Arreglo de disparos
 * @param num_disparos Cantidad de disparos definida
 */
void actualizar_disparos(Disparo disparos[], int num_disparos)
{
    for (int i = 0; i < num_disparos; i++)
    {
        if (disparos[i].activo)
        {
            disparos[i].y -= disparos[i].velocidad;
            if (disparos[i].y < 0)
            {
                disparos[i].activo = false;
            }
        }
    }
}

/**
 * @brief 
 * 
 * @param disparos Arreglo de disparos
 * @param num_disparos Cantidad de disparos definida
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
 * @brief Permite que la neve pueda disparar
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
            disparos[i].x = nave.x + nave.ancho / 2;
            disparos[i].y = nave.y;
            disparos[i].velocidad = 10;
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
void actualizar_juego(Nave* nave, bool teclas[], Asteroide asteroides[], int num_asteroides, Disparo disparos[], int num_disparos, int* puntaje)
{
    actualizar_nave(nave, teclas, asteroides, al_get_time());
    actualizar_disparos(disparos, num_disparos);
    for (int i = 0; i < num_asteroides; i++)
    {
        actualizar_asteroide(&asteroides[i]);
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
        detectar_colision(nave, asteroides[i]);
    }
}

/**
 * @brief Permite dibujar el puntaje que hace el jugador en la pantalla.
 * 
 * @param puntaje Puntaje que hace el jugador
 */
void dibujar_puntaje(int puntaje)
{
    ALLEGRO_FONT* fuente = al_create_builtin_font();
    if (!fuente)
    {
        fprintf(stderr, "Error: no se pudo cargar la fuente.\n");
        return;
    }
    
    char text_puntaje[20];
    sprintf(text_puntaje, "Puntaje: %d", puntaje);
    al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 40, ALLEGRO_ALIGN_LEFT, text_puntaje);

    al_destroy_font(fuente);
}

/**
 * @brief Inicializa los botones del menu principal.
 * 
 * @param botones Arreglo de los botones del menu principal
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
 * @brief Dibuja los botones del menu principal.
 * 
 * @param botones Arreglo de botones del menu principal
 * @param num_botones Numero de botones del menu principal
 * @param fuente Fuente de letra usada en el menu principal
 */
void dibujar_botones(Boton botones[], int num_botones, ALLEGRO_FONT* fuente)
{
    for (int i = 0; i < num_botones; i++)
    {
        al_draw_filled_rectangle(botones[i].x, botones[i].y, botones[i].x + botones[i].ancho, botones[i].y + botones[i].alto, al_map_rgb(0, 0, 0));
        al_draw_text(fuente, al_map_rgb(0, 255, 0), botones[i].x + botones[i].ancho / 2, botones[i].y + botones[i].alto / 2, ALLEGRO_ALIGN_CENTER, botones[i].texto);
    }   
}

/**
 * @brief Permite detectar los clicks que hace el mouse en los botones del menu principal.
 * 
 * @param botones Arreglo de los botones del menu principal
 * @param num_botones Numero de botones del menu principal
 * @param x Posicion en el eje x del mouse
 * @param y Posicion en el eje y del mouse
 * @return int El numero de boton que se clickeo
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
            if (evento.mouse.x >= boton_volver.x && evento.mouse.x <= boton_volver.x + boton_volver.ancho &&
                evento.mouse.y >= boton_volver.y && evento.mouse.y <= boton_volver.y + boton_volver.alto)
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
void capturar_nombre(ALLEGRO_FONT* fuente, char* nombre)
{
    ALLEGRO_EVENT_QUEUE* cola_eventos = al_create_event_queue();
    al_register_event_source(cola_eventos, al_get_keyboard_event_source());

    int pos = 0;
    nombre[0] = '\0';

    while (true)
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if (evento.keyboard.unichar >= 'a' && evento.keyboard.unichar <= 'z' && pos < MAX_NOMBRE - 1)
            {
                nombre[pos++] = (char)evento.keyboard.unichar;
                nombre[pos] = '\0';
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && pos > 0)
            {
                nombre[--pos] = '\0';
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                break;
            }
        }
        
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, "Ingrese su nombre:");
        al_draw_text(fuente, al_map_rgb(255, 255, 255), 400, 350, ALLEGRO_ALIGN_CENTER, nombre);
        al_flip_display();
    }
    
    al_destroy_event_queue(cola_eventos);
}