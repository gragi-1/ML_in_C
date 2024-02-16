#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    float sepal_length;
    float sepal_width;
    float petal_length;
    float petal_width;
    char class_name[50];
} Iris;

// Function to read the data from the file and return the data as an array of Iris structures and the count of the data
Iris* read_data(char* filename, int* count)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Could not open file %s\n", filename);
        return NULL;
    }
    Iris* data = (Iris*)malloc(150 * sizeof(Iris));  // The Iris dataset has 150 instances
    *count = 0;
    while (fscanf(file, "%f,%f,%f,%f,%s", &data[*count].sepal_length, &data[*count].sepal_width, &data[*count].petal_length, &data[*count].petal_width, data[*count].class_name) == 5)
    {
        (*count)++;
    }
    fclose(file);
    return (data);
}

// Function to shuffle the data using the Fisher-Yates algorithm to avoid any bias
void shuffle_data(Iris* data, int count)
{
    srand(time(NULL));
    for (int i = count - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Iris temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }
}

// Function to split the data into training and testing data using a 70-30 split
void split_data(Iris* data, int count, Iris** train, int* train_count, Iris** test, int* test_count)
{
    *train_count = (int)(0.7 * count);
    *test_count = count - *train_count;
    *train = (Iris*)malloc(*train_count * sizeof(Iris));
    *test = (Iris*)malloc(*test_count * sizeof(Iris));
    for (int i = 0; i < *train_count; i++)
    {
        (*train)[i] = data[i];
    }
    for (int i = 0; i < *test_count; i++)
    {
        (*test)[i] = data[i + *train_count];
    }
}

// Function to normalize the data using the z-score normalization technique
void normalize_data(Iris* data, int count)
{
    float mean[4] = {0, 0, 0, 0};
    float std_dev[4] = {0, 0, 0, 0};
    for (int i = 0; i < count; i++)
    {
        mean[0] += data[i].sepal_length;
        mean[1] += data[i].sepal_width;
        mean[2] += data[i].petal_length;
        mean[3] += data[i].petal_width;
    }
    for (int i = 0; i < 4; i++)
    {
        mean[i] /= count;
    }
    for (int i = 0; i < count; i++)
    {
        std_dev[0] += pow(data[i].sepal_length - mean[0], 2);
        std_dev[1] += pow(data[i].sepal_width - mean[1], 2);
        std_dev[2] += pow(data[i].petal_length - mean[2], 2);
        std_dev[3] += pow(data[i].petal_width - mean[3], 2);
    }
    for (int i = 0; i < 4; i++)
    {
        std_dev[i] = sqrt(std_dev[i] / count);
    }
    for (int i = 0; i < count; i++)
    {
        data[i].sepal_length = (data[i].sepal_length - mean[0]) / std_dev[0];
        data[i].sepal_width = (data[i].sepal_width - mean[1]) / std_dev[1];
        data[i].petal_length = (data[i].petal_length - mean[2]) / std_dev[2];
        data[i].petal_width = (data[i].petal_width - mean[3]) / std_dev[3];
    }
}

// Function to calculate the Euclidean distance between two data points
double calculate_distance(Iris* a, Iris* b)
{
    double sum = pow(a->sepal_length - b->sepal_length, 2)
               + pow(a->sepal_width - b->sepal_width, 2)
               + pow(a->petal_length - b->petal_length, 2)
               + pow(a->petal_width - b->petal_width, 2);
    return (sqrt(sum));
}

// Function to find the k-nearest neighbors of the query data in the training data and return the neighbors
void find_k_nearest_neighbors(Iris* train, int train_count, Iris* query, int k, Iris** neighbors)
{
    double* distances = (double*)malloc(train_count * sizeof(double));
    for (int i = 0; i < train_count; i++)
    {
        distances[i] = calculate_distance(&train[i], query);
    }
    for (int i = 0; i < k; i++)
    {
        int min_index = i;
        for (int j = i + 1; j < train_count; j++)
        {
            if (distances[j] < distances[min_index])
                min_index = j;
        }
        neighbors[i] = &train[min_index];
        distances[min_index] = distances[i];
    }
    free(distances);
}

