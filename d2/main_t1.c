#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 40
#define MAX_NUMBERS 20

int is_safe(int numbers[], int count)
{
    int increasing = 1;
    int decreasing = 1;
    int valid_distance = 1;

    /* check if monotonic */
    for (int i = 0; i < count - 1; i++)
    {
        if (numbers[i] < numbers[i + 1])
        {
            increasing = 0;
        } 
        else if (numbers[i] > numbers[i + 1])
        {
            decreasing = 0;
        }

        /* distance between two numbers */
        int diff = abs(numbers[i] - numbers[i + 1]);

        /* check if the difference is between 1 and 3 */
        if (diff < 1 || diff > 3)
        {
            valid_distance = 0;
        }
    }
    if (valid_distance && (increasing || decreasing))
    {
        return 1; // safe
    }

    return 0;
}

int main()
{
    FILE *fp;

    fp = fopen("input.txt", "r");

    if (fp == NULL)
    {
        perror("file open error");
        return 1;
    }

    /* The levels are either all increasing or all decreasing. */
    /* Any two adjacent levels differ by at least one and at most three. */
    /* Tolerate single bad level */
    char line[MAX_LINE_LENGTH];
    int numbers[MAX_NUMBERS];
    int safe_functions = 0;

    while (fgets(line, sizeof(line), fp))
    {
        char *token = strtok(line, " ");
        char *endptr;
        int count = 0;

        while (token != NULL)
        {
            numbers[count] = strtol(token, &endptr, 10);
            count++;
            token = strtok(NULL, " \n");
        }

        if (is_safe(numbers, count))
        {
            safe_functions++;
        }
    }

    printf("Safe functions: %d\n", safe_functions);
}
