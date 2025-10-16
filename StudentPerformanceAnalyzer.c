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
    int rollNo;
    char name[50];
    float marks[SUBJECTS];
    float total;
    float average;
    Grade grade;
} Student;

void printRollRecursive(const Student students[], int index, int count)
{
    if (index >= count)
    {
        return;
    }

    printf("%d ", students[index].rollNo);
    printRollRecursive(students, index + 1, count);
}

float calculateTotal(const float marks[])
{
    float total = 0;

    for (int i = 0; i < SUBJECTS; i++)
    {
        total += marks[i];
    }

    return total;
}

float calculateAverage(float total)
{
    return total / SUBJECTS;
}

Grade calculateGrade(float average)
{
    if (average >= 85)
    {
        return GRADE_A;
    }
    else if (average >= 70)
    {
        return GRADE_B;
    }
    else if (average >= 50)
    {
        return GRADE_C;
    }
    else if (average >= 35)
    {
        return GRADE_D;
    }
    else
    {
        return GRADE_F;
    }
}

int isValidMark(float mark)
{
    if (mark >= MIN_MARK && mark <= MAX_MARK)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void readSingleStudent(Student *s)
{
    scanf("%d %s %f %f %f",
          &s->rollNo,
          s->name,
          &s->marks[0],
          &s->marks[1],
          &s->marks[2]);
}

void readAllStudents(Student students[], int count)
{
    for (int i = 0; i < count; i++)
    {
        readSingleStudent(&students[i]);
    }
}

void processStudents(Student students[], int count)
{
    for (int i = 0; i < count; i++)
    {
        int valid = 1;

        for (int j = 0; j < SUBJECTS; j++)
        {
            if (!isValidMark(students[i].marks[j]))
            {
                valid = 0;
                break;
            }
        }

        if (!valid)
        {
            printf("Invalid marks for Roll %d. Skipping.\n", students[i].rollNo);
            continue;
        }

        students[i].total = calculateTotal(students[i].marks);
        students[i].average = calculateAverage(students[i].total);
        students[i].grade = calculateGrade(students[i].average);
    }
}

void showStudent(const Student *s)
{
    printf("Roll No: %d\n", s->rollNo);
    printf("Name: %s\n", s->name);
    printf("Marks: %.2f %.2f %.2f\n", s->marks[0], s->marks[1], s->marks[2]);
    printf("Total: %.2f\n", s->total);
    printf("Average: %.2f\n", s->average);
    printf("Grade: %c\n\n", 'A' + s->grade);
}

void displayAllStudents(const Student students[], int count)
{
    for (int i = 0; i < count; i++)
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

    printf("List of Roll Numbers: ");
    printRollRecursive(students, 0, totalStudents);
    printf("\n");

    return 0;
}
