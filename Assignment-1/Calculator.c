#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
int evaluate(char *expression, int *error);
int operation(int a,int b,char op,int *error);
char *skip_spaces(char *p);
int precedence(char op);
int main()
{
    char expression[100];
    int valid = 1;
    printf("Enter the expression:");
    fgets(expression,sizeof(expression),stdin);
    
    //to check whether valid expression or not
    for(int i=0; expression[i]!='\0';i++)
    {
        char c=expression[i];
        if(!(isdigit(c) || c=='+'|| c=='-'|| c=='*' || c=='/' || isspace(c)))
        {
            valid = 0;
            break;

        }
    }

    if(!valid)
    {
        printf("Invalid expression\n");
        return 0;
    }


    int error=0;
    int result=evaluate(expression,&error);
    if(error==1)
    {
        printf("Error: Invalid Expression\n");

    }

    else if(error==2)
    {
        printf("Error: Division by Zero\n");
    }

    else{
        printf("Result: %d\n",result);
    }

    return 0;
}

int evaluate(char *expression, int *error)
{
    int numstack[100],numTop=-1;
    char opstack[100],opTop=-1;
    char *p=expression;
    p=skip_spaces(p);

    while(*p)
    {
        if(isdigit(*p)) //if digit
        {
            int num=strtol(p,&p,10);
            numstack[++numTop]=num;
        }

        else if(*p=='+'|| *p=='-'|| *p=='*' || *p=='/')  //if operator
        {
            while(opTop>=0 && precedence(opstack[opTop])>=precedence(*p))
            {
                int b=numstack[numTop--];
                int a=numstack[numTop--];
                char op=opstack[opTop--];
                numstack[++numTop]=operation(a,b,op,error);
                if(*error) 
                return 0;
            }
             opstack[++opTop] = *p;  // 
             p++;
        }
        else if(*p==' ' || *p=='\n')  //if newline or spaces
        {
            p++;
            
        }

        else{           //if invalid character  
            *error=1;
            return 0;
        }



    }

    while(opTop>=0)
    {
        int b=numstack[numTop--];
        int a=numstack[numTop--];
        char op=opstack[opTop--];
        numstack[++numTop]=operation(a,b,op,error);
        if(*error) 
        return 0;
    
    }
   return numstack[numTop];


}

char *skip_spaces(char *p)
{
    while(*p==' ')
    p++;
    return p;
}

int precedence(char op)
{
    if(op=='+' || op=='-')
    return 1;
    
    if(op=='*' || op== '/')
    return 2;

    return 0;
}

int operation(int a,int b,char op,int *error)
{
    switch(op)
    {
        case '+':return a+b;
        case '-': return a-b;
        case '*':return a*b;
        case '/':
            if(b==0)
            {
                *error=2;
                return 0;
            }
            return a/b;

    }
    *error=1;
    return 0;
}