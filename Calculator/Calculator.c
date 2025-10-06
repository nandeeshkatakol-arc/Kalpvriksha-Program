#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

char *skipSpaces(char *ptr)
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

int operation(int operand1, int operand2, char operator, int *errorFlag) {
    int result = 0; // single return variable

    switch (operator) {
        case '+':
            result = operand1 + operand2;
            break;
        case '-':
            result = operand1 - operand2;
            break;
        case '*':
            result = operand1 * operand2;
            break;
        case '/':
            if (operand2 == 0) {
                *errorFlag = 2; // Division by zero
                result = 0;
            } else {
                result = operand1 / operand2;
            }
            break;
        default:
            *errorFlag = 1; // Invalid operator
            result = 0;
    }

    return result;
}

int evaluateExpression(char *expression, int *errorFlag)
{
    int numberStack[100], numberTop = -1;
    char operatorStack[100], operatorTop = -1;
    char *currentPtr = skipSpaces(expression);
    int finalResult = 0;

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
                    finalResult = 0;
                    goto END;
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
            finalResult = 0;
            goto END;
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
            finalResult = 0;
            goto END;
        }
    }

    finalResult = numberStack[numberTop];
END:
    return finalResult;
}

int main() {
    char expression[100];
    printf("Enter the expression: ");
    fgets(expression, sizeof(expression), stdin);

    int errorFlag = 0;
    int result = evaluateExpression(expression, &errorFlag);

    if (errorFlag == 1)
    {
        printf("Error: Invalid Expression\n");
    }   
    else if (errorFlag == 2)
    {
        printf("Error: Division by Zero\n");
    }
    else
    {
        printf("Result: %d\n", result);
    }

    return 0;
}
