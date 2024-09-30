#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    int base;
    char value[100];
} Root;

int decode_y_value(const char *y, int base) {
    int value = 0, i, len = strlen(y);
    for (i = 0; i < len; i++) {
        value *= base;
        value += (y[i] >= '0' && y[i] <= '9') ? y[i] - '0' : y[i] - 'A' + 10;
    }
    return value;
}

double lagrange_interpolation(int n, int *x_values, int *y_values, int x) {
    double result = 0.0;
    for (int i = 0; i < n; i++) {
        double l_i = 1.0;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                l_i *= (double)(x - x_values[j]) / (double)(x_values[i] - x_values[j]);
            }
        }
        result += (double)y_values[i] * l_i;
    }
    return result;
}

void matrix_method(int n, int *x_values, int *y_values, double *coefficients) {
    double **matrix = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double *)malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            matrix[i][j] = pow(x_values[i], j);
        }
    }

    // Gaussian elimination
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double factor = matrix[j][i] / matrix[i][i];
            for (int k = i; k < n; k++) {
                matrix[j][k] -= factor * matrix[i][k];
            }
            y_values[j] -= factor * y_values[i];
        }
    }

    for (int i = n - 1; i >= 0; i--) {
        coefficients[i] = y_values[i];
        for (int j = i - 1; j >= 0; j--) {
            coefficients[i] -= matrix[i][j] * coefficients[j];
        }
        coefficients[i] /= matrix[i][i];
    }

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    FILE *fp;
    char line[100];
    int n, k, x, y, base;
    Root roots[100];
    int num_roots = 0;

    // Read input from JSON file
    fp = fopen("test_case.json", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    while (fgets(line, 100, fp) != NULL) {
        if (strstr(line, "\"keys\":") != NULL) {
            sscanf(line, "{\"keys\": {\"n\": %d, \"k\": %d},", &n, &k);
        } else if (strstr(line, "\"base\":") != NULL) {
            sscanf(line, "\" %d\": {\"base\": %d, \"value\": \"%s\"},", &x, &base, roots[num_roots].value);
            roots[num_roots].base = base;
            num_roots++;
        }
    }
    fclose(fp);

    // Decode y values
    int *x_values = (int *)malloc(num_roots * sizeof(int));
    int *y_values = (int *)malloc(num_roots * sizeof(int));
    for (int i = 0; i < num_roots; i++) {
        x_values[i] = i + 1;
        y_values[i] = decode_y_value(roots[i].value, roots[i].base);
    }

    // Calculate secret using Lagrange interpolation
    double secret_lagrange = lagrange_interpolation(num_roots, x_values, y_values, 0);

    // Calculate secret using matrix method
    double coefficients[num_roots];
    matrix_method(num_roots, x_values, y_values, coefficients);
    double secret_matrix = coefficients[0];

    // Print results
    printf("Secret (Lagrange): %.0f\n", secret_lagrange);
    printf("Secret (Matrix): %.0f\n", secret_matrix);

    // Identify imposter points (optional)
    // ...

    free(x_values);
    free(y_values);
    return 0;
}