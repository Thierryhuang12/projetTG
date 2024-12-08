#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_SOMMETS 50
#define INFINI 999999

typedef struct {
    char nom[20];
    double population;
    double coefficientCroissance;
} Sommet;

Sommet sommets[MAX_SOMMETS];
int nombreDeSommets = 0;
double graphe[MAX_SOMMETS][MAX_SOMMETS] = {0}; // Matrice d'adjacence

// Fonction pour trouver l'indice d'un sommet
int trouverIndexSommet(char *nom) {
    for (int i = 0; i < nombreDeSommets; i++) {
        if (strcmp(sommets[i].nom, nom) == 0) {
            return i;
        }
    }
    return -1;
}

// Charger les sommets et les arcs depuis un fichier texte
void chargerGrapheDepuisFichier(const char *nomFichier) {
    FILE *file = fopen(nomFichier, "r");
    if (!file) {
        printf("Erreur: impossible d'ouvrir le fichier %s.\n", nomFichier);
        exit(EXIT_FAILURE);
    }

    char ligne[256];
    int lectureArcs = 0;

    while (fgets(ligne, sizeof(ligne), file)) {
        if (ligne[0] == '#' || ligne[0] == '\n') continue;

        if (!lectureArcs) {
            char nom[20];
            double population, coefCroissance;
            if (sscanf(ligne, "%s %lf %lf", nom, &population, &coefCroissance) == 3) {
                strcpy(sommets[nombreDeSommets].nom, nom);
                sommets[nombreDeSommets].population = population;
                sommets[nombreDeSommets].coefficientCroissance = coefCroissance;
                nombreDeSommets++;
            } else {
                lectureArcs = 1;
            }
        }

        if (lectureArcs) {
            char origine[20], destination[20];
            int poids;
            if (sscanf(ligne, "%s %s %d", origine, destination, &poids) == 3) {
                int u = trouverIndexSommet(origine);
                int v = trouverIndexSommet(destination);
                if (u != -1 && v != -1) {
                    graphe[u][v] = poids;
                }
            }
        }
    }

    fclose(file);
    printf("Graphe chargé depuis le fichier %s avec succès.\n", nomFichier);
}
void afficherSommets() {
    printf("\n--- Sommets ---\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("Sommet: %s, Population: %.2f, Coeff Croissance: %.2f\n",
               sommets[i].nom, sommets[i].population, sommets[i].coefficientCroissance);
    }
}
void afficherGraphe() {
    printf("\n--- Graphe des relations de prédation ---\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("%s : ", sommets[i].nom);
        int hasRelations = 0;
        for (int j = 0; j < nombreDeSommets; j++) {
            if (graphe[i][j] > 0) {
                printf("-> %s (poids: %.0f) ", sommets[j].nom, graphe[i][j]);
                hasRelations = 1;
            }
        }
        if (!hasRelations) printf("Aucune relation");
        printf("\n");
    }
}
void reglerPopulation() {
    for (int i = 0; i < nombreDeSommets; i++) {
        double K = sommets[i].population * 10; // Capacité de portage
        double r = sommets[i].coefficientCroissance;
        double populationActuelle = sommets[i].population;
        double croissance = r * populationActuelle * (1 - populationActuelle / K); // Croissance logistique

        // Calcul de la réduction par prédation
        double reductionParPredateurs = 0;
        for (int j = 0; j < nombreDeSommets; j++) {
            if (graphe[j][i] > 0) { // Si j est un prédateur de i
                reductionParPredateurs += graphe[j][i] * sommets[j].population / 100.0;
            }
        }

        printf("\n--- Sommet: %s ---\n", sommets[i].nom);
        printf("Population actuelle: %.2f\n", populationActuelle);
        printf("Croissance: %.2f\n", croissance);
        printf("Réduction par prédation: %.2f\n", reductionParPredateurs);

        // Mise à jour de la population
        sommets[i].population += croissance - reductionParPredateurs;

        // Vérification que la population reste positive
        if (sommets[i].population < 0) {
            sommets[i].population = 0;
        }

        printf("Nouvelle population calculée: %.2f\n", sommets[i].population);
    }
}

void afficherPredateursProies(char *sommet) {
    int index = trouverIndexSommet(sommet);
    if (index == -1) {
        printf("Sommet introuvable : %s\n", sommet);
        return;
    }

    printf("\n--- Relations pour %s ---\n", sommet);

    printf("Prédécesseurs (prédateurs) : ");
    int hasPredecesseurs = 0;
    for (int i = 0; i < nombreDeSommets; i++) {
        if (graphe[i][index] > 0) {
            printf("%s (poids: %.0f) ", sommets[i].nom, graphe[i][index]);
            hasPredecesseurs = 1;
        }
    }
    if (!hasPredecesseurs) printf("Aucun");
    printf("\n");

    printf("Successeurs (proies) : ");
    int hasSuccesseurs = 0;
    for (int i = 0; i < nombreDeSommets; i++) {
        if (graphe[index][i] > 0) {
            printf("%s (poids: %.0f) ", sommets[i].nom, graphe[index][i]);
            hasSuccesseurs = 1;
        }
    }
    if (!hasSuccesseurs) printf("Aucune");
    printf("\n");
}

