#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define MAX_ITER 100

// Función para calcular la distancia euclidiana entre dos puntos
double calcular_distancia(double *punto1, double *punto2, int num_caracteristicas) {
    double sum = 0;
    for (int i = 0; i < num_caracteristicas; i++) {
        double diff = punto1[i] - punto2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

// Función para inicializar los centroides iniciales
void inicializar_centroides(double **datos, int num_datos, int num_caracteristicas, int k, double **centroides) {
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < num_caracteristicas; j++) {
            centroides[i][j] = datos[i][j];
        }
    }
}

// Función para asignar cada punto al centroide más cercano
void asignar_puntos_a_centroides(double **datos, int num_datos, int num_caracteristicas, double **centroides, int *grupos, int k) {
    for (int i = 0; i < num_datos; i++) {
        double min_dist = DBL_MAX;
        for (int j = 0; j < k; j++) {
            double dist = calcular_distancia(datos[i], centroides[j], num_caracteristicas);
            if (dist < min_dist) {
                min_dist = dist;
                grupos[i] = j;
            }
        }
    }
}

// Función para recalcular los centroides
void recalcular_centroides(double **datos, int num_datos, int num_caracteristicas, int *grupos, int k, double **centroides) {
    int *counts = (int *)calloc(k, sizeof(int));
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < num_caracteristicas; j++) {
            centroides[i][j] = 0;
        }
    }

    for (int i = 0; i < num_datos; i++) {
        int group = grupos[i];
        counts[group]++;
        for (int j = 0; j < num_caracteristicas; j++) {
            centroides[group][j] += datos[i][j];
        }
    }

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < num_caracteristicas; j++) {
            centroides[i][j] /= counts[i];
        }
    }

    free(counts);
}

// Función para calcular la distancia promedio a todos los puntos en el mismo grupo
double average_distance(double **datos, int *grupos, int num_datos, int num_caracteristicas, int index, int cluster) {
    double sum = 0;
    int count = 0;
    for (int i = 0; i < num_datos; i++) {
        if (grupos[i] == cluster) {
            sum += calcular_distancia(datos[index], datos[i], num_caracteristicas);
            count++;
        }
    }
    return sum / count;
}

// Función para calcular el silhouette score
double silhouette_score(double **datos, int *grupos, int num_datos, int num_caracteristicas, int k) {
    double* a = malloc(num_datos * sizeof(double));
    double* b = malloc(num_datos * sizeof(double));
    for (int i = 0; i < num_datos; i++) {
        a[i] = average_distance(datos, grupos, num_datos, num_caracteristicas, i, grupos[i]);
        double min_average_distance = DBL_MAX;
        for (int j = 0; j < k; j++) {
            if (j != grupos[i]) {
                double average_distance_to_cluster = average_distance(datos, grupos, num_datos, num_caracteristicas, i, j);
                if (average_distance_to_cluster < min_average_distance) {
                    min_average_distance = average_distance_to_cluster;
                }
            }
        }
        b[i] = min_average_distance;
    }
    double silhouette_sum = 0;
    for (int i = 0; i < num_datos; i++) {
        silhouette_sum += (b[i] - a[i]) / (a[i] > b[i] ? a[i] : b[i]);
    }
    free(a);
    free(b);
    return silhouette_sum / num_datos;
}

// Función principal del algoritmo K-Means
void k_means(double **datos, int num_datos, int num_caracteristicas, int k) {
    // Inicializa los centroides iniciales
    double **centroides = (double **)malloc(k * sizeof(double *));
    for (int i = 0; i < k; ++i) {
        centroides[i] = (double *)malloc(num_caracteristicas * sizeof(double));
    }
    inicializar_centroides(datos, num_datos, num_caracteristicas, k, centroides);

    // Inicializa el arreglo de grupos
    int *grupos = (int *)malloc(num_datos * sizeof(int));

    // Repite hasta convergencia
    for (int iter = 0; iter < MAX_ITER; iter++) {
        // Asigna puntos a centroides
        asignar_puntos_a_centroides(datos, num_datos, num_caracteristicas, centroides, grupos, k);

        // Recalcula centroides
        recalcular_centroides(datos, num_datos, num_caracteristicas, grupos, k, centroides);
    }

    // Liberar memoria
    for (int i = 0; i < k; ++i) {
        free(centroides[i]);
    }
    free(centroides);
    free(grupos);
}

// Función para leer los datos del archivo iris.data
double **leer_datos(char *filename, int *num_datos, int *num_caracteristicas) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("No se pudo abrir el archivo %s\n", filename);
        exit(1);
    }

    // Contar el número de líneas en el archivo para determinar el número de datos
    char ch;
    *num_datos = 0;
    while(!feof(file)) {
        ch = fgetc(file);
        if(ch == '\n') {
            (*num_datos)++;
        }
    }

    // Establecer el número de características (4 para el conjunto de datos Iris)
    *num_caracteristicas = 4;

    // Asignar memoria para los datos
    double **datos = (double **)malloc(*num_datos * sizeof(double *));
    for (int i = 0; i < *num_datos; i++) {
        datos[i] = (double *)malloc(*num_caracteristicas * sizeof(double));
    }

    // Reiniciar el indicador de posición del archivo al principio del archivo
    rewind(file);

    // Leer los datos del archivo
    for (int i = 0; i < *num_datos; i++) {
        for (int j = 0; j < *num_caracteristicas; j++) {
            if (!fscanf(file, "%lf,", &datos[i][j])) {
                break;
            }
        }
        // Saltar la especie de la flor
        fscanf(file, "%*s\n");
    }

    fclose(file);

    return datos;
}

void write_to_csv(double **datos, int *grupos, int num_datos, int num_caracteristicas, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Write data and cluster assignments to file
    for (int i = 0; i < num_datos; i++) {
        for (int j = 0; j < num_caracteristicas; j++) {
            fprintf(file, "%f,", datos[i][j]);
        }
        fprintf(file, "%d\n", grupos[i]);
    }

    fclose(file);
}

// Función principal
int main() {
    // Read data from the Iris dataset
    int num_datos, num_caracteristicas;
    double **datos = leer_datos("iris.data", &num_datos, &num_caracteristicas);

    // Define the range of k values to test
    int k_min = 2;
    int k_max = 10;

    // Allocate memory for cluster assignments
    int *grupos = malloc(num_datos * sizeof(int));

    // Run K-Means for each k and compute the silhouette score
    for (int k = k_min; k <= k_max; k++) {
        // Apply the K-Means algorithm
        k_means(datos, num_datos, num_caracteristicas, k);

        // Compute the silhouette score
        double score = silhouette_score(datos, grupos, num_datos, num_caracteristicas, k);

        // Print the silhouette score for this k
        printf("Silhouette score for k=%d: %f\n", k, score);
    }

    // Write data and cluster assignments to CSV file
    write_to_csv(datos, grupos, num_datos, num_caracteristicas, "value_of_assignments_and_clusters.csv");

    // Free memory
    free(grupos);
    for (int i = 0; i < num_datos; ++i) {
        free(datos[i]);
    }
    free(datos);

    return 0;
}