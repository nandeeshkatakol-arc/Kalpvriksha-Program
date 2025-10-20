#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateImage(int *image, int sizeOfImage)
{
    int iterator;
    for (iterator = 0; iterator < sizeOfImage * sizeOfImage; iterator++)
    {
        *(image + iterator) = rand() % 256;
    }
}

void printImage(int *image, int sizeOfImage)
{
    int rows;
    int columns;
    for (rows = 0; rows < sizeOfImage; rows++)
    {
        for (columns = 0; columns < sizeOfImage; columns++)
        {
            printf("%3d ", *(image + rows * sizeOfImage + columns));
        }
        printf("\n");
    }
}

void rotateImageClockwise(int *image, int sizeOfImage)
{
    int rows;
    int columns;
    for (rows = 0; rows < sizeOfImage / 2; rows++)
    {
        for (columns = rows; columns < sizeOfImage - rows - 1; columns++)
        {
            int *top = image + rows * sizeOfImage + columns;
            int *right = image + columns * sizeOfImage + (sizeOfImage - 1 - rows);
            int *bottom = image + (sizeOfImage - 1 - rows) * sizeOfImage + (sizeOfImage - 1 - columns);
            int *left = image + (sizeOfImage - 1 - columns) * sizeOfImage + rows;

            int temp = *left;
            *left = *bottom;
            *bottom = *right;
            *right = *top;
            *top = temp;
        }
    }
}

void applySmoothingFilter(int *image, int sizeOfImage)
{
    int tempRow[sizeOfImage];
    int rowIndex;
    int columnIndex;

    for (rowIndex = 0; rowIndex < sizeOfImage; rowIndex++)
    {
        for (columnIndex = 0; columnIndex < sizeOfImage; columnIndex++)
        {
            int sum = 0;
            int count = 0;
            int neighborRow;
            int neighborColumn;

            for (neighborRow = rowIndex - 1; neighborRow <= rowIndex + 1; neighborRow++)
            {
                for (neighborColumn = columnIndex - 1; neighborColumn <= columnIndex + 1; neighborColumn++)
                {
                    if (neighborRow >= 0 && neighborRow < sizeOfImage && neighborColumn >= 0 && neighborColumn < sizeOfImage)
                    {
                        sum += *(image + neighborRow * sizeOfImage + neighborColumn);
                        count++;
                    }
                }
            }
            tempRow[columnIndex] = sum / count;
        }

        for (columnIndex = 0; columnIndex < sizeOfImage; columnIndex++)
        {
            *(image + rowIndex * sizeOfImage + columnIndex) = tempRow[columnIndex];
        }
    }
}

int main()
{
    int sizeOfImage;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &sizeOfImage);
    srand(time(NULL));

    int image[sizeOfImage][sizeOfImage];
    generateImage(&image[0][0], sizeOfImage);

    printf("\nOriginal Randomly Generated Image:\n");
    printImage(&image[0][0], sizeOfImage);

    rotateImageClockwise(&image[0][0], sizeOfImage);
    printf("\nImage after 90 Clockwise Rotation:\n");
    printImage(&image[0][0], sizeOfImage);

    applySmoothingFilter(&image[0][0], sizeOfImage);
    printf("\nImage after Applying 3x3 Smoothing Filter:\n");
    printImage(&image[0][0], sizeOfImage);

    return 0;
}
