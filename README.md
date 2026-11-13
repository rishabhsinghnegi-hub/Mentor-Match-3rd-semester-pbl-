# Mentor Match

A console based C++ application for student and teacher registration, topic preferences, mentor recommendations, and administrator directories. It is a project based learning example for data structures, file handling, and object oriented programming.

## Project layout

```text
mentormatch/
├── database/                 # CSV records read and updated by the application
│   ├── admins.csv
│   ├── ratings.csv           # Reserved; rating functionality is not implemented yet
│   ├── students.csv
│   ├── teachers.csv
│   └── topics.csv
├── docs/
│   ├── function-and-oop-guide.txt
│   └── project-flow.txt
├── include/                  # Declarations and implementation shared by the program
│   ├── core.h
│   ├── matching.h
│   ├── staff.h
│   ├── student.h
│   └── utils.h
├── src/
│   └── main.cpp
└── README.md
```

Run the executable from the repository root. The program uses paths such as `database/students.csv` relative to the current working directory.

## Build and run

Requires a C++11-compatible compiler. From the repository root:

```bash
g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude src/main.cpp -o mentormatch
./mentormatch
```

On Windows with MinGW:

```powershell
g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude src/main.cpp -o mentormatch.exe
.\mentormatch.exe
```

The CSV files include sample records and are the application's local storage. Keep a backup before editing records manually. Passwords are stored as plain text, so use only sample credentials and do not use this design for real accounts.

## Current capabilities

- Student and teacher registration and login
- Student topic selection and preference updates
- Teacher expertise selection and updates
- Mentor ranking from shared topics, with the top three results shown
- Administrator views for student/teacher profiles, topics, and system counts
- CSV loading, registration append, and updates to selected topic columns

`ratings.csv` is currently empty and is not read or written by the program. There is no rating feature in the current code.

## Learning guides

- [Function, data structure, and OOP guide](docs/function-and-oop-guide.txt) describes the current functions and concepts by file.
- [Project flow and architecture](docs/project-flow.txt) follows each menu option and gives an example of mentor scoring.
