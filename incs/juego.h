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

/**
 * @def MAX_POWERUPS
 * @brief Número máximo de power-ups en el juego.
 */
#define MAX_POWERUPS 10

/**
 * @def POWERUP_PROB
 * @brief Probabilidad de que aparezca un power-up.
 */
#define POWERUP_PROB 25

/**
 * @def POWERUP_ESCUDO_PROB
 * @brief Probabilidad de que aparezca un power-up de escudo.
 */
#define POWERUP_ESCUDO_PROB 40

/**
 * @def POWERUP_VIDA_PROB
 * @brief Probabilidad de que aparezca un power-up de vida.
 */
#define POWERUP_VIDA_PROB 30

/**
 * @def POWERUP_RADIAL_PROB
 * @brief Probabilidad de que aparezca un power-up de disparo radial.
 */
#define POWERUP_LASER_PROB 25

/**
 * @def POWERUP_EXPLOSIVO_PROB
 * @brief Probabilidad de que aparezca un power-up explosivo.
 */
#define POWERUP_EXPLOSIVO_PROB 30

/**
 * @def POWERUP_MISIL_PROB
 * @brief Probabilidad de que aparezca un power-up de misil.
 */
#define POWERUP_MISIL_PROB 15

/**
 * @def MAX_COLA_MENSAJES
 * @brief Número máximo de mensajes en la cola de mensajes.
 */
#define MAX_COLA_MENSAJES 5

/**
 * @def NUM_TIPOS_ENEMIGOS
 * @brief Número de tipos de enemigos en el juego.
 */
#define NUM_TIPOS_ENEMIGOS 5

/**
 * @def NUM_TIPOS_JEFES
 * @brief Numero de tipos de jefes en el juego
 */
#define NUM_TIPOS_JEFES 2

/**
 * @def MAX_ENEMIGOS_JEFE
 * @brief Número máximo de enemigos que puede invocar un jefe.
 */
#define MAX_ATAQUES_JEFE 20

/** 
 * @def TIEMPO_INVOCACION_ENEMIGOS
 * @brief Tiempo en segundos entre invocaciones de enemigos. 
*/
#define TIEMPO_INVOCACION_ENEMIGOS 15.0

/**
 * @enum TipoArma
 * @brief Enumeración que define los tipos de armas disponibles en el juego.
 */
typedef enum
{
    Arma_normal = 0,
    Arma_laser = 1,
    Arma_explosiva = 2,
    Arma_misil = 3
} TipoArma;

typedef enum
{
    Ataque_rafaga = 0,
    Ataque_laser_giratorio,
    Ataque_lluvia,
    Ataque_ondas,
    Ataque_perseguidor
} TipoAtaqueJefe;

/*Estructuras usadas en el juego*/

/**
 * @struct SistemaArma
 * @brief Estructura que representa un sistema de arma en el juego.
 */
typedef struct
{
    TipoArma tipo;
    int nivel;
    int kills_mejora;
    int kills_necesarias;
    bool desbloqueado;
    double ultimo_uso;
    char nombre[30];
    char descripcion[100];
} SistemaArma;

/**
 * @struct DisparoLaser
 * @brief Estructura que representa un disparo láser en el juego.
 * 
 */
typedef struct
{
    float x;
    float y;
    float x_nave;
    float y_nave;
    float ancho;
    float alto;
    float angulo;
    bool activo;
    double tiempo_inicio;
    double duracion_max;
    double ultimo_dano;
    int poder;
    float alcance;
    float dano_por_segundo;
    ALLEGRO_COLOR color;
} DisparoLaser;

/**
 * @struct DisparoExplosivo
 * @brief Estructura que representa un disparo explosivo en el juego.
 * 
 */
typedef struct
{
    float x;
    float y;
    float vx;
    float vy;
    float ancho;
    float alto;
    bool activo;
    double tiempo_vida;
    int radio_explosion;
    int dano_directo;
    int dano_area;
    bool exploto;
    bool dano_aplicado;
} DisparoExplosivo;

