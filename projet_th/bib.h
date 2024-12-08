//
// bib.h - Header File
//

#ifndef PROJET_TH_BIB_H
#define PROJET_TH_BIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Définition des constantes globales
#define MAX_SOMMETS 50
#define INFINI 999999

// Structure représentant un sommet
typedef struct {
    char nom[20];
    double population;
    double coefficientCroissance;
} Sommet;

// Variables globales pour les sommets et le graphe
extern Sommet sommets[MAX_SOMMETS];
extern double graphe[MAX_SOMMETS][MAX_SOMMETS];
extern int nombreDeSommets;

// Prototypes des fonctions
void chargerGraphe();                        // Charger les relations de prédation dans le graphe
int trouverIndexSommet(char *nom);           // Trouver l'index d'un sommet donné par son nom
double calculerCapacite(int i);              // Calculer la capacité de portage pour un sommet
void reglerPopulation();                     // Calculer une itération des populations
void afficherGraphe();                       // Afficher toutes les relations du graphe
void afficherPredSucc(int sommet);           // Afficher les prédécesseurs et successeurs d'un sommet
void genererDotFile(const char *nomFichier); // Générer un fichier DOT pour une visualisation
void calculerDegre(int sommet);              // Calculer les degrés intérieur et extérieur d'un sommet
void calculerCentraliteIntermediaire();      // Calculer la centralité d'intermédiarité pour les sommets
void dijkstra(int source, int destination);  // Trouver le plus court chemin avec l'algorithme de Dijkstra

#endif // PROJET_TH_BIB_H
