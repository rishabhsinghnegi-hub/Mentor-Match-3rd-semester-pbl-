
#pragma once
#include "core.h"

class Student : public TopicUser {
public:
    Student(const vector<string>& f)
        : TopicUser(
            f[0], f[1], f[8],
            vector<string>(f.begin(), f.begin() + 8),
            "LOW", "HIGH", "PRIORITY"
          ) {}

    void save(int changed) override;
    void showProfile();
    void dashboard() override;
};

vector<Student> students;
void showMentorMatches(const Student& student);
void findMentors(Student& student);
void studentProjectsMenu(Student& student);

void Student::save(int changed) {
    saveTable(
        "database/students.csv",
        "id,name,father_name,phone,roll_no,course,semester,email,password,high_priority_topics,low_priority_topics",
        students,
        changed
    );
}

void TopicUser::showMyTopics() {
    for (int p = 2; p >= 1; p--) {
        vector<string> list = (p == 2) ? highList() : lowList();

        cout << "\n " << BOLD
             << (p == 2 ? GREEN : YELLOW)
             << levelText(p) << " TOPICS" << RESET << "\n";

        if (list.empty())
            cout << "     (none)\n";

        for (size_t i = 0; i < list.size(); i++)
            cout << " " << i + 1 << ". " << list[i] << "\n";
    }
}

int TopicUser::pickTopics(int p) {
    cout << "\n " << BOLD << "--- "
         << levelText(p) << " TOPICS ---" << RESET << "\n\n";

    for (size_t i = 0; i < allTopics.size(); i++) {
        int cur = priority(allTopics[i]);

        cout << " " << i + 1 << ". " << allTopics[i];

        if (cur > 0)
            cout << "   " << (cur == 2 ? GREEN : YELLOW)
                 << "(" << toLower(levelText(cur)) << ")" << RESET;

        cout << "\n";
    }

    int changed = 0;

    for (int n : readNumbers(allTopics.size()))
        if (addTopic(allTopics[n - 1], p))
            changed++;

    return changed;
}

void TopicUser::removeTopics() {
    printTitle("REMOVE TOPICS");

    vector<string> all = highList();
    size_t highCount = all.size();

    for (const string& t : lowList())
        all.push_back(t);

    if (all.empty()) {
        showNote("You have no topics to remove.");
        return;
    }

    cout << "\n";

    for (size_t i = 0; i < all.size(); i++)
        cout << " " << i + 1 << ". " << all[i] << "     "
             << (i < highCount ? GREEN : YELLOW)
             << "(" << toLower(levelText(i < highCount ? 2 : 1))
             << ")" << RESET << "\n";

    int removed = 0;

    for (int n : readNumbers(all.size()))
        if (priority(all[n - 1]) != 0) {
            removeTopic(all[n - 1]);
            removed++;
        }

    save(removed);
}

void TopicUser::updateTopics() {
    while (true) {
        clearScreen();
        printTitle("UPDATE MY TOPICS");
        showMyTopics();

        cout << "\n";
        menuItem("1", "Add or change " + word[2] + " " + toLower(noun) + " topics");
        menuItem("2", "Add or change " + word[1] + " " + toLower(noun) + " topics");
        menuItem("3", "Remove topics");
        menuItem("0", "Back");

        int c = getNumber("Choose an option: ", 0, 3);

        if (c == 0)
            return;

        clearScreen();

        if (c == 3)
            removeTopics();
        else
            save(pickTopics(c == 1 ? 2 : 1));

        pauseScreen();
    }
}

void TopicUser::topicScreen(bool choose) {
    clearScreen();

    if (choose) {
        printTitle("CHOOSE TOPICS");
        int changed = pickTopics(2);
        save(changed + pickTopics(1));
    } else {
        printTitle("MY CURRENT TOPICS");
    }

    showMyTopics();
    pauseScreen();
}

void Student::showProfile() {
    static const char* labels[] = {
        "Full Name", "Student ID", "Father's Name",
        "Phone Number", "Roll Number", "Course",
        "Semester", "Email ID"
    };

    static const int column[] = {1, 0, 2, 3, 4, 5, 6, 7};

    printTitle("STUDENT PROFILE");
    cout << "\n";

    for (int i = 0; i < 8; i++)
        showField(labels[i], info[column[i]]);
}

void Student::dashboard() {
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
        menuItem("4", "Find My Mentor");
        menuItem("5", "My Projects, Meetings & Reviews");
        menuItem("0", "Logout");

        int c = getNumber("Choose an option: ", 0, 5);

        if (c == 0)
            return;

        if (c == 2)
            updateTopics();
        else if (c == 4)
            findMentors(*this);
        else if (c == 5)
            studentProjectsMenu(*this);
        else
            topicScreen(c == 1);
    }
}

void loadStudents() {
    for (auto& f : readCsv("database/students.csv"))
        if (f.size() >= 9) {
            students.push_back(Student(f));
            addTopicsFrom(students.back(), f, 9);
        }
}

void registerStudent() {
    clearScreen();
    printTitle("STUDENT REGISTRATION");

    vector<string> f(11);

    while (true) {
        f[0] = toUpper(getCsvSafeText("Student ID (e.g. S1001): "));

        if (f[0].size() < 2 || f[0][0] != 'S') {
            showError("Student ID must begin with S.");
            continue;
        }

        bool exists = false;

        for (const Student& s : students) {
            if (s.getId() == f[0]) {
                exists = true;
                break;
            }
        }

        if (!exists)
            break;

        showError("This Student ID already exists.");
    }

    f[1] = getCsvSafeText("Full Name: ");
    f[2] = getCsvSafeText("Father's Name: ");
    f[3] = getCsvSafeText("Phone Number: ");
    f[4] = getCsvSafeText("Roll Number: ");
    f[5] = getCsvSafeText("Course: ");
    f[6] = getCsvSafeText("Semester: ");
    f[7] = getCsvSafeText("Email ID: ");
    f[8] = getCsvSafeText("Create Password: ");

    f[9] = "";
    f[10] = "";

    Student newStudent(f);

    if (!appendCsvRow(
        "database/students.csv",
        "id,name,father_name,phone,roll_no,course,semester,email,password,high_priority_topics,low_priority_topics",
        newStudent.csvRow()
    )) {
        pauseScreen();
        return;
    }

    students.push_back(newStudent);

    showSuccess("Student registration completed successfully.");
    cout << "\n Your Student ID: " << f[0] << "\n";
    pauseScreen();
}

void findMentors(Student& student) {
    clearScreen();
    printTitle("FIND MY MENTOR");

    if (student.highList().empty() && student.lowList().empty()) {
        showNote("Please select your topics before finding a mentor.");
        pauseScreen();
        return;
    }

    showMentorMatches(student);
    pauseScreen();
}
