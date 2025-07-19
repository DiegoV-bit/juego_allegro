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
#include <math.h>
#include <time.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/joystick.h>

/**
 * @def NUM_ASTEROIDES
 * @brief Numero de asteroides del juego.
 */
#define NUM_ASTEROIDES 10
/**
 * @def FPS
 * @brief Frames por segundo del juego.
 */
#define FPS 60
/**
 * @def MAX_JUGADORES
 * @brief Numero maximo de jugadores en el ranking.
 */
#define MAX_JUGADORES 50
/**
 * @def MAX_NOMBRE
 * @brief Longitud maxima del nombre del jugador.
 */
#define MAX_NOMBRE 40
/**
 * @def TILE_ANCHO
 * @def TILE_ALTO
 * @brief Tamaño de los tiles del mapa.
 */
#define TILE_ANCHO 20
#define TILE_ALTO 20
/**
 * @def MAPA_FILAS
 * @def MAPA_COLUMNAS
 * @brief Filas y columnas del mapa del juego.
 */
#define MAPA_FILAS 24
#define MAPA_COLUMNAS 39
/**
 * @def MAX_DISPAROS
 * @brief Cantidad de disparos que puede efectuar la nave
 */
#define MAX_DISPAROS 15

/**
 * @def NUM_ENEMIGOS
 * @brief Número de enemigos en el juego.
 */
#define NUM_ENEMIGOS (MAPA_FILAS * MAPA_COLUMNAS)

/**
 * @def NUM_DISPAROS_ENEMIGOS
 * @brief Número máximo de disparos de enemigos.
 */
#define NUM_DISPAROS_ENEMIGOS 15

#define MAX_POWERUPS 10
#define POWERUP_PROB 25

/*Estructuras usadas en el juego*/

typedef struct
{
    float x;
    float y;
    int tipo;
    bool activo;
    double tiempo_aparicion;
    double duracion_vida;
    ALLEGRO_COLOR color;
} Powerup;

typedef struct
{
    bool activo;
    int hits_restantes;
    int hits_max;
    double tiempo_activacion;
    ALLEGRO_COLOR color;
    float intensidad;
} Escudo;

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
    float x; /**< Posicion en el eje x */
    float y; /**< Posicion en el eje y */
    float velocidad; /** <Velocidad del disparo */
    bool activo; /** <Actividad del disparo */
    float angulo; /**< Angulo del disparo */
} Disparo;

/**
 * @struct Nave
 * @brief Estructura que representa una nave en el juego. 
 * 
 * La nave tiene una posicion en los ejes "x" e "y", al igual que un ancho y un largo.
 * Además, tiene una vida, un tiempo invulnerable y un tiempo en el que recibio el ultimo dano.
 * 
 * Se le implemento un angulo para poder rotar la nave
 */
typedef struct
{
    float x; /**< Posicion en el eje x */
    float y; /**< Posicion en el eje y */
    float ancho; /**< Ancho de la nave  */
    float largo; /**< Largo de la nave */
    int vida; /**< Vida de la nave */
    int tipo; /**< Tipo de movilidad de la nave */
    double tiempo_invulnerable; /**< Tiempo que la nave es invulnerable */
    double tiempo_ultimo_dano; /**< Tiempo en el que la nave recibio el ultimo dano */
    ALLEGRO_BITMAP* imagen; /**< Imagen de la nave */
    float angulo; /**< Angulo de la nave */
    Disparo disparos[MAX_DISPAROS]; /**< Arreglo de disparos de la nave */
    int nivel_disparo_radial;
    int kills_para_mejora;
    Escudo escudo;
} Nave;

/**
 * @struct Asteroide
 * @brief Estructura que representa un asteroide en el juego.
 * 
 * El asteroide tiene una posicion en los ejes "x" e "y", al igual que un ancho y un largo.
 */
typedef struct
{
    float x; /** <posicion en el eje x */
    float y; /** <posicion en el eje y */
    float velocidad; /** <Velocidad de los asteroides */
    float ancho; /** <Ancho de los asteroides */
    float alto; /** <Alto de los asteroides */
    ALLEGRO_BITMAP* imagen; /** <Imagen de los asteroides */
} Asteroide;

/**
 * @struct Boton 
 *
 * @brief Estructura que representa los botones en el menu del juego.
 * 
 * Los botones tienen una posicion en x e y, un ancho y un alto, y un texto que los identifica.
 */
typedef struct
{
    float x; /**< Posicion en el eje x del boton*/
    float y; /**< Posicion en el eje y del boton*/
    float ancho; /**<Ancho del boton */
    float alto; /**<Alto del boton */
    char texto[20]; /**<Texto del boton */
} Boton;

