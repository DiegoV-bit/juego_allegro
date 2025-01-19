#ifndef JUEGO_H
#define JUEGO_H

/**
 * @file juego.h
 * @author DiegoV-bit (diegoandresvidalmunoz21@gmail.com)
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

#define NUM_ASTEROIDES 10
#define FPS 60

/*Estructuras usadas en el juego*/
typedef struct
{
    float x; //Posicion en el eje x
    float y; //Posicion en el eje y
    float ancho; //Ancho de la nave
    float largo; //Largo de la nave
    int vida; //Vida de la nave
    double tiempo_invulnerable; //Tiempo que la nave es invulnerable
} Nave;

typedef struct
{
    float x;
    float y;
    float velocidad;
    float ancho;
    float alto;
} Asteroide;

typedef struct {
    float x, y;
    float velocidad;
    bool activo;
} Disparo;

/*Funciones*/
Nave init_nave(float x, float y, float ancho, float largo);
void init_asteroides(Asteroide asteroides[], int num_asteroides, int ancho_ventana);
void actualizar_asteroide(Asteroide* asteroide);
bool detectar_colision(Nave nave, Asteroide asteroide);
void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave, bool teclas[], Disparo disparos[], int num_disparos);
void dibujar_juego(Nave nave, Asteroide asteroides[], int num_asteroides);
void actualizar_nave(Nave* nave, bool teclas[], Asteroide asteroides[], double tiempo_actual);
void dibujar_barra_vida(Nave nave);
void init_disparos(Disparo disparos[], int num_disparos);
void actualizar_disparos(Disparo disparos[], int num_disparos);
void dibujar_disparos(Disparo disparos[], int num_disparos);
void disparar(Disparo disparos[], int num_disparos, Nave nave);
bool detectar_colision_disparo(Asteroide asteroide, Disparo disparo);
void actualizar_juego(Nave* nave, bool teclas[], Asteroide asteroides[], int num_asteroides, Disparo disparos[], int num_disparos, int* puntaje);

#endif