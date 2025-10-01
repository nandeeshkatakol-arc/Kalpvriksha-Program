#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define FILEname "users.txt"

void Create();
void Read();
void Update();
void Delete();

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
        printf("\n1. Create\n2. Read\n3. Update\n4. Delete\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice)
        {
            case 1:
                Create();
                break;
            case 2:
                Read();
                break;
            case 3:
                Update();
                break;
            case 4:
                Delete();
                break;
            case 5:
                exit(0);
            default:
                printf("***Invalid choice!***\n");
        }
    }
    return 0;
}

void Create()
{
    FILE *fp =fopen(FILEname, "a+");

    printf("Enter user's details\n");

    struct User user;
    printf("1.ID : ");
    scanf("%d", &user.id);
    printf("2.Name : ");
    scanf("%s", user.name);
    printf("3.Age : "); 
    scanf("%d", &user.age);
    fprintf(fp, "%d %s %d\n", user.id, user.name, user.age);
    printf("User created successfully!\n");
    fclose(fp);

}

void Read()
{
    FILE *fp = fopen(FILEname, "r");
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

void Update()
{
    FILE *fp = fopen(FILEname, "r");
    if(!fp)
    {
        printf("User info doesn't exists\n");
        return;
    }

    int id,flag=0;
    printf("Enter user ID to update: ");
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
    remove(FILEname);
    rename("temp.txt",FILEname);

    if(flag==1)
    {
        printf("User info updated successfully!\n");

    }
    else
    {
        printf("User with ID %d not found!\n", id);
    }

}

void Delete()
{
    FILE *fp=fopen(FILEname,"r");
    if(!fp)
    {
        printf("User info doesn't exists\n");
        return;
    }
    int choice, id ,flag=0;
    printf("delete by\n1.by ID\n2.by name\nEnter your choice: ");
    scanf("%d",&choice);
    FILE *temp=fopen("temp.txt","w");
    struct User user;
    switch(choice)
    {
        case 1:
            printf("Enter user id to delete: ");
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
            break;
        case 2:
            printf("Enter the name to delete: ");
            char name[40];
            scanf("%s",name);
            while(fscanf(fp,"%d %s %d",&user.id,user.name,&user.age)!=EOF)
            {
                if(strcmp(user.name,name)==0)
                {
                    flag=1;
                    continue;
                }
                fprintf(temp,"%d %s %d\n",user.id,user.name,user.age);
            }
            break;
        default:
            printf("Invalid choice!\n");
            fclose(fp);
            fclose(temp);
            remove("temp.txt"); 
            return;
    }

    fclose (fp);
    fclose(temp);
    remove(FILEname);
    rename("temp.txt",FILEname);

    if(flag)
    printf("User info deleted successfully!\n");
    else
    printf("User not found!\n");
}