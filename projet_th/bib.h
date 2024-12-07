//
// Created by trist on 23/11/2024.
//

#ifndef PROJET_TH_BIB_H
#define PROJET_TH_BIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_SOMMETS 100
#define INFINI INT_MAX
typedef struct {
    char nom[20];
    double population;
    double classe;    // Classe d'animal (pour la quantité de nourriture consommée)
    double coefficientCroissance;
} Sommet;
int graphe[MAX_SOMMETS][MAX_SOMMETS];
Sommet sommets[MAX_SOMMETS];
int nombreDeSommets = 0;
int trouverIndexSommet(char *nom);


void dijkstra(int source, int destination);
#endif //PROJET_TH_BIB_H