/**
 * @struct MisilTeledirigido
 * @brief Estructura que representa un misil teledirigido en el juego.
 * 
 */
typedef struct
{
    float x;
    float y;
    float vx;
    float vy;
    float ancho;
    float alto;
    float vel_max;
    float fuerza_giro;
    int enemigo_objetivo;
    bool activo;
    bool tiene_objetivo;
    double tiempo_vida;
    int dano;
} MisilTeledirigido;

/**
 * @struct Powerup
 * @brief Estructura que representa un power-up en el juego.
 * 
 */
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

/**
 * @struct Escudo
 * @brief Estructura que representa un escudo en el juego.
 * 
 */
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
    float vida; /**< Vida de la nave */
    int tipo; /**< Tipo de movilidad de la nave */
    double tiempo_invulnerable; /**< Tiempo que la nave es invulnerable */
    double tiempo_ultimo_dano; /**< Tiempo en el que la nave recibio el ultimo dano */
    ALLEGRO_BITMAP* imagen; /**< Imagen de la nave */
    float angulo; /**< Angulo de la nave */
    Disparo disparos[MAX_DISPAROS]; /**< Arreglo de disparos de la nave */
    int nivel_disparo_radial;
    int kills_para_mejora;
    Escudo escudo;
    SistemaArma armas[4];
    TipoArma arma_actual;
    int arma_seleccionada;
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
    float vida;
    float vida_max;
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
    bool centrado;
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

typedef struct
{
    char texto[100];
    double duracion;
    ALLEGRO_COLOR color;
    bool centrado;
} MensajeEnCola;

typedef struct
{
    MensajeEnCola mensajes[MAX_COLA_MENSAJES];
    int inicio;
    int fin;
    int cantidad;
    Mensaje mensaje_actual;
    bool procesando;
} ColaMensajes;

typedef struct
{
    Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS];
    ALLEGRO_BITMAP *imagen_juego;
} ContextoJuego;

typedef struct
{
    float x;
    float y;
    float vx;
    float vy;
    float angulo;
    bool activo;
    TipoAtaqueJefe tipo;
    double tiempo_vida;
    float dano;
    float velocidad;
    bool persigue_jugador;
    ALLEGRO_COLOR color;
} AtaqueJefe;

typedef struct
{
    float x;
    float y;
    float ancho;
    float alto;
    float vida;
    float vida_max;
    int tipo;
    bool activo;

    // Sistema de ataques de los jefes
    AtaqueJefe ataques[MAX_ATAQUES_JEFE];
    double ultimo_ataque;
    double intervalo_ataque;
    TipoAtaqueJefe ataque_actual;
    int fase_ataque;

    // Invocacion de enemigos
    double ultima_invocacion;
    int enemigos_invocados;
    int max_enemigos_invocacion;

    // Estados especiales
    bool en_furia;
    double tiempo_furia;
    float velocidad_movimiento;

    // Animacion y efectos
    float angulo_laser;
    double tiempo_animacion;

    ALLEGRO_BITMAP *imagen;
} Jefe;


