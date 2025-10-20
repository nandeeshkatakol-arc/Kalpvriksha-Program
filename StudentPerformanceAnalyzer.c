#include <stdio.h>

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

Grade computeGrade(const float average)
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

void readStudentData(Student students[], const int totalStudents)
{
    int studentIndex;
    printf("Enter details for Student %d (RollNumber Name Marks1 Marks2 Marks3): ", studentIndex + 1);
    for (studentIndex = 0; studentIndex < totalStudents; studentIndex++)
    {
        scanf("%d %s %f %f %f",
              &students[studentIndex].rollNumber,
              students[studentIndex].studentName,
              &students[studentIndex].marks[0],
              &students[studentIndex].marks[1],
              &students[studentIndex].marks[2]);
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
        students[studentIndex].grade = computeGrade(students[studentIndex].averageMarks);
    }
}

void printPerformanceStars(const Grade grade)
{
    int starsCount = 0;

    switch (grade)
    {
        case A:
            starsCount = 5;
            break;
        case B:
            starsCount = 4;
            break;
        case C:
            starsCount = 3;
            break;
        case D:
            starsCount = 2;
            break;
        default:
            starsCount = 0;
            break;
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
        showStudent(&students[studentIndex]);
        
        if (students[studentIndex].grade == F)
        {
            continue;
        }

        printf("Performance: ");
        printPerformanceStars(students[studentIndex].grade);
        printf("\n");
    }
}

int main()
{
    int totalStudents;
    printf("Enter number of students: ");
    scanf("%d", &totalStudents);

    if (totalStudents > 0 && totalStudents <= MAX_STUDENTS)
    {
        Student students[MAX_STUDENTS];

        readStudentData(students, totalStudents);
        processStudents(students, totalStudents);
        displayAllStudents(students, totalStudents);

        printf("List of Roll Numbers : ");
        printRollNumbersRecursive(students, 0, totalStudents);
        printf("\n");
    }
    else
    {
        printf("Invalid number of students.\n");
    }

    return 0;
}
