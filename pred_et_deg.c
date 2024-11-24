#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_SOMMETS 100

typedef struct {
    char nom[20];
} Sommet;

Sommet sommets[MAX_SOMMETS];
int graphe[MAX_SOMMETS][MAX_SOMMETS] = {0};
int nombreDeSommets = 0;

int trouverIndexSommet(char *nom) {
    for (int i = 0; i < nombreDeSommets; i++) {
        if (strcmp(sommets[i].nom, nom) == 0) {
            return i;
        }
    }
    return -1;
}

void ajouterSommet(char *nom) {
    strcpy(sommets[nombreDeSommets++].nom, nom);
}

void ajouterArc(char *origine, char *destination, int poids) {
    int u = trouverIndexSommet(origine);
    int v = trouverIndexSommet(destination);
    if (u == -1) {
        ajouterSommet(origine);
        u = nombreDeSommets - 1;
    }
    if (v == -1) {
        ajouterSommet(destination);
        v = nombreDeSommets - 1;
    }
    graphe[u][v] = poids;
}

void afficherPredSucc(int sommet) {
    printf("\nRelations pour %s :\n", sommets[sommet].nom);

    // Successeurs
    printf("Successeurs : ");
    int hasSuccesseurs = 0;
    for (int i = 0; i < nombreDeSommets; i++) {
        if (graphe[sommet][i] > 0) {
            printf("%s (poids: %d) ", sommets[i].nom, graphe[sommet][i]);
            hasSuccesseurs = 1;
        }
    }
    if (!hasSuccesseurs) printf("Aucun");
    printf("\n");

    // Prédécesseurs
    printf("Predecesseurs : ");
    int hasPredecesseurs = 0;
    for (int i = 0; i < nombreDeSommets; i++) {
        if (graphe[i][sommet] > 0) {
            printf("%s (poids: %d) ", sommets[i].nom, graphe[i][sommet]);
            hasPredecesseurs = 1;
        }
    }
    if (!hasPredecesseurs) printf("Aucun");
    printf("\n");
}

void calculerDegre(int sommet) {
    int degreInterieur = 0, degreExterieur = 0;

    for (int i = 0; i < nombreDeSommets; i++) {
        if (graphe[i][sommet] > 0) degreInterieur++;
        if (graphe[sommet][i] > 0) degreExterieur++;
    }

    printf("Sommet %s : Degre intérieur = %d, Degre exterieur = %d\n",
           sommets[sommet].nom, degreInterieur, degreExterieur);
}

int main() {
    char nomFichier[100];
    printf("Entrez le nom du fichier contenant le graphe : ");
    scanf("%s", nomFichier);

    FILE *fichier = fopen(nomFichier, "r");
    if (!fichier) {
        printf("Erreur d'ouverture du fichier : %s\n", nomFichier);
        return 1;
    }

    char origine[20], destination[20];
    int poids;

    while (fscanf(fichier, "%s %s %d", origine, destination, &poids) == 3) {
        ajouterArc(origine, destination, poids);
    }

    fclose(fichier);

    while (1) {
        printf("\nMenu :\n");
        printf("1. Afficher les predecesseurs et successeurs d'un sommet\n");
        printf("2. Calculer le degre intérieur et extérieur d'un sommet\n");
        printf("3. Quitter\n");
        printf("\nVotre choix : ");
        int choix;
        scanf("%d", &choix);

        if (choix == 3) break;

        if (choix == 1) {
            printf("Entrez le nom du sommet : ");
            scanf("%s", origine);

            int sommet = trouverIndexSommet(origine);
            if (sommet == -1) {
                printf("Sommet invalide.\n");
                continue;
            }

            afficherPredSucc(sommet);
        } else if (choix == 2) {
            printf("Entrez le nom du sommet : ");
            scanf("%s", origine);

            int sommet = trouverIndexSommet(origine);
            if (sommet == -1) {
                printf("Sommet invalide.\n");
                continue;
            }

            calculerDegre(sommet);
        } else {
            printf("Choix invalide.\n");
        }
    }

    return 0;
}