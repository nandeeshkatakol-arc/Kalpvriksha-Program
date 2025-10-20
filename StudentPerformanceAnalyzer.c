 #include <stdio.h>

 #define MAX_STUDENTS 100
 #define SUBJECTS 3

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
    char name[50];
    float marks[SUBJECTS];
    float total;
    float average;
    Grade grade;
} Student;

float calculateTotal ( const float marks[] )
{
    float total = 0.0;
    for (int subjectIndex = 0; subjectIndex < SUBJECTS; subjectIndex++)
    {
        total += marks[subjectIndex];
    }
    return total;
}

float calculateAverage ( const float total )
{
    return total / SUBJECTS;
}

Grade assignGrade ( const float average )
{
    Grade resultGrade = F;

    if ( average >= 85 )
    {
        resultGrade = A;
    }
    else if ( average >= 70 )
    {
        resultGrade = B;
    }
    else if ( average >= 55 )
    {
        resultGrade = C;
    }
    else if ( average >= 40 )
    {
        resultGrade = D;
    }

    return resultGrade;
}

void inputStudentData ( Student *student )
{
    printf ("Enter Roll Number: ");
    scanf ("%d", &student->rollNumber);

    printf ("Enter Name: " );
    scanf ("%s", student->name );

    printf ("Enter Marks for %d Subjects: ", SUBJECTS);
    for ( int subjectIndex = 0; subjectIndex < SUBJECTS; subjectIndex++)
    {
        scanf ("%f", &student->marks[subjectIndex]);
    }

    student->total = calculateTotal ( student->marks );
    student->average = calculateAverage ( student->total );
    student->grade = assignGrade ( student->average );
}

void readAllStudents ( Student students[], int totalStudents )
{
    for (int studentIndex = 0; studentIndex < totalStudents; studentIndex++)
    {
        printf ( "\nEnter details for Student %d:\n", studentIndex + 1 );
        inputStudentData ( &students[studentIndex] );
    }
}

void displayStudentData (const Student *student)
{
    printf ("\nRoll Number: %d", student->rollNumber);
    printf ("\nName: %s", student->name);
    printf ("\nMarks: ");

    for (int subjectIndex = 0; subjectIndex < SUBJECTS; subjectIndex++)
    {
        printf ("%.2f ", student->marks[subjectIndex] );
    }

    printf ("\nTotal: %.2f", student->total);
    printf ("\nAverage: %.2f", student->average);
    printf ("\nGrade: ");

    switch ( student->grade )
    {
        case A:
            printf ( "A" );
            break;
        case B:
            printf ( "B" );
            break;
        case C:
            printf ( "C" );
            break;
        case D:
            printf ( "D" );
            break;
        case F:
            printf ( "F" );
            break;
        default:
            printf ( "Invalid" );
            break;
    }

    printf ("\n");
}

void displayAllStudents (const Student students[], int totalStudents)
{
    printf ( " STUDENT DETAILS " );
    for ( int studentIndex = 0; studentIndex < totalStudents; studentIndex++ )
    {
        displayStudentData ( &students[studentIndex] );
    }
}

void printRollNumbersRecursive (const Student students[], const int index, const int totalStudents)
{
    if (index >= totalStudents)
    {
        return;
    }

    printf ("%d ", students[index].rollNumber);
    printRollNumbersRecursive (students, index + 1, totalStudents);
}

int main ( void )
{
    int totalStudents;
    
    int exitStatus = 0;

    printf ("Enter number of students (max %d): ", MAX_STUDENTS );
    scanf ("%d", &totalStudents );

    if ( totalStudents <= 0 || totalStudents > MAX_STUDENTS )
    {
        printf ( "Invalid number of students.\n" );
        exitStatus = 1;
    }
    else
    {
        Student students[MAX_STUDENTS];

        readAllStudents ( students, totalStudents );
        displayAllStudents ( students, totalStudents );

        printf ( "\nRoll Numbers (Recursive Print): " );
        printRollNumbersRecursive ( students, 0, totalStudents );
        printf ( "\n" );
    }

    return exitStatus;
}
