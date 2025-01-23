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
#include <allegro5/allegro_font.h>

#define NUM_ASTEROIDES 10
#define FPS 60
#define MAX_JUGADORES 50
#define MAX_NOMBRE 40

/*Estructuras usadas en el juego*/
/**
 * @struct Nave
 * @brief Estructura que representa una nave en el juego. 
 * 
 * La nave tiene una posicion en los ejes "x" e "y", al igual que un ancho y un largo.
 * Además, tiene una vida, un tiempo invulnerable y un tiempo en el que recibio el ultimo dano.
 */
typedef struct
{
    float x; //Posicion en el eje x
    float y; //Posicion en el eje y
    float ancho; //Ancho de la nave
    float largo; //Largo de la nave
    int vida; //Vida de la nave
    double tiempo_invulnerable; //Tiempo que la nave es invulnerable
    double tiempo_ultimo_dano; //Tiempo en el que la nave recibio el ultimo dano
} Nave;

/**
 * @struct Asteroide
 * @brief Estructura que representa un asteroide en el juego.
 * 
 * El asteroide tiene una posicion en los ejes "x" e "y", al igual que un ancho y un largo.
 */
typedef struct
{
    float x;
    float y;
    float velocidad;
    float ancho;
    float alto;
} Asteroide;

/**
 * @struct Disparo
 * @brief Estructura que representa un disparo en el juego.
 * 
 * Caracteristicas de un disparo:
 * - Posicion en los ejes "x" e "y".
 * - Velocidad del disparo.
 * - Estado del disparo (activo o inactivo).
 */
typedef struct {
    float x, y;
    float velocidad;
    bool activo;
} Disparo;

typedef struct
{
    float x;
    float y;
    float ancho;
    float alto;
    char texto[20];
} Boton;

typedef struct
{
    char nombre[MAX_NOMBRE];
    int puntaje;
} Jugador;


/*Funciones*/
Nave init_nave(float x, float y, float ancho, float largo, int vida, double tiempo_invulnerable);
void init_asteroides(Asteroide asteroides[], int num_asteroides, int ancho_ventana);
void actualizar_asteroide(Asteroide* asteroide);
bool detectar_colision(Nave* nave, Asteroide asteroide);
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
void dibujar_puntaje(int puntaje);
void init_botones(Boton botones[]);
void dibujar_botones(Boton botones[], int num_botones, ALLEGRO_FONT* fuente);
int detectar_click(Boton botones[], int num_botones, int x, int y);
void guardar_puntaje(const char* nombre, int puntaje);
void cargar_ranking(Jugador ranking[], int* num_jugadores);
void mostrar_ranking(ALLEGRO_FONT* fuente, Jugador ranking[], int num_jugadores, bool* volver_menu);
void capturar_nombre(ALLEGRO_FONT* fuente, char* nombre);
int comparar_puntajes(const void* a, const void* b);

#endif