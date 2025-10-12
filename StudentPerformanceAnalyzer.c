#include <stdio.h>
#include <string.h>

#define SUBJECT_COUNT 3
#define MAX_MARK 100
#define MIN_MARK 0

typedef enum
{
    GRADE_A,
    GRADE_B,
    GRADE_C,
    GRADE_D,
    GRADE_E,
    GRADE_F
} Grade;

typedef struct
{
    int rollNumber;
    char name[50];
    float marks[SUBJECT_COUNT];
    float total;
    float average;
    Grade grade;

} Student;

void printRollRecursive(const Student students[], int index, int totalStudents)
{
    if (index >= totalStudents)
    {
        return;
    }
    printf("%d ", students[index].rollNumber);
    printRollRecursive(students, index + 1, totalStudents);
}

float getTotal(const float marks[])
{
    float total = 0;
    for (int i = 0; i < SUBJECT_COUNT; i++)
    {
        total += marks[i];
    }
    return total;
}

float getAverage(float total)
{
    return total / SUBJECT_COUNT;
}

Grade getGrade(float average)
{
    Grade grade;
    if (average >= 90)
    {
        grade = GRADE_A;
    }
    else if (average >= 80)
    {
        grade = GRADE_B;
    }
    else if (average >= 70)
    {
        grade = GRADE_C;
    }
    else if (average >= 60)
    {
        grade = GRADE_D;
    }
    else if (average >= 50)
    {
        grade = GRADE_E;
    }
    else
    {
        grade = GRADE_F;
    }
    return grade;
}

int isValidMark(float mark)
{
    return mark >= MIN_MARK && mark <= MAX_MARK;
}

void inputSingleStudent(Student *s)
{
    scanf("%d %s %f %f %f",
          &s->rollNumber,
          s->name,
          &s->marks[0],
          &s->marks[1],
          &s->marks[2]);
}

void inputAllStudents(Student students[], int totalStudents)
{
    for (int i = 0; i < totalStudents; i++)
    {
        inputSingleStudent(&students[i]);
    }
}

void processStudentResults(Student students[], int totalStudents)
{
    for (int i = 0; i < totalStudents; i++)
    {
        int valid = 1;
        for (int j = 0; j < SUBJECT_COUNT; j++)
            if (!isValidMark(students[i].marks[j]))
                valid = 0;

        if (!valid)
        {
            printf("Invalid marks for Roll %d. Skipping.\n", students[i].rollNumber);
            continue;
        }

        students[i].total = getTotal(students[i].marks);
        students[i].average = getAverage(students[i].total);
        students[i].grade = getGrade(students[i].average);
    }
}

void printPerformance(Grade grade)
{
    switch (grade)
    {
    case GRADE_A:
        printf("Performance: *****\n");
        break;
    case GRADE_B:
        printf("Performance: ****\n");
        break;
    case GRADE_C:
        printf("Performance: ***\n");
        break;
    case GRADE_D:
        printf("Performance: **\n");
        break;
    case GRADE_E:
        printf("Performance: *\n");
        break;
    case GRADE_F:
        printf("Performance: No stars\n");
        break;
    default:
        break;
    }
}

void showStudent(const Student *s)
{
    printf("Roll No: %d\n", s->rollNumber);
    printf("Name: %s\n", s->name);
    printf("Total Marks: %f\n", s->total);
    printf("Average: %f\n", s->average);
    printf("Grade: %c\n", 'A' + s->grade);

    if (s->grade != GRADE_F)
    {
        printPerformance(s->grade);
    }
    printf("\n");
}

void displayAllStudents(const Student students[], int totalStudents)
{
    for (int i = 0; i < totalStudents; i++)
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
    };

    Student students[totalStudents];

    inputAllStudents(students, totalStudents);
    processStudentResults(students, totalStudents);
    displayAllStudents(students, totalStudents);

    printf("List of Roll Numbers: ");
    printRollRecursive(students, 0, totalStudents);
}