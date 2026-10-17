
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

class Admin : public User {
public:
    Admin(string i, string n, string p)
        : User(i, n, p) {}

    void dashboard() override {}
};

vector<Teacher> teachers;
vector<Admin> admins;

void Teacher::save(int changed) {
    saveTable(
        "teachers.csv",
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
        menuItem("0", "Logout");

        int c = getNumber("Choose an option: ", 0, 3);

        if (c == 0)
            return;

        if (c == 2)
            updateTopics();
        else
            topicScreen(c == 1);
    }
}
void loadTeachers() {
    for (auto& f : readCsv("teachers.csv"))
        if (f.size() > 5) {
            teachers.push_back(Teacher(f));
            addTopicsFrom(teachers.back(), f, 6);
        }
}

void loadAdmins() {
    for (auto& f : readCsv("admins.csv"))
        if (f.size() > 4)
            admins.push_back(Admin(f[0], f[1], f[4]));
}