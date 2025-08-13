#ifndef VENTANA_H
#define VENTANA_H

/**
 * @file ventana.h
 * @brief Biblioteca que contiene las funciones para crear y destruir la ventana del juego.
 * @version 0.1
 * @date 2025-01-17
 * 
 * 
 */

/*Bibliotecas usadas*/
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/joystick.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

/*Constantes*/
#define ANCHO_VENTANA 800 /**< Ancho de la ventana */
#define ALTO_VENTANA 600 /**< Alto de la ventana */

/*Funciones*/
int init_allegro(); /*Inicializa allegro*/
ALLEGRO_DISPLAY *crear_ventana(int ancho, int largo, const char *titulo); /*Permite crear la ventana dandole una resolucion especifica y una titulo a la ventana*/
void destruir_recursos(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* cola_eventos, ALLEGRO_TIMER* temporizador, ALLEGRO_FONT* fuente, ALLEGRO_BITMAP* imagen, ALLEGRO_BITMAP* imagen_nave, ALLEGRO_BITMAP* imagen_asteroide, ALLEGRO_BITMAP* imagen_enemigo, ALLEGRO_BITMAP *imagen_menu, ALLEGRO_SAMPLE *musica_menu); /*Destruye los recursos de la ventana*/
int init_juego(ALLEGRO_DISPLAY **ventana, ALLEGRO_EVENT_QUEUE **cola_eventos, ALLEGRO_TIMER **temporizador, ALLEGRO_FONT **fuente, ALLEGRO_BITMAP **imagen_fondo, ALLEGRO_BITMAP **imagen_nave, ALLEGRO_BITMAP **imagen_asteroide, ALLEGRO_BITMAP **imagen_enemigo, ALLEGRO_BITMAP **imagen_menu, ALLEGRO_SAMPLE **musica_menu);

#endif