# Assignment 1: Stack-based Calculator

**Author:** Nandeesh MK

---

## Problem Statement
Design a console-based calculator program in C that accepts mathematical expressions as input in the form of strings.  
The program should evaluate the expression and return the result. Supported operations include addition (+), subtraction (-), multiplication (*), and division (/).  
The program should handle integer operations and output the result as an integer, even if the result of division has a remainder.

---

## Requirements
- Input is a mathematical expression in the form of a string.  
- The input string contains integers and operators (+, -, *, /) only.  
- Whitespace between numbers and operators should be ignored.  
- Output is a single integer or an error message.  
- If division by zero occurs, display:  Error: Division by zero.
- If the input expression is invalid, display:  
- The program should follow the order of operations (DMAS): multiplication and division are performed before addition and subtraction.  

---

## Input Format
A single line containing the mathematical expression as a string.  

Example:  
3 + 5 * 2

---

## Output Format
A single integer or an error message depending on the evaluation result.  

Example Output:  
13
