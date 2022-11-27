#include "board.h"
#include "connect4.h"
#include "3dmode.h"

#include <stdbool.h>
#include <stdio.h>

#define skyboxName(skybox) skybox == 1 ? "Al mar" : skybox == 2 ? "Fabrica abandonada" : "Habitacio acollidora"

int main()
{
    int mode3D = 1;
    int skybox = 3;
    int twoPlayers = 0;
    int firstPlayer = PLAYER1;
    
    int ans;
    while (true)
    {
        clearScr();
        printf("Configuracio actual:\n");
        printf("    1. Mode 3D: %s\n", mode3D ? "Activat" : "Desactivat");
        if (mode3D) printf("    2. Escenari: %s\n", skyboxName(skybox));
        printf("    3. Mode 2 jugadors: %s\n", twoPlayers ? "Activat" : "Desactivat");
        if (!twoPlayers) printf("    4. Comenca la partida: %s\n", firstPlayer == 1 ? "Tu" : "La maquina");
        printf("Introdueix un dels numeros per canviar la corresponent opcio, o introdueix 5 per comencar.\n");
        getUserInput(&ans, 1, 5, false);

        clearScr();
        if (ans == 1)
        {
            printf("Vols jugar en mode 3D o en consola?\n");
            printf("    1. Consola\n");
            printf("    2. 3D\n");
            getUserInput(&mode3D, 1, 2, true);
        }
        else if (ans == 2)
        {
            printf("Quin escenari vols pel mode 3D?\n");
            printf("    1. %s\n", skyboxName(1));
            printf("    2. %s\n", skyboxName(2));
            printf("    3. %s\n", skyboxName(3));
            getUserInput(&skybox, 1, 3, false);
        }
        else if (ans == 3)
        {
            printf("Vols jugar en mode 2 jugadors o contra la maquina?\n");
            printf("    1. Maquina\n");
            printf("    2. 2 Jugadors\n");
            getUserInput(&twoPlayers, 1, 2, true);
        }
        else if (ans == 4)
        {
            printf("Qui comenca?\n");
            printf("        1. Tu\n");
            printf("        2. La maquina\n");
            getUserInput(&firstPlayer, 1, 2, false);
        }
        else
        {
            if (mode3D)
            {
                return main3d(firstPlayer, twoPlayers, skybox);
            }
            else
            {
                return mainConsole(firstPlayer, twoPlayers);
            }
        }
    }
}