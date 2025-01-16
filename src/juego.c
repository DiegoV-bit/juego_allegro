#include "juego.h"

/**
 * @file juego.c
 * @brief Este archivo contiene todas las funciones referentes al juego.
 * /
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


void actualizar_asteroide(Asteroide* asteroide)
{
    asteroide->y += asteroide->velocidad;

    if (asteroide->y > 600)
    {
        asteroide->y = -asteroide->alto;
        asteroide->y = rand() % (800 - (int)asteroide->ancho);
    }
}


bool detectar_colision(Nave nave, Asteroide asteroide)
{
    return !(nave.x + nave.ancho < asteroide.x ||
             nave.x > asteroide.x + asteroide.ancho ||
             nave.y + nave.largo < asteroide.y ||
             nave.y > asteroide.y + asteroide.alto);
}


void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave)
{
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch (evento.keyboard.keycode)
        {
        case ALLEGRO_KEY_LEFT:
            nave->x -= 10;
            break;
        
        case ALLEGRO_KEY_RIGHT:
            nave->x += 10;
            break;
        }
    }
}


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