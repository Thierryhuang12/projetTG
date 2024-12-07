#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nom[20];
    double population;
    double coefficientCroissance;
} Sommet;

Sommet sommets[] = {
        {"fourmis", 600, 0.5},
        {"cafard", 15, 0.3},
        {"moucheron", 3, 0.2},
        {"moustique", 3, 0.4},
        {"mouche", 3, 0.3},
        {"cloporte", 3, 0.2},
        {"guepe", 6, 0.4},
        {"poissonargent", 3, 0.5},
        {"punaiseassassine", 12, 0.3},
        {"araignee", 3, 0.1},
        {"chat", 3, 0.05},
        {"souris", 6, 0.1},
        {"acarien", 6000000, 0.1},
        {"homme", 3, 0.01},
        {"matiereorganique", 810, 0.3},
        {"restesalimentaire", 3000, 0.4},
        {"bois", 90, 0.5},
        {"nectar", 180, 0.6},
        {"poussiere", 5760, 0.05},
        {"graine", 390, 0.2}
};
#define N 21

double graphe[N][N] = {0};  // Matrice d'adjacence pour les relations de prédation

// Fonction pour charger les relations de prédation depuis le fichier
void chargerRelations(const char *fichier) {
    FILE *file = fopen(fichier, "r");
    if (!file) {
        printf("Erreur d'ouverture du fichier %s.\n", fichier);
        exit(1);
    }

    char origine[20], destination[20];
    int poids;
    while (fscanf(file, "%s %s %d", origine, destination, &poids) == 3) {
        int u = -1, v = -1;
        for (int i = 0; i < N; i++) {
            if (strcmp(sommets[i].nom, origine) == 0) u = i;
            if (strcmp(sommets[i].nom, destination) == 0) v = i;
        }
        if (u != -1 && v != -1) {
            graphe[u][v] = poids;
        }
    }

    fclose(file);
}

double calculerCapacite(int i) {
    return sommets[i].population * 10;
}

void reglerPopulation() {
    for (int i = 0; i < N; i++) {
        double K = calculerCapacite(i); // Capacité de portage
        double r = sommets[i].coefficientCroissance;
        double populationActuelle = sommets[i].population;
        double croissance = r * populationActuelle * (1 - populationActuelle / K); // Croissance logistique

        // Calcul de la réduction par prédation basée sur les relations dans graphe
        double reductionParPredateurs = 0;
        for (int j = 0; j < N; j++) {
            if (graphe[j][i] > 0) { // Si j est un prédateur de i
                reductionParPredateurs += graphe[j][i] * sommets[j].population / 100.0;
            }
        }

        printf("\n--- Sommet: %s ---\n", sommets[i].nom);
        printf("Population actuelle: %.2f\n", populationActuelle);
        printf("Capacité (K): %.2f\n", K);
        printf("Croissance: %.2f\n", croissance);
        printf("Réduction par Prédation: %.2f\n", reductionParPredateurs);

        // Calcul de la nouvelle population
        sommets[i].population += croissance - reductionParPredateurs;

        // Assurer que la population reste positive
        if (sommets[i].population < 0) {
            sommets[i].population = 0;
        }

        printf("Nouvelle population calculée: %.2f\n", sommets[i].population);
    }
}

int main() {
    // Charger les relations de prédation depuis graphe2.txt
    chargerRelations("graphe2.txt");

    printf("\n--- Calcul pour une seule itération ---\n");
    reglerPopulation(); // Applique le calcul de la population à chaque sommet
    return 0;
}
