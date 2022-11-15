#include "boardModel.h"
#include "board.h"

#include <stdio.h>
#include <stdlib.h>

void generateVertices(float* verts)
{
    // L'array passat com a parametre verts ha de ser de longitud 54 (3 floats per vertex, 6 floats per parella de normals, hi ha 6 vertex en total, (3+6)*6)
    // Guardem les coordenades dels 6 vertex del quad, i el vector normal per cada vertex
    float xcoord = 0.25f * NUM_COLS; // 0.5 * ncols - 0.5/2 * ncols
    float ycoord = 0.5f * NUM_ROWS;
    verts[0]  = -xcoord; verts[1]  = 0;      verts[2]  = 0; /* bottom left   */
    verts[9]  = -xcoord; verts[10] = ycoord; verts[11] = 0; /* top left      */
    verts[18] =  xcoord; verts[19] = ycoord; verts[20] = 0; /* top right     */
    verts[27] =  xcoord; verts[28] = ycoord; verts[29] = 0; /* top right     */
    verts[36] =  xcoord; verts[37] = 0;      verts[38] = 0; /* bottom right  */
    verts[45] = -xcoord; verts[46] = 0;      verts[47] = 0; /* bottom left   */

    for (int i = 0; i < 6; i++)
    {
        verts[i * 9 + 3] = 0.f;
        verts[i * 9 + 4] = 0.f;
        verts[i * 9 + 5] = 1.f;
        verts[i * 9 + 6] = 0.f;
        verts[i * 9 + 7] = 0.f;
        verts[i * 9 + 8] = -1.f;
    }
}

void getCenters(float cents[][NUM_COLS][2])
{
    // L'array cents ha de tenir longitud NUM_ROWS*NUM_COLS*2 o be ser un array de vec2 de tamany NUM_ROWS*NUM_COLS
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            cents[i][j][0] = 0.5f * j - 0.25f * NUM_COLS + 0.25f;
            cents[i][j][1] = 0.5f * i + 0.25f;
        }
    }
}
