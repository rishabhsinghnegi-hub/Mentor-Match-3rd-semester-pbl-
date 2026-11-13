#include "utils.h"
#include "core.h"
#include "student.h"
#include "staff.h"
#include "matching.h"

int main() {
    system("");
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    if (!ifstream("students.csv")) {
        showError("students.csv was not found. Put it in the same folder and run again.");
        return 1;
    }

    loadTopics();
    loadStudents();
    loadTeachers();
    loadAdmins();

    if (allTopics.empty()) {
        showError("No topics found. Put topics.csv (with at least one topic) in the same folder.");
        return 1;
    }

    while (true) {
        clearScreen();
        printBanner();

        menuItem("1", "Login as Student");
        menuItem("2", "Login as Teacher");
        menuItem("3", "Login as Administrator");
        menuItem("4", "Student Registration");
        menuItem("5", "Teacher Registration");
        menuItem("0", "Exit");

        int c = getNumber("Enter your choice: ", 0, 5);

        if (c == 0) {
            cout << "\n " << BOLD << CYAN
                 << "Thank you for using MENTOR MATCH. Goodbye!"
                 << RESET << "\n\n";
            return 0;
        }

        if (c == 1)
            login(students, "STUDENT LOGIN", "Student ID: ", true, "students.csv");
        else if (c == 2)
            login(teachers, "TEACHER LOGIN", "Teacher ID: ", true, "teachers.csv");
        else if (c == 3)
            login(admins, "ADMINISTRATOR LOGIN", "Admin ID: ", false, "admins.csv");
        else if (c == 4)
            registerStudent();
        else if (c == 5)
            registerTeacher();
    }
}
