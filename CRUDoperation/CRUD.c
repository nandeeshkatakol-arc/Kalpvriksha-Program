#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define FILE_NAME "users.txt"

void addUserData();
void readUserData();
void updateUserData();
void deleteUserData();

struct User
{
    char name[40];
    int id;
    int age;
};


int main()
{
    int choice;
    
    while(1)
    {
        printf("\n1. addUserData\n2. readUserData\n3. updateUserData\n4. deleteUserData\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice)
        {
            case 1:
                addUserData();
                break;
            case 2:
                readUserData();
                break;
            case 3:
                updateUserData();
                break;
            case 4:
                deleteUserData();
                break;
            case 5:
                exit(0);
            default:
                printf("***Invalid choice!***\n");
        }
    }
    return 0;
}

void addUserData()
{
    FILE *fp =fopen(FILE_NAME, "a+");

    printf("Enter user's details\n");

    struct User user;
    printf("1.ID : ");
    scanf("%d", &user.id);
    printf("2.Name : ");
    scanf("%s", user.name);
    printf("3.Age : "); 
    scanf("%d", &user.age);
    fprintf(fp, "%d %s %d\n", user.id, user.name, user.age);
    printf("User addUserDatad successfully!\n");
    fclose(fp);

}

void readUserData()
{
    FILE *fp = fopen(FILE_NAME, "r");
    struct User user;
    if(!fp)
    {
        printf("User info doesn't exists\n");   
        return;
    }

    
    while(fscanf(fp,"%d %s %d", &user.id, user.name, &user.age) != EOF)
    {
        printf("ID: %d, Name: %s, Age: %d\n", user.id, user.name, user.age);
    }

    fclose(fp);

}

void updateUserData()
{
    FILE *fp = fopen(FILE_NAME, "r");
    if(!fp)
    {
        printf("User info doesn't exists\n");
        return;
    }

    int id,flag=0;
    printf("Enter user ID to updateUserData: ");
    scanf("%d", &id);

    FILE *temp=fopen("temp.txt","w");
    struct User user;
    while(fscanf(fp,"%d %s %d", &user.id,user.name,&user.age)!=EOF)
    {
        if(user.id==id)
        {
            flag=1;

            printf("Enter new details\n"); 
            printf("2.Name : ");
            scanf("%s", user.name);
            printf("3.Age : "); 
            scanf("%d", &user.age);
        }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt",FILE_NAME);

    if(flag==1)
    {
        printf("User info updateUserDatad successfully!\n");

    }
    else
    {
        printf("User with ID %d not found!\n", id);
    }

}

void deleteUserData()
{
    FILE *fp=fopen(FILE_NAME,"r");
    if(!fp)
    {
        printf("User info doesn't exists\n");
        return;
    }
    int choice, id ,flag=0; 
    FILE *temp=fopen("temp.txt","w");
    struct User user;
    printf("Enter user id to deleteUserData: ");
    scanf("%d",&id);
    while(fscanf(fp,"%d %s %d",&user.id,user.name,&user.age)!=EOF)
    {
      if(user.id==id)
        {
            flag=1;
            continue;
        }
        fprintf(temp,"%d %s %d\n",user.id,user.name,user.age);
    }
    fclose (fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt",FILE_NAME);

    if(flag)
    printf("User info deleteUserDatad successfully!\n");
    else
    printf("User not found!\n");
}