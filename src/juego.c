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
Nave init_nave(float x, float y, float ancho, float largo)
{
    Nave nave;
    nave.x = x;
    nave.y = y;
    nave.ancho = ancho;
    nave.largo = largo;
    nave.vida = 100;
    nave.tiempo_invulnerable = 0;
    return nave;
}

/**
 * @brief Inicializa los asteroides
 * 
 * Esta funcion inicializa los asteroides dandole una posicion en los ejes "x" e "y", al igual que le da un tamano
 * 
 * @param x Posicion en el eje x.
 * @param y posicion en el eje y.
 * @param velocidad Velocidad del asteroides.
 * @param ancho Anchura de los asteroides.
 * @param largo Largo de los asteorides.
 */ 
void init_asteroides(Asteroide asteroides[], int num_asteroides, int ancho_ventana, int alto_ventana)
{
    for (int i = 0; i < num_asteroides; i++)
    {
        asteroides[i].x = rand() % (ancho_ventana - 50);
        asteroides[i].y = 0;
        asteroides[i].velocidad = (rand() % 5) + 1;
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
        asteroide->y = rand() % (800 - (int)asteroide->ancho);
    }
}

/**
   * @brief Maneja los eventos de teclado para mover la nave.
   *
   * Esta función actualiza la posición de la nave en función de las teclas presionadas.
   *
   * @param evento El evento de teclado que se va a manejar.
   * @param nave Puntero a la nave que se va a mover.
   */
bool detectar_colision(Nave nave, Asteroide asteroide)
{
    return nave.x < asteroide.x + asteroide.ancho && nave.x + nave.ancho > asteroide.x &&
           nave.y < asteroide.y + asteroide.alto && nave.y + nave.largo > asteroide.y;
}

/**
   * @brief Maneja los eventos de teclado para mover la nave.
   *
   * Esta función actualiza la posición de la nave en función de las teclas presionadas.
   *
   * @param evento El evento de teclado que se va a manejar.
   * @param nave Puntero a la nave que se va a mover.
   */
void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave, bool teclas[])
{
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        teclas[evento.keyboard.keycode] = true;
    }

    if (evento.type == ALLEGRO_EVENT_KEY_UP)
    {
        teclas[evento.keyboard.keycode] = false;
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
void actualizar_nave(Nave* nave, bool teclas[], Asteroide asteroides[], int num_asteroides, double tiempo_actual)
{
    if (teclas[ALLEGRO_KEY_LEFT])
    {
        nave->x -= 10;
    }
    
    if (teclas[ALLEGRO_KEY_RIGHT])
    {
        nave->x += 10;
    }

    for (int i = 0; i < NUM_ASTEROIDES; i++)
    {
        if (detectar_colision(*nave, asteroides[i]))
        {
            printf("Colisión detectada con el asteroide %d\n", i);
            nave->vida -= 10;
            nave->tiempo_invulnerable = tiempo_actual + 5;
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