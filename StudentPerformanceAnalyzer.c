#include <stdio.h>
#include <string.h>

#define SUBJECTS 3
#define MAX_MARK 100
#define MIN_MARK 0

typedef enum
{
    GRADE_A,
    GRADE_B,
    GRADE_C,
    GRADE_D,
    GRADE_F
} Grade;

typedef struct
{
    int rollNumber;
    char studentName[50];
    float marks[SUBJECTS];
    float totalMarks;
    float averageMarks;
    Grade grade;
} Student;

void printRollNumbersRecursive(const Student students[], int index, int totalStudents)
{
    if (index >= totalStudents)
    {
        return;
    }

    printf("%d ", students[index].rollNumber);
    printRollNumbersRecursive(students, index + 1, totalStudents);
}

float calculateTotal(const float marks[])
{
    float total = 0.0;
    int i;
    for (i = 0; i < SUBJECTS; i++)
    {
        total = total + marks[i];
    }
    return total;
}

float calculateAverage(float total)
{
    float average = total / SUBJECTS;
    return average;
}

Grade calculateGrade(float average)
{
    Grade resultGrade = GRADE_F;

    if (average >= 85)
    {
        resultGrade = GRADE_A;
    }
    else if (average >= 70)
    {
        resultGrade = GRADE_B;
    }
    else if (average >= 50)
    {
        resultGrade = GRADE_C;
    }
    else if (average >= 35)
    {
        resultGrade = GRADE_D;
    }
    else
    {
        resultGrade = GRADE_F;
    }

    return resultGrade;
}

int isValidMark(float mark)
{
    int valid = 0;
    if (mark >= MIN_MARK)
    {
        if (mark <= MAX_MARK)
        {
            valid = 1;
        }
    }
    return valid;
}

void readSingleStudent(Student *student)
{
    scanf("%d %s %f %f %f",
          &student->rollNumber,
          student->studentName,
          &student->marks[0],
          &student->marks[1],
          &student->marks[2]);
}

void readAllStudents(Student students[], int totalStudents)
{
    int i;
    for (i = 0; i < totalStudents; i++)
    {
        readSingleStudent(&students[i]);
    }
}

void processStudents(Student students[], int totalStudents)
{
    int i, j;
    for (i = 0; i < totalStudents; i++)
    {
        int validMarks = 1;

        for (j = 0; j < SUBJECTS; j++)
        {
            if (!isValidMark(students[i].marks[j]))
            {
                validMarks = 0;
                break;
            }
        }

        if (!validMarks)
        {
            printf("Invalid marks for Roll %d. Skipping.\n", students[i].rollNumber);
            continue;
        }

        students[i].totalMarks = calculateTotal(students[i].marks);
        students[i].averageMarks = calculateAverage(students[i].totalMarks);
        students[i].grade = calculateGrade(students[i].averageMarks);
    }
}

void printPerformanceStars(Grade grade)
{
    int starsCount = 0;

    if (grade == GRADE_A)
    {
        starsCount = 5;
    }
    else if (grade == GRADE_B)
    {
        starsCount = 4;
    }
    else if (grade == GRADE_C)
    {
        starsCount = 3;
    }
    else if (grade == GRADE_D)
    {
        starsCount = 2;
    }

    if (starsCount > 0)
    {
        int i;
        for (i = 0; i < starsCount; i++)
        {
            printf("*");
        }
        printf("\n");
    }
}

void showStudent(const Student *student)
{
    printf("Roll: %d\n", student->rollNumber);
    printf("Name: %s\n", student->studentName);
    printf("Total: %.2f\n", student->totalMarks);
    printf("Average: %.2f\n", student->averageMarks);
    printf("Grade: %c\n", 'A' + student->grade);

    if (student->grade != GRADE_F)
    {
        printf("Performance: ");
        printPerformanceStars(student->grade);
    }

    printf("\n");
}

void displayAllStudents(const Student students[], int totalStudents)
{
    int i;
    for (i = 0; i < totalStudents; i++)
    {
        showStudent(&students[i]);
    }
}

int main(void)
{
    int totalStudents;
    scanf("%d", &totalStudents);

    if (totalStudents <= 0)
    {
        return 0;
    }

    Student students[totalStudents];

    readAllStudents(students, totalStudents);
    processStudents(students, totalStudents);
    displayAllStudents(students, totalStudents);

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbersRecursive(students, 0, totalStudents);
    printf("\n");

    return 0;
}