/**
 * @struct Jugador
 * 
 * @brief Estructura que representa un jugador en el ranking del juego.
 * 
 * Los jugadores tienen un nombre y el puntaje respectivo al terminar de jugar
 */
typedef struct
{
    char nombre[MAX_NOMBRE]; /**<Arreglo de caracteres que representa el nombre del jugador */
    int puntaje; /**<Puntaje del jugador */
} Jugador;

/**
 * @struct Tile
 * 
 * @brief Estructura que representa un tile en el mapa del juego.
 * 
 * Los tiles tienen un tipo (que puede ser un asteroide, un enemigo, etc.) y una vida (para asteroides o enemigos).
 */
typedef struct {
    int tipo;
    int vida;
} Tile;

/**
 * @struct Enemigo
 * 
 * @brief Estructura que representa un enemigo en el juego.
 * 
 * Los enemigos tienen una posicion en los ejes "x" e "y", un ancho y un alto, una velocidad, una vida, un estado activo, un ultimo disparo, un intevalo de disparo, una imagen y un tipo.
 */
typedef struct
{
    float x;
    float y;
    float ancho;
    float alto;
    float velocidad;
    int vida;
    bool activo;
    double ultimo_disparo;
    double intervalo_disparo;
    ALLEGRO_BITMAP* imagen;
    int tipo; /*Tipo de enemigo: 0 enemigo normal/ 1 enemigo perseguidor*/
} Enemigo;

typedef struct
{
    char texto[100];
    float x;
    float y;
    double tiempo_inicio;
    double duracion;
    bool activo;
    ALLEGRO_COLOR color;
} Mensaje;

typedef struct
{
    int nivel_actual;
    bool todos_enemigos_eliminados;
    bool mostrar_transicion;
    double tiempo_inicio_transicion;
    double duracion_transicion;
    bool nivel_completado;
} EstadoJuego;


