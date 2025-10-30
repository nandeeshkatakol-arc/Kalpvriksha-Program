#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"

struct User
{
    char name[40];
    int id;
    int age;
};

enum CrudOperation
{
    ADD_USER = 1,
    DISPLAY_USERS,
    UPDATE_USER,
    DELETE_USER,
    EXIT_PROGRAM
};

FILE *OpenFile(const char *mode);
void CloseFile(FILE *fp);
void addUserData();
void ReadUserData();
void UpdateUserData();
void DeleteUserData();
void displayUserData();

FILE *OpenFile(const char *mode)
{
    FILE *fp = fopen(FILE_NAME, mode);
    if (fp == NULL)
    {
        printf("Error: Could not open file %s\n", FILE_NAME);
    }
    return fp;
}

void CloseFile(FILE *fp)
{
    if (fp != NULL)
    {
        fclose(fp);
    }
}

void addUserData()
{
    FILE *fp = OpenFile("a+");
    struct User user;

    if (fp != NULL)
    {
        printf("Enter user's details\n");
        printf("1.ID : ");
        scanf("%d", &user.id);
        printf("2.Name : ");
        scanf("%s", user.name);
        printf("3.Age : ");
        scanf("%d", &user.age);

        fprintf(fp, "%d %s %d\n", user.id, user.name, user.age);
        printf("User added successfully!\n");

        CloseFile(fp);
    }
    else
    {
        printf("Error: Could not open file for adding user.\n");
    }

    return;
}

void ReadUserData()
{
    FILE *fp = OpenFile("r");
    struct User user;

    if (fp == NULL)
    {
        printf("User info doesn't exist\n");
    }
    else
    {
        while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) != EOF)
        {
            printf("ID: %d, Name: %s, Age: %d\n", user.id, user.name, user.age);
        }
    }

    CloseFile(fp);
    return;
}

void UpdateUserData()
{
    FILE *fp = OpenFile("r");
    FILE *temp = NULL;
    struct User user;
    int id = 0, flag = 0;

    if (fp == NULL)
    {
        printf("User info doesn't exist\n");
    }
    else
    {
        printf("Enter user ID to updateUserData: ");
        scanf("%d", &id);
        temp = fopen("temp.txt", "w");

        while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) != EOF)
        {
            if (user.id == id)
            {
                flag = 1;
                printf("Enter new details\n");
                printf("2.Name : ");
                scanf("%s", user.name);
                printf("3.Age : ");
                scanf("%d", &user.age);
            }
            fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
        }

        CloseFile(fp);
        CloseFile(temp);
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);

        if (flag == 1)
        {
            printf("User info updateUserDatad successfully!\n");
        }
        else
        {
            printf("User with ID %d not found!\n", id);
        }
    }

    return;
}

void DeleteUserData()
{
    FILE *fp = OpenFile("r");
    FILE *temp = NULL;
    struct User user;
    int id = 0, flag = 0;

    if (fp == NULL)
    {
        printf("User info doesn't exist\n");
    }
    else
    {
        temp = fopen("temp.txt", "w");
        printf("Enter user id to deleteUserData: ");
        scanf("%d", &id);

        while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) != EOF)
        {
            if (user.id == id)
            {
                flag = 1;
                continue;
            }
            fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
        }

        CloseFile(fp);
        CloseFile(temp);
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);

        if (flag == 1)
        {
            printf("User info deleteUserDatad successfully!\n");
        }
        else
        {
            printf("User not found!\n");
        }
    }

    return;
}

void displayUserData()
{
    printf("menu:\n");
    printf("1. addUserData\n");
    printf("2. ReadUserData\n");
    printf("3. UpdateUserData\n");
    printf("4. DeleteUserData\n");
    printf("5. Exit\n");
}

int main()
{
    int choice = 0, start = 1;
    enum CrudOperation operation;

    while (start)
    {
        displayUserData();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        operation = (enum CrudOperation)choice;

        switch (operation)
        {
            case ADD_USER:
            {
                addUserData();
                break;
            }
            case DISPLAY_USERS:
            {
                ReadUserData();
                break;
            }
            case UPDATE_USER:
            {
                UpdateUserData();
                break;
            }
            case DELETE_USER:
            {
                DeleteUserData();
                break;
            }
            case EXIT_PROGRAM:
            {
                start = 0;
                break;
            }
            default:
            {
                printf("Invalid choice!\n");
                break;
            }
        }
    }

    return 0;
}
