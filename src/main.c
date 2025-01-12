#include "ventana.h"

int main() {
    if (!al_init()) {
        return -1;
    }

    ALLEGRO_DISPLAY *ventana = crear_ventana(800, 600, "Ventana");
    if (!ventana) {
        return -1;
    }

    mostrar_ventana();

    destruir_ventana();

    return 0;
}
