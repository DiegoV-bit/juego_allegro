#ifndef JUEGO_H
#define JUEGO_H

/**
 * @file juego.h
 * @author DiegoV-bit (you@domain.com)
 * @brief Biblioteca que contiene las funciones y estructuras del juego.
 * @version 0.1
 * @date 2025-01-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*Bibliotecas usadas*/
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define NUM_ASTEROIDES 5
#define FPS 60

/*Estructuras usadas en el juego*/
typedef struct
{
    float x; //Posicion en el eje x
    float y; //Posicion en el eje y
    float ancho; //Ancho de la nave
    float largo; //Largo de la nave
    int vida; //Vida de la nave
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
void init_asteroides(Asteroide asteroides[], int num_asteroides, int ancho_ventana, int alto_ventana);
void actualizar_asteroide(Asteroide* asteroide);
bool detectar_colision(Nave nave, Asteroide asteroide);
void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave, bool teclas[]);
void dibujar_juego(Nave nave, Asteroide asteroides[], int num_asteroides);
void actualizar_nave(Nave* nave, bool teclas[], Asteroide asteroides[], int num_asteroides);
void dibujar_barra_vida(Nave nave);

#endif