#ifndef PRIMITIVES_HPP_INCLUDED
#define PRIMITIVES_HPP_INCLUDED

static float cube_vertices[] = {
    // positions       // UVs
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // 0
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f, // 1
     1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // 2
    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // 3
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // 4
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // 5
     1.0f,  1.0f,  1.0f,  1.0f, 1.0f, // 6
    -1.0f,  1.0f,  1.0f,  0.0f, 1.0f  // 7
};

static unsigned int indices[] = {
    0, 1, 2,  // premier triangle de la face arrière
    2, 3, 0,  // second triangle de la face arrière
    4, 5, 6,  // premier triangle de la face avant
    6, 7, 4,  // second triangle de la face avant
    0, 1, 5,  // premier triangle de la face inférieure
    5, 4, 0,  // second triangle de la face inférieure
    3, 2, 6,  // premier triangle de la face supérieure
    6, 7, 3,  // second triangle de la face supérieure
    0, 4, 7,  // premier triangle de la face gauche
    7, 3, 0,  // second triangle de la face gauche
    1, 5, 6,  // premier triangle de la face droite
    6, 2, 1   // second triangle de la face droite
};

// Vertices
static float sphere_vertices[] = {
     0.000f,  0.000f,  1.000f, 0.5f, 1.0f,
     0.894f,  0.000f,  0.447f, 0.6f, 0.75f,
     0.276f,  0.851f,  0.447f, 0.4f, 0.75f,
    -0.724f,  0.526f,  0.447f, 0.3f, 0.75f,
    -0.724f, -0.526f,  0.447f, 0.2f, 0.75f,
     0.276f, -0.851f,  0.447f, 0.7f, 0.75f,
     0.724f,  0.526f, -0.447f, 0.6f, 0.25f,
    -0.276f,  0.851f, -0.447f, 0.4f, 0.25f,
    -0.894f,  0.000f, -0.447f, 0.3f, 0.25f,
    -0.276f, -0.851f, -0.447f, 0.2f, 0.25f,
     0.724f, -0.526f, -0.447f, 0.7f, 0.25f,
     0.000f,  0.000f, -1.000f, 0.5f, 0.0f
};

// Indices
static unsigned int sphere_indices[] = {
     0,  1,  2,
     0,  2,  3,
     0,  3,  4,
     0,  4,  5,
     0,  5,  1,
     1,  6,  2,
     2,  7,  3,
     3,  8,  4,
     4,  9,  5,
     5, 10,  1,
     6,  7,  2,
     7,  8,  3,
     8,  9,  4,
     9, 10,  5,
    10,  6,  1,
    11,  7,  6,
    11,  8,  7,
    11,  9,  8,
    11, 10,  9,
    11,  6, 10
};

#endif // PRIMITIVES_HPP_INCLUDED