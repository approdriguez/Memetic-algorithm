#ifndef hormiga
#define hormiga
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "random_ppio.c"
#include <queue>

using namespace std;


//ozono
//const int n_caracteristicas = 72;
//const int n_cadenas = 185;
//const char * path = "./tablas/ozone.arff";
//sonar
const int n_caracteristicas = 60;
const int n_cadenas = 208;
const char * path = "./tablas/sonar.arff";
//spam
//const int n_caracteristicas = 57;
//const int n_cadenas = 460;
//const char * path = "./tablas/spambase.arff";


float dtablas[n_cadenas][n_cadenas];
float datos[n_cadenas][n_caracteristicas];
int nclases[n_cadenas];

typedef bool Caracteristica;
typedef Caracteristica *caracteristicas;

typedef struct {
	caracteristicas s;
	float tasa_clas;
	float tasa_red;
    int tam;
} s_caracteristicas;



s_caracteristicas sol;
s_caracteristicas mejorSol;
float feromonasCaracteristicas[n_caracteristicas];
float feromonasNoCaracteristicas[n_caracteristicas];


#endif