/*Funciones*/
Nave init_nave(float x, float y, float ancho, float largo, int vida, double tiempo_invulnerable, ALLEGRO_BITMAP* imagen_nave);
void init_asteroides(Asteroide asteroides[], int num_asteroides, int ancho_ventana, ALLEGRO_BITMAP* imagen_asteroide);
void actualizar_asteroide(Asteroide* asteroide, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Nave* nave);
bool detectar_colision(Nave* nave, Asteroide asteroide);
void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave, bool teclas[], Disparo disparos[], int num_disparos);
void dibujar_juego(Nave nave, Asteroide asteroides[], int num_asteroides, int nivel_actual);
void actualizar_nave(Nave* nave, bool teclas[], Asteroide asteroides[], double tiempo_actual);
void dibujar_barra_vida(Nave nave);
void init_disparos(Disparo disparos[], int num_disparos);
void actualizar_disparos(Disparo disparos[], int num_disparos);
void dibujar_disparos(Disparo disparos[], int num_disparos);
void disparar(Disparo disparos[], int num_disparos, Nave nave);
bool detectar_colision_disparo(Asteroide asteroide, Disparo disparo);
void actualizar_juego(Nave* nave, bool teclas[], Asteroide asteroides[], int num_asteroides, Disparo disparos[], int num_disparos, int* puntaje, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos[], int num_enemigos, Disparo disparos_enemigos[], int num_disparos_enemigos, Mensaje *mensaje_powerup, Mensaje *mensaje_movilidad, EstadoJuego* estado_nivel, double tiempo_actual, Powerup powerups[], int max_powerups);
void dibujar_puntaje(int puntaje, ALLEGRO_FONT* fuente);
void init_botones(Boton botones[]);
void dibujar_botones(Boton botones[], int num_botones, ALLEGRO_FONT* fuente, int cursor_x, int cursor_y);
int detectar_click(Boton botones[], int num_botones, int x, int y);
void guardar_puntaje(const char* nombre, int puntaje);
void cargar_ranking(Jugador ranking[], int* num_jugadores);
void mostrar_ranking(ALLEGRO_FONT* fuente, Jugador ranking[], int num_jugadores, bool* volver_menu);
void capturar_nombre(ALLEGRO_FONT* fuente, char* nombre, ALLEGRO_EVENT_QUEUE* cola_eventos);
int comparar_puntajes(const void* a, const void* b);
bool cursor_sobre_boton(Boton boton, int x, int y);
bool detectar_colision_circular(float x1, float y1, float r1, float x2, float y2, float r2);
void cargar_tilemap(const char* filename, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos[], int* num_enemigos, ALLEGRO_BITMAP* imagen_enemigo, float *nave_x, float *nave_y);
void dibujar_tilemap(Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], ALLEGRO_BITMAP* imagen_asteroide);
void init_enemigos(Enemigo enemigos[], int num_enemigos, ALLEGRO_BITMAP* imagen_enemigo);
void actualizar_enemigos(Enemigo enemigos[], int num_enemigos, Disparo disparos_enemigos[], int num_disparos_enemigos, double tiempo_actual, Nave nave);
void dibujar_enemigos(Enemigo enemigos[], int num_enemigos);
void actualizar_disparos_enemigos(Disparo disparos[], int num_disparos);
void dibujar_disparos_enemigos(Disparo disparos[], int num_disparos);
bool detectar_colision_disparo_enemigo_nave(Nave nave, Disparo disparo);
void enemigo_disparar(Disparo disparos[], int num_disparos, Enemigo enemigo);
bool detectar_colision_disparo_enemigo(Disparo disparo, Enemigo enemigo);
bool detectar_colision_nave_enemigo(Nave nave, Enemigo enemigo);
bool detectar_colision_generica(float x1, float y1, float ancho1, float alto1, float x2, float y2, float ancho2, float alto2);
void disparar_radial(Disparo disparos[], int num_disparos, Nave nave);
void verificar_mejora_disparo_radial(Nave *nave, Mensaje* mensaje_powerup);
void dibujar_nivel_powerup(Nave nave, ALLEGRO_FONT* fuente);
void init_mensaje(Mensaje* mensaje);
void mostrar_mensaje(Mensaje* mensaje, const char* texto, float x, float y, double duracion, ALLEGRO_COLOR color);
void actualizar_mensaje(Mensaje* mensaje, double tiempo_actual);
void dibujar_mensaje(Mensaje mensaje, ALLEGRO_FONT* fuente);
void init_estado_juego(EstadoJuego* estado);
bool verificar_nivel_completado(Enemigo enemigos[], int num_enemigos);
void mostrar_pantalla_transicion(int nivel_completado, int nivel_siguiente, ALLEGRO_FONT* fuente, double tiempo_transcurrido, double duracion_total);
bool cargar_siguiente_nivel(int nivel, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos_mapa[], int* num_enemigos_cargados, ALLEGRO_BITMAP* imagen_enemigo, float* nave_x, float* nave_y);
void actualizar_estado_nivel(EstadoJuego* estado, Enemigo enemigos[], int num_enemigos, double tiempo_actual);
bool asteroides_activados(int nivel_actual);
void init_enemigo_tipo(Enemigo* enemigo, int col, int fila, int tipo, ALLEGRO_BITMAP* imagen_enemigo);
void francotirador_disparar(Disparo disparos[], int num_disparos, Enemigo enemigo, Nave nave);
void tanque_disparar(Disparo disparos[], int num_disparos, Enemigo enemigo);
bool detectar_colision_disparo_enemigo_escudo(Disparo disparo, float tile_x, float tile_y);
void init_powerup(Powerup* powerup);
void crear_powerup_escudo(Powerup powerups[], int max_powerups, float x, float y);
void actualizar_powerups(Powerup powerups[], int max_powerups, double tiempo_actual);
void dibujar_powerups(Powerup powerups[], int max_powerups);
bool detectar_colision_powerup(Nave nave, Powerup powerup);
void recoger_powerup(Nave* nave, Powerup* powerup, Mensaje* mensaje);
void init_escudo(Escudo* escudo);
void actualizar_escudo(Escudo* escudo, double tiempo_actual);
void dibujar_escudo(Nave nave);
bool escudo_activo(Nave nave);
void init_powerup(Powerup* powerup);
void crear_powerup_escudo(Powerup powerups[], int max_powerups, float x, float y);
void actualizar_powerups(Powerup powerups[], int max_powerups, double tiempo_actual);
void dibujar_powerups(Powerup powerups[], int max_powerups);
bool detectar_colision_powerup(Nave nave, Powerup powerup);
void recoger_powerup(Nave* nave, Powerup* powerup, Mensaje* mensaje);
void init_escudo(Escudo* escudo);
void activar_escudo(Escudo* escudo, int hits_maximos);
void actualizar_escudo(Escudo* escudo, double tiempo_actual);
void dibujar_escudo(Nave nave);
bool escudo_activo(Nave nave);
bool escudo_recibir_dano(Escudo* escudo);
bool verificar_colision_nave_muro(float x, float y, float ancho, float largo);
#endif