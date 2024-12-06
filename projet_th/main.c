#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "bib.h"

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

void dijkstra(int source, int destination) {
    int distance[MAX_SOMMETS];
    int precedent[MAX_SOMMETS];
    int visite[MAX_SOMMETS] = {0};

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
            if (!visite[v] && graphe[u][v] && distance[u] != INFINI && distance[u] + graphe[u][v] < distance[v]) {
                distance[v] = distance[u] + graphe[u][v];
                precedent[v] = u;
            }
        }
    }

    if (distance[destination] == INFINI) {
        printf("Aucun chemin trouve de %s à %s.\n", sommets[source].nom, sommets[destination].nom);
        return;
    }

    printf("Chemin le plus court de %s a %s avec une distance de %d :\n", sommets[source].nom, sommets[destination].nom, distance[destination]);

    int chemin[MAX_SOMMETS], compte = 0;
    for (int v = destination; v != -1; v = precedent[v]) {
        chemin[compte++] = v;
    }

    for (int i = compte - 1; i >= 0; i--) {
        printf("%s", sommets[chemin[i]].nom);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

void afficherGraphe() {
    printf("\nGraphe entier :\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("%s : ", sommets[i].nom);
        int hasRelations = 0;
        for (int j = 0; j < nombreDeSommets; j++) {
            if (graphe[i][j] > 0) {
                printf("-> %s (poids: %d) ", sommets[j].nom, graphe[i][j]);
                hasRelations = 1;
            }
        }
        if (!hasRelations) printf("Aucune relation");
        printf("\n");
    }
}
void calculerDegre(int sommet) {
    int degreInterieur = 0, degreExterieur = 0;

    for (int i = 0; i < nombreDeSommets; i++) {
        if (graphe[i][sommet] > 0) degreInterieur++;
        if (graphe[sommet][i] > 0) degreExterieur++;
    }

    printf("Sommet %s : Degre interieur = %d, Degre exterieur = %d\n",
           sommets[sommet].nom, degreInterieur, degreExterieur);
}

// Calcul de la centralité d'intermédiarité -> sommets
void calculerCentraliteIntermediaire() {
    int centralite[MAX_SOMMETS] = {0};

    for (int source = 0; source < nombreDeSommets; source++) {
        for (int destination = 0; destination < nombreDeSommets; destination++) {
            if (source != destination) {
                //algo de Dijkstra pour trouve le plus petit
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
    int visite[MAX_SOMMETS] = {0}; // Marqueurs pour savoir si un sommet a été visité

    // Fonction récursive DFS pour calculer le niveau trophique d'un sommet
    void dfs(int sommet) {
        if (visite[sommet]) return; // Si déjà visité, on retourne
        visite[sommet] = 1;         // Marque comme visité

        int maxNiveau = 0;
        for (int i = 0; i < nombreDeSommets; i++) {
            if (graphe[sommet][i] > 0) { // Si i est un successeur du sommet
                dfs(i);                  // Calcul récursif pour i
                if (niveaux[i] > maxNiveau) {
                    maxNiveau = niveaux[i];
                }
            }
        }

        // Le niveau trophique est 1 + le max des niveaux de ses successeurs
        niveaux[sommet] = maxNiveau + 1;
    }

    // Initialisation des niveaux pour tous les sommets
    for (int i = 0; i < nombreDeSommets; i++) {
        if (!visite[i]) {
            dfs(i);
        }
    }

    // Normalisation pour fixer le maximum à un certain niveau (par ex. 3)
    int maxTrophique = 0;
    for (int i = 0; i < nombreDeSommets; i++) {
        if (niveaux[i] > maxTrophique) {
            maxTrophique = niveaux[i];
        }
    }

    for (int i = 0; i < nombreDeSommets; i++) {
        niveaux[i] = 4 - niveaux[i]; // Ajuste les niveaux pour que le maximum soit 3 et le minimum 1
    }
}


int main() {
    FILE *fichier = fopen("graphe.txt", "r");
    if (!fichier) {
        printf("Erreur d'ouverture du fichier.\n");
        return 1;
    }
    int niveaux[MAX_SOMMETS];
    char origine[20], destination[20];
    int poids;

    while (fscanf(fichier, "%s %s %d", origine, destination, &poids) == 3) {
        ajouterArc(origine, destination, poids);
    }

    fclose(fichier);

    while (1) {
        printf("\n\n ************************************\n\n");
        printf("\nMenu :\n");
        printf("1. Trouver le chemin le plus court\n");
        printf("2. Afficher predecesseurs et successeurs\n");
        printf("3. Afficher tout le graphe\n");
        printf("4. centralite d'intermediarite\n");
        printf("5. degre sommets\n");
        printf("6. Calcul des niveaux trophiques\n");
        printf("7. Quitter\n");

        printf("\n\n ************************************\n\n");
        printf("Votre choix : ");
        int choix;
        scanf("%d", &choix);

        if (choix == 7) break;

        if (choix == 1) {
            printf("\n\n ************************************\n\n");
            printf("Entrez le sommet de depart : ");
            scanf("%s", origine);
            printf("Entrez le sommet d'arrivee : ");
            scanf("%s", destination);

            int src = trouverIndexSommet(origine);
            int dest = trouverIndexSommet(destination);

            if (src == -1 || dest == -1) {
                printf("Sommets invalides.\n");
                continue;
            }

            dijkstra(src, dest);
        } else if (choix == 2) {
            printf("\n\n ************************************\n\n");
            printf("Entrez le sommet a explorer : ");
            scanf("%s", origine);

            int sommet = trouverIndexSommet(origine);
            if (sommet == -1) {
                printf("Sommet invalide.\n");
                continue;
            }

            afficherPredSucc(sommet);
        } else if (choix == 3) {
            printf("\n\n ************************************\n\n");
            afficherGraphe();
        }
        if (choix == 5) {
            printf("\n\n ************************************\n\n");
            printf("Entrez le nom du sommet : ");
            scanf("%s", origine);
            int sommet = trouverIndexSommet(origine);
            if (sommet == -1) {
                printf("Sommet invalide.\n");
                continue;
            }
            calculerDegre(sommet);
        } else if (choix == 4) {
            printf("\n\n ************************************\n\n");
            calculerCentraliteIntermediaire();

        }
        else if (choix == 6) {
            printf("\n\n ************************************\n\n");
            // Calcul et affichage des niveaux trophiques
            calculerNiveauxTrophiques(niveaux);
            printf("\nNiveaux trophiques :\n");
            for (int i = 0; i < nombreDeSommets; i++) {
                printf("Sommet %s : Niveau %d\n", sommets[i].nom, niveaux[i]);
            }
        }
        else {
           // printf("Choix invalide.\n");
        }
    }

    return 0;
}