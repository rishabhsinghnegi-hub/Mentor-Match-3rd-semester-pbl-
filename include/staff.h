
#pragma once
#include "core.h"
#include "student.h"

class Teacher : public TopicUser {
public:
    Teacher(const vector<string>& f)
        : TopicUser(
            f[0], f[1], f[5],
            vector<string>(f.begin(), f.begin() + 5),
            "SECONDARY", "PRIMARY", "EXPERTISE"
          ) {}

    void save(int changed) override;
    void showProfile();
    void dashboard() override;
};

void teacherProjectsMenu(Teacher& teacher);

class Admin : public User {
public:
    Admin(string i, string n, string p)
        : User(i, n, p) {}

    void dashboard() override;
};

vector<Teacher> teachers;
vector<Admin> admins;

template <class T>
void viewDirectory(vector<T>& users, string title) {

    while (true) {

        clearScreen();
        printTitle(title);

        
        if (users.empty()) {
            showNote("No records found.");
            pauseScreen();
            return;
        }

        
        for (size_t i = 0; i < users.size(); i++) {

            cout << "\n "
                 << i + 1 << ". "
                 << users[i].getName()
                 << " (" << users[i].getId() << ")";
        }

        cout << "\n\n";

        menuItem("0", "Back");

        int count = static_cast<int>(users.size());

        int choice = getNumber(
            "Enter record number to view profile: ",
            0,
            count
        );

        
        if (choice == 0)
            return;

        
        clearScreen();

        users[choice - 1].showProfile();

        pauseScreen();
    }
}



void viewTopicDirectory() {

    clearScreen();

    printTitle("AVAILABLE TOPICS");

    if (allTopics.empty()) {
        showNote("No topics available.");
    }
    else {
        for (size_t i = 0; i < allTopics.size(); i++) {

            cout << "\n "
                 << i + 1 << ". "
                 << allTopics[i];
        }
    }

    cout << "\n";

    pauseScreen();
}



void showSystemOverview() {

    clearScreen();

    printTitle("SYSTEM OVERVIEW");

    showField("Total Students", to_string(students.size()));

    showField("Total Teachers", to_string(teachers.size()));

    showField("Total Admins", to_string(admins.size()));

    showField("Available Topics", to_string(allTopics.size()));

    cout << "\n";

    pauseScreen();
}

void Admin::dashboard() {

    while (true) {

        clearScreen();

        printTitle("ADMIN DASHBOARD");

        cout << "\n Welcome, "
             << BOLD << YELLOW
             << name
             << RESET << "\n\n";

        menuItem("1", "View Student Directory");

        menuItem("2", "View Teacher Directory");

        menuItem("3", "View Available Topics");

        menuItem("4", "System Overview");

        menuItem("0", "Logout");


        int choice = getNumber(
            "Choose an option: ",
            0,
            4
        );


        if (choice == 0) {
            showNote("Logging out...");
            pauseScreen();
            return;
        }

        else if (choice == 1) {

            viewDirectory(
                students,
                "STUDENT DIRECTORY"
            );
        }

        else if (choice == 2) {

            viewDirectory(
                teachers,
                "TEACHER DIRECTORY"
            );
        }

        else if (choice == 3) {

            viewTopicDirectory();
        }

        else if (choice == 4) {

            showSystemOverview();
        }
    }
}

void Teacher::save(int changed) {
    saveTable(
        "database/teachers.csv",
        "id,name,phone,department,email,password,primary_expertise_topics,secondary_expertise_topics",
        teachers,
        changed
    );
}

void Teacher::showProfile() {
    static const char* labels[] = {
        "Full Name", "Teacher ID", "Phone Number",
        "Department", "Email ID"
    };

    static const int column[] = {1, 0, 2, 3, 4};

    printTitle("TEACHER PROFILE");
    cout << "\n";

    for (int i = 0; i < 5; i++)
        showField(labels[i], info[column[i]]);
}

void Teacher::dashboard() {
    clearScreen();
    showProfile();
    pauseScreen();

    while (true) {
        clearScreen();

        cout << "\n Welcome, " << BOLD
             << YELLOW << name << RESET << "\n\n";

        menuItem("1", "Choose Topics");
        menuItem("2", "Update My Topics");
        menuItem("3", "Show My Topics");
        menuItem("4", "Shared Student Projects & Meetings");
        menuItem("0", "Logout");

        int c = getNumber("Choose an option: ", 0, 4);

        if (c == 0)
            return;

        if (c == 2)
            updateTopics();
        else if (c == 4)
            teacherProjectsMenu(*this);
        else
            topicScreen(c == 1);
    }
}
void loadTeachers() {
    for (auto& f : readCsv("database/teachers.csv"))
        if (f.size() > 5) {
            teachers.push_back(Teacher(f));
            addTopicsFrom(teachers.back(), f, 6);
        }
}

void registerTeacher() {
    clearScreen();
    printTitle("TEACHER REGISTRATION");

    vector<string> f(8);

    while (true) {
        f[0] = toUpper(getCsvSafeText("Teacher ID (e.g. T017): "));

        if (f[0].size() < 2 || f[0][0] != 'T') {
            showError("Teacher ID must begin with T.");
            continue;
        }

        bool exists = false;

        for (const Teacher& t : teachers) {
            if (t.getId() == f[0]) {
                exists = true;
                break;
            }
        }

        if (!exists)
            break;

        showError("This Teacher ID already exists.");
    }

    f[1] = getCsvSafeText("Full Name: ");
    f[2] = getCsvSafeText("Phone Number: ");
    f[3] = getCsvSafeText("Department: ");
    f[4] = getCsvSafeText("Email ID: ");
    f[5] = getCsvSafeText("Create Password: ");

    
    f[6] = "";
    f[7] = "";

    Teacher newTeacher(f);

    if (!appendCsvRow(
        "database/teachers.csv",
        "id,name,phone,department,email,password,primary_expertise_topics,secondary_expertise_topics",
        newTeacher.csvRow()
    )) {
        pauseScreen();
        return;
    }

    teachers.push_back(newTeacher);

    showSuccess("Teacher registration completed successfully.");
    cout << "\n Your Teacher ID: " << f[0] << "\n";
    pauseScreen();
}


void loadAdmins() {
    for (auto& f : readCsv("database/admins.csv"))
        if (f.size() > 4)
            admins.push_back(Admin(f[0], f[1], f[4]));
}
