# Mentor Match

A console based C++ application for student and teacher registration, topic preferences, mentor recommendations, and administrator directories. It is a project based learning example for data structures, file handling, and object oriented programming.

## Project layout

```text
mentormatch/
├── database/                 # CSV records read and updated by the application
│   ├── admins.csv
│   ├── appointments.csv      # Teacher meeting bookings
│   ├── projects.csv          # Team and project lifecycle records
│   ├── ratings.csv           # Student project reviews
│   ├── students.csv
│   ├── teachers.csv
│   └── topics.csv
├── docs/
│   ├── function-and-oop-guide.txt
│   └── project-flow.txt
├── include/                  # Declarations and implementation shared by the program
│   ├── core.h
│   ├── matching.h
│   ├── project.h             # Project lifecycle, meeting, and review records
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
- Teacher selection after recommendations and shared project/team records
- Six weekday meetings per project, with progress follow-ups and a six-booking per teacher/day limit
- Project completion tracking and one 1–5 star review per team member per project
- Average teacher ratings shown beside recommendations; ratings do not affect matching scores
- Administrator views for student/teacher profiles, topics, and system counts
- CSV loading, registration append, and updates to selected topic columns

Students enter today's date during login. After choosing a suggested teacher, a team leader can record registered student IDs and names, course, semester, project, and guidance reason. The system schedules six weekday meetings over the two-month project window so the teacher can check progress; each teacher can have up to six bookings per date. There is no project submission to the teacher. Teachers see the shared project/team data in their dashboard. Listed teammates can mark a project complete, and each can submit one star rating when the project is complete or its two-month window ends. Reviews affect displayed teacher averages only; topic match scores remain unchanged.

Project, appointment, and review data use separate CSV tables with stable IDs and foreign-key-like IDs (`teacher_id`, `student_id`, `project_id`) to keep future API/backend integration straightforward. CSV is still a learning/demo persistence layer, not a concurrent booking database; production use should replace it with transactional storage and account security.

Demo teachers T017–T019 use the password `demo123` and have expertise that matches the existing topic list. Sample records give T001 six bookings on 2026-11-16 and T003 six bookings on 2026-11-17; other teachers have open dates. Completed demo projects provide sample ratings for recommendation displays.

## Learning guides

- [Function, data structure, and OOP guide](docs/function-and-oop-guide.txt) describes the current functions and concepts by file.
- [Project flow and architecture](docs/project-flow.txt) follows each menu option and gives an example of mentor scoring.
- [Project lifecycle and rating data](docs/project-flow.txt) documents team setup, appointments, review eligibility, and CSV schemas.
