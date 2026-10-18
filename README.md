
# MentorMatch

A C++-based mentor matching system that manages students, teachers, administrators, and topic preferences using CSV file handling.

## Features

- Student records and profiles
- Teacher profiles and expertise topics
- Administrator records
- Topic selection and priority management
- CSV-based data storage
- Console-based menu interface

## Technologies Used

- C++
- Object-Oriented Programming
- File Handling
- CSV
- Git and GitHub

## Project Structure

- `main.cpp` — Program entry point
- `utils.h` — Utility functions and console formatting
- `core.h` — Core classes and shared functionality
- `student.h` — Student functionality
- `staff.h` — Teacher and administrator functionality
- `students.csv` — Student records
- `teachers.csv` — Teacher records
- `admins.csv` — Administrator records
- `topics.csv` — Available topics

## How to Run

Compile the program using a C++ compiler such as g++:

```bash
g++ main.cpp -o main.exe
```

Run it on Windows:

```powershell
.\main.exe
```

## Note

The CSV files are intended for sample project data. Do not use plain-text passwords or real personal information in a production system.
git status