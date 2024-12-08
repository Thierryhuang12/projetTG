#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <windows.h>

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
void color(int texte, int fond) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (fond << 4) | texte);
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
    printf("Graphe charge depuis le fichier %s avec succes.\n", nomFichier);
}
void afficherSommets() {
    printf("\n--- Sommets ---\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("Sommet: %s, Population: %.2f, Coeff Croissance: %.2f\n",
               sommets[i].nom, sommets[i].population, sommets[i].coefficientCroissance);
    }
}
void afficherGraphe() {
    printf("\n--- Graphe des relations de predation ---\n");
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
void reglerPopulationAvecNiveaux(int niveaux[], double tauxCroissanceNiveau1) {
    for (int i = 0; i < nombreDeSommets; i++) {
        double K = 0;
        double croissance = 0;
        double populationActuelle = sommets[i].population;

        // Gestion des niveaux trophiques 1
        if (niveaux[i] == 1) {
            double r = tauxCroissanceNiveau1;
            croissance = r * populationActuelle; // Croissance linéaire
        } else {
            // Calcul de K pour les autres niveaux
            for (int j = 0; j < nombreDeSommets; j++) {
                if (graphe[j][i] > 0) {
                    K += graphe[j][i] * sommets[j].population / 100.0;
                }
            }

            // Appliquer un seuil minimum pour K
            if (K < populationActuelle) {
                K = populationActuelle * 2;
            }

            double r = sommets[i].coefficientCroissance;
            croissance = r * populationActuelle * (1 - populationActuelle / K);
        }

        // Calcul de la réduction par prédation
        double reductionParPredateurs = 0;
        for (int j = 0; j < nombreDeSommets; j++) {
            if (graphe[j][i] > 0) {
                reductionParPredateurs += graphe[j][i] * sommets[j].population / 100.0;
            }
        }

        // Limiter la réduction par prédation
        if (reductionParPredateurs > populationActuelle * 0.9) {
            reductionParPredateurs = populationActuelle * 0.9;
        }

        // Affichage des informations pour chaque sommet
        printf("\n--- Sommet: %s ---\n", sommets[i].nom);
        printf("Population actuelle: %.2f\n", populationActuelle);
        if (niveaux[i] != 1) {
            printf("Capacité (K): %.2f\n", K);
        }
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

    printf("Predecesseurs (predateurs) : ");
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
        printf("Aucun chemin trouve de %s a %s.\n", sommets[source].nom, sommets[destination].nom);
        return;
    }

    printf("Chemin le plus court de %s a %s :\n",
           sommets[source].nom, sommets[destination].nom);

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
void calculerNiveauxTrophiques(int niveaux[]) {
    int visite[MAX_SOMMETS] = {0};

    void dfs(int sommet) {
        if (visite[sommet]) return;
        visite[sommet] = 1;

        int maxNiveau = 0;
        for (int i = 0; i < nombreDeSommets; i++) {
            if (graphe[sommet][i] > 0) { // Si sommet a une relation avec i
                dfs(i);
                if (niveaux[i] > maxNiveau) {
                    maxNiveau = niveaux[i];
                }
            }
        }
        niveaux[sommet] = maxNiveau + 1;
    }

    // Réinitialiser les niveaux avant recalcul
    for (int i = 0; i < nombreDeSommets; i++) {
        niveaux[i] = 0;
    }

    // Calculer les niveaux trophiques pour chaque sommet
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

    printf("\n--- Centralite d'intermédiarite---\n");
    for (int i = 0; i < nombreDeSommets; i++) {
        printf("Sommet %s : Centralite %d\n", sommets[i].nom, centralite[i]);
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

    printf("\n--- Degre du sommet %s ---\n", sommet);
    printf("Degre entrant : %d\n", degreEntrant);
    printf("Degre sortant : %d\n", degreSortant);
}

// Générer un fichier DOT pour visualisation
void genererDotFile() {
    char nomFichier[50];
    printf("Entrez le nom du fichier DOT e generer (par ex. graphe.dot) : ");
    scanf("%s", nomFichier);

    // Vérifie si l'extension .dot est présente
    if (strstr(nomFichier, ".dot") == NULL) {
        strcat(nomFichier, ".dot"); // Ajoute .dot si absent
    }

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
    printf("Fichier DOT genere : %s\n", nomFichier);
}


// Fonction pour vider le tampon
void viderTampon() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}



void genererRapport(int iteration, int niveaux[], const char *nomFichierRapport) {
    FILE *file = fopen(nomFichierRapport, "a"); // Ouvrir en mode ajout
    if (!file) {
        printf("Erreur: impossible de créer le fichier %s.\n", nomFichierRapport);
        return;
    }

    fprintf(file, "\n--- Rapport après l'itération %d ---\n", iteration);
    for (int i = 0; i < nombreDeSommets; i++) {
        fprintf(file, "Sommet: %s\n", sommets[i].nom);
        fprintf(file, "  Niveau trophique: %d\n", niveaux[i]);
        fprintf(file, "  Population actuelle: %.2f\n", sommets[i].population);
        fprintf(file, "  Coefficient de croissance: %.2f\n", sommets[i].coefficientCroissance);
        fprintf(file, "----------------------\n");
    }

    fclose(file);
    printf("Rapport généré : %s\n", nomFichierRapport);
}

int main() {
    char nomFichier[50];
    int iteration = 1;
    char buffer[10];
    FILE *testFile;

    // Instructions pour l'utilisateur
    color(7, 0);
    printf("Entrez le nom du fichier contenant le reseau trophique :\n");
    color(4, 0);
    printf("\nTapez graphe.txt Pour la Savane\n");
    color(2, 0);
    printf("\nTapez graphe2.txt Pour la Maison\n");
    color(6, 0);
    printf("\nTapez graphe3.txt Pour la Foret\n");
    color(7, 0);

    // Saisie sécurisée pour le choix de fichier
    do {
        color(3,0);
        printf("\n\nVotre choix : ");
        color(7, 0);
        if (fgets(nomFichier, sizeof(nomFichier), stdin) != NULL) {
            nomFichier[strcspn(nomFichier, "\n")] = '\0'; // Supprimer le retour à la ligne
        }

        testFile = fopen(nomFichier, "r");
        if (!testFile) {
            printf("Erreur : graphe non existant. Veuillez réessayer.\n");
        }
    } while (!testFile);
    fclose(testFile);

    // Charger le fichier sélectionné
    chargerGrapheDepuisFichier(nomFichier);

    // Saisie du coefficient de croissance
    double tauxCroissanceNiveau1;
    int saisieValide = 0;
    do {
        printf("Entrez le taux de croissance constant pour les niveaux trophiques 1 : ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL && sscanf(buffer, "%lf", &tauxCroissanceNiveau1) == 1 && tauxCroissanceNiveau1 > 0) {
            saisieValide = 1;
        } else {
            printf("Erreur : coefficient impossible. Veuillez entrer un nombre valide.\n");
        }
    } while (!saisieValide);

    // Calculer les niveaux trophiques
    int niveaux[MAX_SOMMETS] = {0};
    calculerNiveauxTrophiques(niveaux);

    int choix;
    do {
        color(3,0);
        printf("\n\n************************************************************");
        printf("\n--- Menu ---\n");
        color(6, 0);
        printf("\n1. Afficher les sommets\n");
        printf("\n2. Afficher le graphe\n");
        printf("\n3. Calculer une iteration de population\n");
        printf("\n4. Trouver le chemin le plus court\n");
        printf("\n5. Generer un fichier DOT\n");
        printf("\n6. Afficher les predateurs et proies d'un sommet\n");
        printf("\n7. Calculer le degre d'un sommet\n");
        printf("\n8. Calculer les niveaux trophiques\n");
        printf("\n9. Calculer la centralite d'intermediarite\n");
        printf("\n10. Quitter\n");
        color(2, 0);
        printf("\n\nVotre choix: ");
        color(7,0);

        // Lecture sécurisée du choix
        if (fgets(buffer, sizeof(buffer), stdin) != NULL && sscanf(buffer, "%d", &choix) == 1) {
            switch (choix) {
                case 1:
                    afficherSommets();
                    break;
                case 2:
                    afficherGraphe();
                    break;
                case 3: {
                    char nomFichierRapport[50];
                    printf("Entrez le nom du fichier de rapport (ex. rapport_population.txt) : ");
                    if (fgets(nomFichierRapport, sizeof(nomFichierRapport), stdin) != NULL) {
                        nomFichierRapport[strcspn(nomFichierRapport, "\n")] = '\0'; // Supprimer le retour à la ligne
                    }
                    reglerPopulationAvecNiveaux(niveaux, tauxCroissanceNiveau1);
                    genererRapport(iteration++, niveaux, nomFichierRapport);
                    break;
                }
                case 4: {
                    char sommet1[20], sommet2[20];
                    printf("Entrez le sommet de depart : ");
                    if (fgets(sommet1, sizeof(sommet1), stdin) != NULL) {
                        sommet1[strcspn(sommet1, "\n")] = '\0';
                    }
                    printf("Entrez le sommet d'arrivee : ");
                    if (fgets(sommet2, sizeof(sommet2), stdin) != NULL) {
                        sommet2[strcspn(sommet2, "\n")] = '\0';
                    }

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
                    genererDotFile();
                    break;
                case 6: {
                    char sommet[20];
                    printf("Entrez le sommet : ");
                    if (fgets(sommet, sizeof(sommet), stdin) != NULL) {
                        sommet[strcspn(sommet, "\n")] = '\0';
                    }
                    afficherPredateursProies(sommet);
                    break;
                }
                case 7: {
                    char sommet[20];
                    printf("Entrez le sommet : ");
                    if (fgets(sommet, sizeof(sommet), stdin) != NULL) {
                        sommet[strcspn(sommet, "\n")] = '\0';
                    }
                    calculerDegre(sommet);
                    break;
                }
                case 8:
                    calculerNiveauxTrophiques(niveaux);
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
        } else {
            printf("Choix invalide.\n");
        }
    } while (choix != 10);

    return 0;
}