/*Funciones*/
Nave init_nave(float x, float y, float ancho, float largo, float vida, double tiempo_invulnerable, ALLEGRO_BITMAP* imagen_nave);
void init_asteroides(Asteroide asteroides[], int num_asteroides, int ancho_ventana, ALLEGRO_BITMAP* imagen_asteroide);
void actualizar_asteroide(Asteroide* asteroide, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Nave* nave, Powerup powerups[], int max_powerups);
bool detectar_colision(Nave* nave, Asteroide asteroide);
void manejar_eventos(ALLEGRO_EVENT evento, Nave* nave, bool teclas[]);
void dibujar_juego(Nave nave, Asteroide asteroides[], int num_asteroides, int nivel_actual, ALLEGRO_BITMAP *imagen_fondo);
void actualizar_nave(Nave* nave, bool teclas[], Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS]);
void dibujar_barra_vida(Nave nave);
void init_disparos(Disparo disparos[], int num_disparos);
void actualizar_disparos(Disparo disparos[], int num_disparos);
void dibujar_disparos(Disparo disparos[], int num_disparos);
void disparar(Disparo disparos[], int num_disparos, Nave nave);
bool detectar_colision_disparo(Asteroide asteroide, Disparo disparo);
void actualizar_juego(Nave* nave, bool teclas[], Asteroide asteroides[], int num_asteroides, Disparo disparos[], int num_disparos, int* puntaje, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], Enemigo enemigos[], int num_enemigos, Disparo disparos_enemigos[], int num_disparos_enemigos, ColaMensajes *cola_mensajes, EstadoJuego* estado_nivel, double tiempo_actual, Powerup powerups[], int max_powerups);
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
void verificar_mejora_disparo_radial(Nave *nave, ColaMensajes *cola_mensajes);
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
void dibujar_powerups(Powerup powerups[], int max_powerups, int *contador_parpadeo, int *contador_debug);
bool detectar_colision_powerup(Nave nave, Powerup powerup);
void recoger_powerup(Nave* nave, Powerup* powerup, ColaMensajes *cola_mensajes);
void init_escudo(Escudo* escudo);
void actualizar_escudo(Escudo* escudo, double tiempo_actual);
void dibujar_escudo(Nave nave);
bool escudo_activo(Nave nave);
void init_powerup(Powerup* powerup);
void crear_powerup_escudo(Powerup powerups[], int max_powerups, float x, float y);
void actualizar_powerups(Powerup powerups[], int max_powerups, double tiempo_actual);
void dibujar_powerups(Powerup powerups[], int max_powerups, int *contador_parpadeo, int *contador_debug);
bool detectar_colision_powerup(Nave nave, Powerup powerup);
void recoger_powerup(Nave* nave, Powerup* powerup, ColaMensajes *cola_mensajes);
void init_escudo(Escudo* escudo);
void activar_escudo(Escudo* escudo, int hits_maximos);
void actualizar_escudo(Escudo* escudo, double tiempo_actual);
void dibujar_escudo(Nave nave);
bool escudo_activo(Nave nave);
bool escudo_recibir_dano(Escudo* escudo);
bool verificar_colision_nave_muro(float x, float y, float ancho, float largo, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS]);
void init_cola_mensajes(ColaMensajes* cola);
void agregar_mensaje_cola(ColaMensajes* cola, const char* texto, double duracion, ALLEGRO_COLOR color, bool centrado);
void actualizar_cola_mensajes(ColaMensajes* cola, double tiempo_actual);
void dibujar_cola_mensajes(ColaMensajes cola, ALLEGRO_FONT* fuente);
void mostrar_mensaje_centrado(Mensaje* mensaje, const char* texto, double duracion, ALLEGRO_COLOR color);
void dibujar_hitboxes_debug(Nave nave, Enemigo enemigos[], int num_enemigos, Disparo disparos[], int num_disparos, Disparo disparos_enemigos[], int num_disparos_enemigos, Asteroide asteroides[], int num_asteroides, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS]);
void limpiar_memoria_juego(Disparo disparos[], int num_disparos, Disparo disparos_enemigos[], int num_disparos_enemigos, Powerup powerups[], int max_powerups, Enemigo enemigos[], int num_enemigos, ColaMensajes* cola_mensajes);
void crear_powerup_vida(Powerup powerups[], int max_powerups, float x, float y);
void crear_powerup_aleatorio(Powerup powerups[], int max_powerups, float x, float y);
float obtener_radio_nave(Nave nave);
void obtener_centro_nave(Nave nave, float* centro_x, float* centro_y);
void init_sistema_armas(Nave* nave);
void cambiar_arma(Nave *nave, TipoArma nueva_arma);
void actualizar_progreso_arma(Nave* nave, TipoArma tipo_arma);
void verificar_mejora_arma(Nave* nave, TipoArma tipo_arma, ColaMensajes* cola_mensajes);
void dibujar_info_armas(Nave nave, ALLEGRO_FONT* fuente);
void disparar_laser(DisparoLaser lasers[], int max_lasers, Nave nave);
void actualizar_lasers(DisparoLaser lasers[], int max_lasers, Enemigo enemigos[], int num_enemigos, int* puntaje, Nave nave, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS], int *contador_debug);
void dibujar_lasers(DisparoLaser lasers[], int max_lasers, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS]);
void crear_powerup_aleatorio(Powerup powerups[], int max_powerups, float x, float y);
void crear_powerup_laser(Powerup powerups[], int max_powerups, float x, float y);
void disparar_segun_arma(Nave nave, Disparo disparos[], int num_disparos, DisparoLaser lasers[], int max_lasers, DisparoExplosivo explosivos[], int max_explosivos, MisilTeledirigido misiles[], int max_misiles, Enemigo enemigos[], int num_enemigos);
void crear_powerup_explosivo(Powerup powerups[], int max_powerups, float x, float y);
void disparar_explosivo(DisparoExplosivo explosivos[], int max_explosivos, Nave nave);
void actualizar_explosivos(DisparoExplosivo explosivos[], int max_explosivos, Enemigo enemigos[], int num_enemigos, int* puntaje, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS]);
void dibujar_explosivos(DisparoExplosivo explosivos[], int max_explosivos);
void crear_powerup_misil(Powerup powerups[], int max_powerups, float x, float y);
void disparar_misil(MisilTeledirigido misiles[], int max_misiles, Nave nave, Enemigo enemigos[], int num_enemigos);
void actualizar_misiles(MisilTeledirigido misiles[], int max_misiles, Enemigo enemigos[], int num_enemigos, int* puntaje);
void dibujar_misiles(MisilTeledirigido misiles[], int max_misiles);
bool punto_en_linea_laser(float x1, float y1, float x2, float y2, float px, float py, float tolerancia);
bool laser_intersecta_enemigo(DisparoLaser laser, Enemigo enemigo);
bool linea_intersecta_rectangulo(float x1, float y1, float x2, float y2, float rect_x1, float rect_y1, float rect_x2, float rect_y2);
bool linea_intersecta_linea(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
float verificar_colision_laser_tilemap(DisparoLaser laser, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS]);
bool laser_intersecta_enemigo_limitado(DisparoLaser laser, Enemigo enemigo, float alcance_real);
bool verificar_linea_vista_explosion(float x1, float y1, float x2, float y2, Tile tilemap[MAPA_FILAS][MAPA_COLUMNAS]);
bool cargar_imagenes_enemigos(ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS]);
void asignar_imagen_enemigo(Enemigo *enemigo, ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS]);
void liberar_imagenes_enemigos(ALLEGRO_BITMAP *imagenes_enemigos[NUM_TIPOS_ENEMIGOS]);
void init_jefe(Jefe* jefe, int tipo, float x, float y, ALLEGRO_BITMAP* imagen);
void actualizar_jefe(Jefe* jefe, Nave nave, Enemigo enemigos[], int* num_enemigos, ALLEGRO_BITMAP* imagenes_enemigos[NUM_TIPOS_ENEMIGOS], double tiempo_actual);
void dibujar_jefe(Jefe jefe);
void jefe_atacar(Jefe* jefe, Nave nave, double tiempo_actual);
void dibujar_ataques_jefe(AtaqueJefe ataques[], int max_ataques);
bool detectar_colision_ataque_jefe_nave(AtaqueJefe ataque, Nave nave);
void jefe_invocar_enemigos(Jefe* jefe, Enemigo enemigos[], int* num_enemigos, ALLEGRO_BITMAP* imagenes_enemigos[NUM_TIPOS_ENEMIGOS]);
bool jefe_recibir_dano(Jefe* jefe, float dano, ColaMensajes* cola_mensajes);
#endif