// Function to classify the query data using the k-nearest neighbors algorithm and return the class
char* classify(Iris* train, int train_count, Iris* query, int k)
{
    Iris** neighbors = (Iris**)malloc(k * sizeof(Iris*));
    find_k_nearest_neighbors(train, train_count, query, k, neighbors);
    int max_count = 0;
    char* class = NULL;
    for (int i = 0; i < k; i++)
    {
        int count = 0;
        for (int j = 0; j < k; j++)
        {
            if (strcmp(neighbors[i]->class_name, neighbors[j]->class_name) == 0)
                count++;
        }
        if (count > max_count)
        {
            max_count = count;
            class = neighbors[i]->class_name;
        }
    }
    free(neighbors);
    return (class);
}

// Function to evaluate the accuracy of the model using the test data and k value
double evaluate(Iris* train, int train_count, Iris* test, int test_count, int k)
{
    int correct_predictions = 0;
    for (int i = 0; i < test_count; i++)
    {
        char* predicted_class = classify(train, train_count, &test[i], k);
        if (strcmp(predicted_class, test[i].class_name) == 0)
            correct_predictions++;
    }
    return ((double)correct_predictions / test_count);
}

// Function to write the results to a CSV file
void write_results_to_csv(Iris* test, int test_count, Iris* train, int train_count, int k, const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Could not open file %s\n", filename);
        return;
    }
    fprintf(file, "sepal_length,sepal_width,petal_length,petal_width,actual_class,predicted_class\n");
    for (int i = 0; i < test_count; i++)
    {
        char* predicted_class = classify(train, train_count, &test[i], k);
        fprintf(file, "%f,%f,%f,%f,%s,%s\n", test[i].sepal_length, test[i].sepal_width, test[i].petal_length, test[i].petal_width, test[i].class_name, predicted_class);
    }
    fclose(file);
}

// Function to plot the data using gnuplot (Bar chart)
void plot_data() {
    FILE *pipe = popen("gnuplot", "w");

    if (pipe) {                                                             // If the pipe is non-null, then it's open
        fprintf(pipe, "set terminal pngcairo\n");                           // Set the terminal to PNG
        fprintf(pipe, "set output 'bar_plot.png'\n");                       // Set the output file name
        fprintf(pipe, "set title 'Bar Plot'\n");                            // Set the title of the plot
        fprintf(pipe, "set style data histogram\n");                        // Set the style of the plot to histogram
        fprintf(pipe, "set style histogram cluster gap 1\n");               // Set the style of the histogram
        fprintf(pipe, "set style fill solid\n");                            // Set the style of the fill
        fprintf(pipe, "set xlabel 'Feature'\n");                            // Set the x-axis label
        fprintf(pipe, "set ylabel 'Average Value'\n");                      // Set the y-axis label
        fprintf(pipe, "plot 'averages.data' using 2:xtic(1) with boxes\n"); // Plot the data
        fflush(pipe);                                                       // Flush the pipe to make sure the plot is saved
    }

    pclose(pipe);
}

// MAIN Function free the memory and call the functions
int main()
{
    int count;
    Iris* data = read_data("iris.data", &count);
    if (data == NULL)
    {
        printf("No se pudo abrir el archivo\n");
        return (1);
    }

    shuffle_data(data, count);

    Iris *train, *test;
    int train_count, test_count;
    split_data(data, count, &train, &train_count, &test, &test_count);

    normalize_data(train, train_count);
    normalize_data(test, test_count);

    int k = 3;
    double accuracy = evaluate(train, train_count, test, test_count, k);
    printf("Accuracy: %.2f%%\n", accuracy * 100);
    write_results_to_csv(test, test_count, train, train_count, k, "results.csv");
    plot_data();

    free(data);
    free(train);
    free(test);
    return 0;
}