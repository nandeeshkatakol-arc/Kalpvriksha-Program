#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100
#define SUBJECTS 3
#define MAX_MARK 100
#define MIN_MARK 0

typedef enum
{
    A,
    B,
    C,
    D,
    F
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
    int subjectIndex;
    for (subjectIndex = 0; subjectIndex < SUBJECTS; subjectIndex++)
    {
        total = total + marks[subjectIndex];
    }
    return total;
}

float calculateAverage(const float total)
{
    float average = total / SUBJECTS;
    return average;
}

Grade assignGrade(const float average)
{
    Grade resultGrade = F;
    if (average >= 85)
    {
        resultGrade = A;
    }
    else if (average >= 70)
    {
        resultGrade = B;
    }
    else if (average >= 50)
    {
        resultGrade = C;
    }
    else if (average >= 35)
    {
        resultGrade = D;
    }
    else
    {
        resultGrade = F;
    }
    return resultGrade;
}

int isValidMark(const float mark)
{
    int valid = 0;
    if (mark >= MIN_MARK && mark <= MAX_MARK)
    {
        valid = 1;
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

void readAllStudents(Student students[], const int totalStudents)
{
    int studentIndex;
    for (studentIndex = 0; studentIndex < totalStudents; studentIndex++)
    {
        readSingleStudent(&students[studentIndex]);
    }
}

void processStudents(Student students[], const int totalStudents)
{
    int studentIndex;
    for (studentIndex = 0; studentIndex < totalStudents; studentIndex++)
    {
        int validMarks = 1;
        int subjectIndex;
        for (subjectIndex = 0; subjectIndex < SUBJECTS; subjectIndex++)
        {
            if (!isValidMark(students[studentIndex].marks[subjectIndex]))
            {
                validMarks = 0;
                break;
            }
        }

        if (!validMarks)
        {
            printf("Invalid marks for Roll %d. Skipping.\n", students[studentIndex].rollNumber);
            continue;
        }

        students[studentIndex].totalMarks = calculateTotal(students[studentIndex].marks);
        students[studentIndex].averageMarks = calculateAverage(students[studentIndex].totalMarks);
        students[studentIndex].grade = assignGrade(students[studentIndex].averageMarks);
    }
}

void printPerformanceStars(const Grade grade)
{
    int starsCount = 0;
    if (grade == A)
    {
        starsCount = 5;
    }
    else if (grade == B)
    {
        starsCount = 4;
    }
    else if (grade == C)
    {
        starsCount = 3;
    }
    else if (grade == D)
    {
        starsCount = 2;
    }

    if (starsCount > 0)
    {
        int iterator;
        for (iterator = 0; iterator < starsCount; iterator++)
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
}

void displayAllStudents(const Student students[], const int totalStudents)
{
    int studentIndex;
    for (studentIndex = 0; studentIndex < totalStudents; studentIndex++)
    {
        if (students[studentIndex].grade == F)
        {
            showStudent(&students[studentIndex]);
            continue;
        }

        showStudent(&students[studentIndex]);
        printf("Performance: ");
        printPerformanceStars(students[studentIndex].grade);
        printf("\n");
    }
}

int main(void)
{
    int totalStudents;
    int exitStatus = 0;
    scanf("%d", &totalStudents);

    if (totalStudents <= 0 || totalStudents > MAX_STUDENTS)
    {
        exitStatus = 1;
    }

    Student students[MAX_STUDENTS];

    readAllStudents(students, totalStudents);
    processStudents(students, totalStudents);
    displayAllStudents(students, totalStudents);

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbersRecursive(students, 0, totalStudents);
    printf("\n");

    return exitStatus;
}
