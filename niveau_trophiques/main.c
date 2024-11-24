#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void calculerNiveauxTrophiques(int niveaux[]) {
    int visite[MAX_SOMMETS] = {0};

    void dfs(int sommet) {
        if (visite[sommet]) return;
        visite[sommet] = 1;

        int maxNiveau = 0;
        for (int i = 0; i < nombreDeSommets; i++) {
            if (graphe[i][sommet] > 0) {
                dfs(i);
                if (niveaux[i] > maxNiveau) {
                    maxNiveau = niveaux[i];
                }
            }
        }
        niveaux[sommet] = maxNiveau + 1;
    }

    for (int i = 0; i < nombreDeSommets; i++) {
        if (!visite[i]) {
            dfs(i);
        }
    }

    printf("\nNiveaux trophiques :\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("Sommet %s : Niveau %d\n", sommets[i].nom, niveaux[i]);
    }
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
    int poids, niveaux[MAX_SOMMETS] = {0};

    while (fscanf(fichier, "%s %s %d", origine, destination, &poids) == 3) {
        ajouterArc(origine, destination, poids);
    }

    fclose(fichier);

    calculerNiveauxTrophiques(niveaux);

    return 0;
}
