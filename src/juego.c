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
 */
Nave init_nave(float x, float y, float ancho, float largo)
{
    Nave nave = {x, y, ancho, largo};
    return nave;
}

/**
 * @brief Inicializa los asteroides
 * 
 * Esta funcion inicializa los asteroides dandole una posicion en los ejes "x" e "y", al igual que le da un tamano
 * 
 * @param x Posicion en el eje x.
 * @param y posicion en el eje y.
 * @param velocidad
 * @param ancho Anchura de los asteroides.
 * @param largo Largo de los asteorides.
 */ 
Asteroide init_asteroide(float x, float y, float velocidad, float ancho, float largo)
{
    Asteroide asteroide;
    asteroide.x = x;
    asteroide.y = y;
    asteroide.velocidad = velocidad;
    asteroide.ancho = ancho;
    asteroide.alto = largo;
    return asteroide;
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
    return !(nave.x + nave.ancho < asteroide.x ||
             nave.x > asteroide.x + asteroide.ancho ||
             nave.y + nave.largo < asteroide.y ||
             nave.y > asteroide.y + asteroide.alto);
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


void actualizar_nave(Nave* nave, bool teclas[])
{
    if (teclas[ALLEGRO_KEY_LEFT])
    {
        nave->x -= 10;
    }
    
    if (teclas[ALLEGRO_KEY_RIGHT])
    {
        nave->x += 10;
    }
}