#ifndef JUEGO_H
#define JUEGO_H

/*Bibliotecas usadas*/
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

/*Estructuras usadas en el juego*/
typedef struct
{
    float x; //Posicion en el eje x
    float y; //Posicion en el eje y
    float ancho; //Ancho de la nave
    float largo; //Largo de la nave
} Nave;

typedef struct
{
    float x;
    float y;
    float velocidad;
    float ancho;
    float alto;
} Asteroide;

/*Funciones*/
Nave init_nave(float x, float y, float ancho, float largo);
Asteroide init_asteroide(float x, float y, float velocidad, float ancho, float largo);
void actualizar_juego(Nave* nave, Asteroide* asteroide);
bool detectar_colision(Nave nave, Asteroide asteroide);
void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave);
void dibujar_juego(Nave nave, Asteroide asteroide);

#endif