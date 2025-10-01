# Assignment 2: CRUD Operations in File using C

## Author
**Nandeesh MK**


## 📋 Problem Statement
Create a **C program** to manage user records stored in a file (`users.txt`).  
The program must perform **CRUD operations**:

| Operation | Description |
|-----------|-------------|
| **Create** | Add a new user to the file |
| **Read**   | Display all users |
| **Update** | Modify user details based on ID |
| **Delete** | Remove a user by ID or Name |

Each user has the following fields:

| Field | Type |
|-------|------|
| ID    | Integer (unique) |
| Name  | String |
| Age   | Integer |


## ✅ Requirements

- Store data in **text files** (`users.txt`).
- Implement separate functions for each CRUD operation:
  - `Create()` – Add new users
  - `Read()` – Display all users
  - `Update()` – Modify user details by ID
  - `Delete()` – Remove users by ID or Name
- Handle invalid inputs gracefully.
- Use `struct` to maintain user data.

## 🖥 Input & Output

**Menu Example:**

1.Create

2.Read

3.Update

4.Delete

5.Exit
Enter your choice:

**Create Example:**

Enter user's details

ID: 1

Name: Nandeesh

Age: 21
User created successfully!


**Read Example:**
1 Nandeesh 21
2 Nikhita 21
3 Sanjana 20
4 Lalit 24


**Update Example:**

Enter user ID to update: 1
Enter new Name: Nandeesh mk
Enter new Age: 22
User info updated successfully!

**Delete Example:**

Delete by:

1.ID
2.Name

Name
Enter your choice: 1
Enter user ID to delete: 1
User info deleted successfully!


## 📁 File Structure

Assignment-2/
│
├── CRUD.c # C program implementing CRUD
├── users.txt # File storing user records
└── README.md # This documentation


## ⚡ How to Run

1. Open terminal/command prompt.
2. Navigate to the folder containing `CRUD.c`.
3. Compile the program:

```bash
gcc CRUD.c -o CRUD.exe