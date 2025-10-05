#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int evaluateExpression(char *expression, int *errorFlagFlag);
int operation(int operand1, int operand2, char operator, int *errorFlag);
char *skip_spaces(char *ptr);
int precedence(char operator);
int valiadateExpression(char *expression);

int main()
{
    char expression[100];
    int valid = 1;

    printf("Enter the expression: ");
    fgets(expression, sizeof(expression), stdin);

    
    valid = valiadateExpression(expression);

    if (!valid)
    {
        printf("Invalid expression\n");
        return 0;
    }

    int errorFlag = 0;
    int result = evaluateExpression(expression, &errorFlag);

    if (errorFlag == 1)
    {
        printf("error: Invalid Expression\n");
    }
    else if (errorFlag == 2)
    {
        printf("error: Division by Zero\n");
    }
    else
    {
        printf("Result: %d\n", result);
    }

    return 0;
}

int valiadateExpression(char *expression)
{
    for (int i = 0; expression[i] != '\0'; i++)
    {
        char c = expression[i];
        if (!(isdigit(c) || c == '+' || c == '-' || c == '*' || c == '/' || isspace(c)))
        {
            return 0; 
        }
    }
    return 1; 
}

int evaluateExpression(char *expression, int *errorFlag)
{
    int numberStack[100], numberTop = -1;
    char operatorStack[100], operatorTop = -1;
    char *currentPtr = skip_spaces(expression);

    while (*currentPtr)
    {
        if (isdigit(*currentPtr))
        {
            int num = strtol(currentPtr, &currentPtr, 10);
            numberStack[++numberTop] = num;
        }
        else if (*currentPtr == '+' || *currentPtr == '-' || *currentPtr == '*' || *currentPtr == '/')
        {
            while (operatorTop >= 0 && precedence(operatorStack[operatorTop]) >= precedence(*currentPtr))
            {
                int operand2 = numberStack[numberTop--];
                int operand1 = numberStack[numberTop--];
                char operator = operatorStack[operatorTop--];

                numberStack[++numberTop] = operation(operand1, operand2, operator, errorFlag);
                if (*errorFlag)
                {
                    return 0;
                }
            }
            operatorStack[++operatorTop] = *currentPtr;
            currentPtr++;
        }
        else if (*currentPtr == ' ' || *currentPtr == '\n')
        {
            currentPtr++; 
        }
        else
        {
            *errorFlag = 1;
            return 0;
        } 
    }

    while (operatorTop >= 0)
    {
        int operand2 = numberStack[numberTop--];
        int operand1 = numberStack[numberTop--];
        char operator = operatorStack[operatorTop--];

        numberStack[++numberTop] = operation(operand1, operand2, operator, errorFlag);
        if (*errorFlag)
        {
            return 0;
        }
    }

    return numberStack[numberTop];
}

char *skip_spaces(char *ptr)
{
    while (*ptr == ' ')
    {
        ptr++;
    }
    return ptr;
}

int precedence(char operator)
{
    int value = 0;

    if (operator == '+' || operator == '-')
    {
        value = 1;
    }
    else if (operator == '*' || operator == '/')
    {
        value = 2;
    }

    return value;
}

int operation(int operand1, int operand2, char operator, int *errorFlag)
{
    switch (operator)
    {
        case '+': return operand1 + operand2;
        case '-': return operand1 - operand2;
        case '*': return operand1 * operand2;
        case '/':
            if (operand2 == 0)
            {
                *errorFlag = 2;
                return 0;
            }
            return operand1 / operand2;
        default:
            *errorFlag = 1;
            return 0;
    }
}