// Fonction Dijkstra pour trouver le plus court chemin
void dijkstra(int source, int destination) {
    int distance[MAX_SOMMETS];
    int precedent[MAX_SOMMETS];
    int visite[MAX_SOMMETS] = {0};

    for (int i = 0; i < nombreDeSommets; i++) {
        distance[i] = INFINI;
        precedent[i] = -1;
    }
    distance[source] = 0;

    for (int i = 0; i < nombreDeSommets; i++) {
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
            if (!visite[v] && graphe[u][v] && distance[u] != INFINI &&
                distance[u] + graphe[u][v] < distance[v]) {
                distance[v] = distance[u] + graphe[u][v];
                precedent[v] = u;
            }
        }
    }

    if (distance[destination] == INFINI) {
        printf("Aucun chemin trouvé de %s à %s.\n", sommets[source].nom, sommets[destination].nom);
        return;
    }

    printf("Chemin le plus court de %s à %s avec une distance de %d :\n",
           sommets[source].nom, sommets[destination].nom, distance[destination]);

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

// Calcul des niveaux trophiques
void calculerNiveauxTrophiques() {
    int niveaux[MAX_SOMMETS] = {0};
    int visite[MAX_SOMMETS] = {0};

    void dfs(int sommet) {
        if (visite[sommet]) return;
        visite[sommet] = 1;

        int maxNiveau = 0;
        for (int i = 0; i < nombreDeSommets; i++) {
            if (graphe[sommet][i] > 0) {
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

    printf("\n--- Niveaux trophiques ---\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("Sommet %s : Niveau trophique %d\n", sommets[i].nom, niveaux[i]);
    }
}

// Calcul de la centralité d'intermédiarité
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

    printf("\n--- Centralité d'intermédiarité ---\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("Sommet %s : Centralité %d\n", sommets[i].nom, centralite[i]);
    }
}
void calculerDegre(char *sommet) {
    int index = trouverIndexSommet(sommet);
    if (index == -1) {
        printf("Sommet introuvable : %s\n", sommet);
        return;
    }

    int degreEntrant = 0, degreSortant = 0;
    for (int i = 0; i < nombreDeSommets; i++) {
        if (graphe[i][index] > 0) degreEntrant++;
        if (graphe[index][i] > 0) degreSortant++;
    }

    printf("\n--- Degré du sommet %s ---\n", sommet);
    printf("Degré entrant : %d\n", degreEntrant);
    printf("Degré sortant : %d\n", degreSortant);
}

// Générer un fichier DOT pour visualisation
void genererDotFile(const char *nomFichier) {
    FILE *file = fopen(nomFichier, "w");
    if (!file) {
        printf("Erreur d'ouverture du fichier DOT.\n");
        return;
    }

    fprintf(file, "digraph graphe {\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        for (int j = 0; j < nombreDeSommets; j++) {
            if (graphe[i][j] > 0) {
                fprintf(file, "    %s -> %s [label=\"%.0f\"];\n",
                        sommets[i].nom, sommets[j].nom, graphe[i][j]);
            }
        }
    }
    fprintf(file, "}\n");
    fclose(file);
    printf("Fichier DOT généré : %s\n", nomFichier);
}

// Main
int main() {
    char nomFichier[50];
    printf("Entrez le nom du fichier contenant le graphe (par ex. graphe.txt) : ");
    scanf("%s", nomFichier);

    chargerGrapheDepuisFichier(nomFichier);

    int choix;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Afficher les sommets\n");
        printf("2. Afficher le graphe\n");
        printf("3. Calculer une itération de population\n");
        printf("4. Trouver le chemin le plus court\n");
        printf("5. Générer un fichier DOT\n");
        printf("6. Afficher les prédateurs et proies d'un sommet\n");
        printf("7. Calculer le degré d'un sommet\n");
        printf("8. Calculer les niveaux trophiques\n");
        printf("9. Calculer la centralité d'intermédiarité\n");
        printf("10. Quitter\n");
        printf("Votre choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                afficherSommets();
                break;
            case 2:
                afficherGraphe();
                break;
            case 3:
                reglerPopulation();
                break;
            case 4: {
                char sommet1[20], sommet2[20];
                printf("Entrez le sommet de départ : ");
                scanf("%s", sommet1);
                printf("Entrez le sommet d'arrivée : ");
                scanf("%s", sommet2);

                int src = trouverIndexSommet(sommet1);
                int dest = trouverIndexSommet(sommet2);

                if (src == -1 || dest == -1) {
                    printf("Sommets invalides.\n");
                } else {
                    dijkstra(src, dest);
                }
                break;
            }
            case 5:
                genererDotFile("graphe.dot");
                break;
            case 6: {
                char sommet[20];
                printf("Entrez le sommet : ");
                scanf("%s", sommet);
                afficherPredateursProies(sommet);
                break;
            }
            case 7: {
                char sommet[20];
                printf("Entrez le sommet : ");
                scanf("%s", sommet);
                calculerDegre(sommet);
                break;
            }
            case 8:
                calculerNiveauxTrophiques();
                break;
            case 9:
                calculerCentraliteIntermediaire();
                break;
            case 10:
                printf("Au revoir!\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 10);

    return 0;
}
