#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_SOMMETS 100
#define INFINI INT_MAX

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

void calculerCentraliteIntermediaire() {
    int centralite[MAX_SOMMETS] = {0};

    for (int source = 0; source < nombreDeSommets; source++) {
        for (int destination = 0; destination < nombreDeSommets; destination++) {
            if (source != destination) {
                int distance[MAX_SOMMETS], precedent[MAX_SOMMETS], visite[MAX_SOMMETS] = {0};

                for (int i = 0; i < nombreDeSommets; i++) {
                    distance[i] = INFINI;
                    precedent[i] = -1;
                }
                distance[source] = 0;

                for (int i = 0; i < nombreDeSommets - 1; i++) {
                    int minDistance = INFINI, u = -1;

                    for (int j = 0; j < nombreDeSommets; j++) {
                        if (!visite[j] && distance[j] < minDistance) {
                            minDistance = distance[j];
                            u = j;
                        }
                    }

                    if (u == -1) break;
                    visite[u] = 1;

                    for (int v = 0; v < nombreDeSommets; v++) {
                        if (!visite[v] && graphe[u][v] > 0 && distance[u] != INFINI &&
                            distance[u] + graphe[u][v] < distance[v]) {
                            distance[v] = distance[u] + graphe[u][v];
                            precedent[v] = u;
                        }
                    }
                }

                for (int v = destination; v != -1; v = precedent[v]) {
                    if (v != source && v != destination) {
                        centralite[v]++;
                    }
                }
            }
        }
    }

    printf("\nCentralite d'intermediarite :\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("Sommet %s : %d\n", sommets[i].nom, centralite[i]);
    }
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

    printf("Calcul des niveaux trophiques :\n");
    calculerNiveauxTrophiques(niveaux);

    printf("\nCalcul de la centralite d'intermediarite :\n");
    calculerCentraliteIntermediaire();

    return 0;
}
