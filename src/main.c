#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>
#include "ventana.h"

int main() {
    if (init_allegro() != 0)
    {
        return -1;
    }
    
    ALLEGRO_DISPLAY *ventana = crear_ventana(800, 600, "Ventana con allegro");
    if (!ventana)
    {
        return -1;
    }
    
    al_clear_to_color(al_map_rgb(0, 0, 255));

    al_flip_display();

    al_rest(5.0);

    destruir_ventana(ventana);

    return 0;
}