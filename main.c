#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//O(n)
void free_matrice(int** matrice, int n){
    for(int i = 0; i < n; i++)
        free(matrice[i]);
    free(matrice);
}

//O(n^2)
void print_matrice(int** matrice, int n){
    if(matrice == NULL){
        printf("Matrice NULL\n");
        return;
    }
    for(int i = 0; i < n; i++){
        printf("[ ");
        for(int j = 0; j < n; j++){
            printf("%d ", matrice[i][j]);
        }
        printf("]\n");
    }
}

//O(n^4)
int** creer_matrice_passage(int n){
    int size = n*n;
    int** A = malloc(sizeof(int*)*size);
    for(int i = 0; i < size; i++){
        A[i] = malloc(sizeof(int)*size);
        for(int j = 0; j < size; j++){
            A[i][j] = 0;
        }
    }
    for(int i = 0; i < size; i++)
    {
        int row = i / n;
        int col = i % n;
        A[i][i] = 1; //On allume la case elle-même
        if(row > 0)
            A[i][i - n] = 1; //La cellule au dessus
        if(row < n - 1)
            A[i][i + n] = 1; //La cellule en dessous
        if(col > 0)
            A[i][i - 1] = 1; //La cellule à gauche
        if(col < n - 1)
            A[i][i + 1] = 1; //La cellule à droite
    }
    return A;
}

//O(1)
void echange_lignes(int** A, int* b, int row1, int row2, int n) {
    int* temp_row = A[row1];
    A[row1] = A[row2];
    A[row2] = temp_row;

    int temp_b = b[row1];
    b[row1] = b[row2];
    b[row2] = temp_b;
}

//Résoud Ax = b, et si il existe plusieurs solutions, ça prend la plus "simple" (0 pour coeffs libres)
//O(n^3), attention ici n = |b|, donc ça fera O(n^6) pour n la taille de la grille
int* gaussian_elim(int** A, int* b, int n) {
    // Phase d'élimination, on triangularise le truc
    for (int i = 0; i < n; ++i) {
        // On trouve la ligne de pivot
        int max_row = i;
        for (int k = i + 1; k < n; ++k) {
            if (abs(A[k][i]) > abs(A[max_row][i])) { //Il y a des valeurs négatives des fois.... il faudrait que je répare ça
                max_row = k;
            }
        }

        if (A[max_row][i] == 0) {
            continue; // On saute si la colonne est 0
        }

        // On échange avec la ligne actuelle
        if (max_row != i) {
            echange_lignes(A, b, i, max_row, n);
        }

        // On tue tout ce qui est en dessous du pivot
        for (int j = i + 1; j < n; ++j) {
            if (A[j][i] != 0) {
                int factor = A[j][i] / A[i][i]; // J'ai l'habitude de faire ça, mais là c'est plus du tout un entier?? ah si, c'est 0 ou 1, at A[i][i] != 0
                for (int k = i; k < n; ++k) {
                    A[j][k] = (A[j][k] - factor * A[i][k]) % 2;
                }
                b[j] = (b[j] - factor * b[i]) % 2;
            }
        }
    }

    // On rebsustitue tout, et c'est gagné
    int* x = (int*)calloc(n, sizeof(int));
    for (int i = n - 1; i >= 0; --i) {
        if (A[i][i] == 0) {
            if (b[i] != 0) {
                free(x);
                return NULL; // Aucune solution
            }
            continue; // On a une variable 'libre', on la laisse à 0 parceque pourquoi pas
        }
        x[i] = b[i];
        for (int j = i + 1; j < n; ++j) {
            x[i] = (x[i] - A[i][j] * x[j]) % 2;
        }
    }

    return x;
}

// Utile pour faire des systèmes avec vecteurs
//O(n^2)
void aplatir_matrice(int** mat, int* vec, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            vec[i * n + j] = mat[i][j];
        }
    }
}

//à partir d'un état initial, on trouve les cases à presser (sans ordre) pour gagner
//O(n^6)
int** solve_matrice(int** initial_state, int n) {
    int** A = creer_matrice_passage(n);
    int* p = (int*)malloc(n * n * sizeof(int));
    aplatir_matrice(initial_state, p, n);

    int* b = (int*)malloc(n * n * sizeof(int));

    for (int i = 0; i < n * n; ++i) {
        b[i] = 1 - p[i]; //Ici le 1 c'est parce que on veut finir avec (1,1,...,1)
    }

    int* solution_vector = gaussian_elim(A, b, n * n);
    free_matrice(A, n*n);
    free(p);
    free(b);
    if (solution_vector == NULL) {
        return NULL;
    }
    
    int** solution_matrix = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        solution_matrix[i] = (int*)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            solution_matrix[i][j] = abs((int)solution_vector[i * n + j] % 2); //Il y a des négatifs et tout
        }
    }

    free(solution_vector);

    return solution_matrix;
}

//O(n^2)
void print_vector(int* vector, int n){
    for(int i = 0; i < n*n; i++){
        printf("%d ", vector[i]);
        if((i+1) % n == 0)
            printf("\n");
    }
    printf("\n");
}


void main(){
    int n = 5;
    int initial_state[5][5] = { {1,0,1,1,1},
                                {1,1,0,0,0},
                                {0,1,1,1,1},
                                {1,0,0,0,1},
                                {1,1,0,1,1}};

    //Ce truc dégueu c'est pour l'avoir en mémoire dynamique même si c'est pas trop utile
    int** initial_state_ptr = malloc(sizeof(int*)*n);
    for(int i = 0; i < n; i++){
        initial_state_ptr[i] = malloc(sizeof(int)*n);
        for(int j = 0; j < n; j++)
            initial_state_ptr[i][j] = initial_state[i][j];
    }

    int** solution = solve_matrice(initial_state_ptr, n);
    if(solution)
        print_matrice(solution, n);
    else
        printf("Pas de solution pour ce système.\n");

    free_matrice(initial_state_ptr, n);
    free_matrice(solution, n);
}