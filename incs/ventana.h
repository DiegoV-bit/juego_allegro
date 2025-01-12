#ifndef VENTANA_H
#define VENTANA_H

/*Bibliotecas usadas*/
#include <stdio.h>
#include <allegro5/allegro.h>

/*Funciones*/
int init_allegro(); /*Inicializa allegro*/
ALLEGRO_DISPLAY *crear_ventana(int ancho, int largo, const char *titulo); /*Permite crear la ventana dandole una resolucion especifica y una titulo a la ventana*/
void mostrar_ventana(); /*Permite mostrar la ventana anteriormente creada*/
void destruir_ventana(); /*Destruye la ventana que se habia creado una vez terminado de usar el programa*/

#endif