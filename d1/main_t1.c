#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int comp(const void* a, const void* b) {
        // If a is smaller, positive value will be returned
        return (*(int*)a - *(int*)b);
}

int count_lines(FILE* fp)
{
        int count = 0;
        char c;
        for (c = getc(fp); c != EOF; c = getc(fp))
        {
                if (c == '\n')
                {
                        count++;
                }
        }
        rewind(fp);
        return count;
}


int main()
{
        char *filename = "input.txt";
        FILE *fp;

        fp = fopen(filename, "r");
        if (fp == NULL)
        {
                perror("error opening file");
                return 1;
        }

        int n = count_lines(fp);

        int *left_arr = (int*)(malloc(n * sizeof(int)));
        int *right_arr = (int*)(malloc(n * sizeof(int)));

        if (left_arr == NULL || right_arr == NULL)
        {
                perror("malloc failed");
                return 1;
        }

        for (int i = 0; i < n; i++)
        {
                fscanf(fp, "%d %d", &left_arr[i], &right_arr[i]);
        }

        qsort(left_arr, n, sizeof(int), comp);
        qsort(right_arr, n, sizeof(int), comp);

        int total = 0;

        for (int i = 0; i < n; i++)
        {
                total += abs(left_arr[i] - right_arr[i]);
        }

        free(left_arr);
        free(right_arr);
        fclose(fp);

        printf("total: %d", total);
        return 0;
}
