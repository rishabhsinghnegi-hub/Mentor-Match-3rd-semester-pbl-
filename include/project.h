#pragma once
#include "matching.h"

struct ProjectRecord {
    string id;
    string teacherId;
    string leaderId;
    string teamStudentIds;
    string teamNames;
    string memberCount;
    string leaderName;
    string semester;
    string course;
    string projectName;
    string reason;
    string startDate;
    string status;
};

struct AppointmentRecord {
    string id;
    string projectId;
    string teacherId;
    string studentId;
    string type;
    string date;
    string status;
};

struct ReviewRecord {
    string id;
    string projectId;
    string teacherId;
    string studentId;
    string rating;
    string createdDate;
};

vector<ProjectRecord> projectRecords;
vector<AppointmentRecord> appointments;
vector<ReviewRecord> reviews;

void loadProjectData() {
    vector<vector<string>> projectRows = readCsv("database/projects.csv");
    for (const vector<string>& fields : projectRows) {
        if (fields.size() < 13) {
            continue;
        }

        ProjectRecord project;
        project.id = fields[0];
        project.teacherId = fields[1];
        project.leaderId = fields[2];
        project.teamStudentIds = fields[3];
        project.teamNames = fields[4];
        project.memberCount = fields[5];
        project.leaderName = fields[6];
        project.semester = fields[7];
        project.course = fields[8];
        project.projectName = fields[9];
        project.reason = fields[10];
        project.startDate = fields[11];
        project.status = fields[12];
        projectRecords.push_back(project);
    }

    vector<vector<string>> appointmentRows = readCsv("database/appointments.csv");
    for (const vector<string>& fields : appointmentRows) {
        if (fields.size() < 7) {
            continue;
        }

        AppointmentRecord appointment;
        appointment.id = fields[0];
        appointment.projectId = fields[1];
        appointment.teacherId = fields[2];
        appointment.studentId = fields[3];
        appointment.type = fields[4];
        appointment.date = fields[5];
        appointment.status = fields[6];
        appointments.push_back(appointment);
    }

    vector<vector<string>> ratingRows = readCsv("database/ratings.csv");
    for (const vector<string>& fields : ratingRows) {
        if (fields.size() < 6) {
            continue;
        }

        ReviewRecord rating;
        rating.id = fields[0];
        rating.projectId = fields[1];
        rating.teacherId = fields[2];
        rating.studentId = fields[3];
        rating.rating = fields[4];
        rating.createdDate = fields[5];
        reviews.push_back(rating);
    }
}

string nextRecordId(const string& prefix, size_t count) {
    ostringstream output;
    output << prefix;
    output.width(5);
    output.fill('0');
    output << count + 1;
    return output.str();
}

bool saveProjectRecords() {
    ofstream output("database/projects.csv");
    if (!output) {
        showError("Could not save database/projects.csv");
        return false;
    }

    output << "id,teacher_id,leader_id,team_student_ids,team_names,member_count,"
           << "leader_name,semester,course,project_name,reason,start_date,status\n";

    for (const ProjectRecord& project : projectRecords) {
        vector<string> fields;
        fields.push_back(project.id);
        fields.push_back(project.teacherId);
        fields.push_back(project.leaderId);
        fields.push_back(project.teamStudentIds);
        fields.push_back(project.teamNames);
        fields.push_back(project.memberCount);
        fields.push_back(project.leaderName);
        fields.push_back(project.semester);
        fields.push_back(project.course);
        fields.push_back(project.projectName);
        fields.push_back(project.reason);
        fields.push_back(project.startDate);
        fields.push_back(project.status);
        output << join(fields, ',') << "\n";
    }

    return (bool)output;
}

int activeBookings(const string& teacherId, const string& date) {
    int count = 0;

    for (const AppointmentRecord& appointment : appointments) {
        if (appointment.teacherId == teacherId &&
            appointment.date == date &&
            appointment.status == "booked") {
            count++;
        }
    }

    return count;
}

string addDaysIso(const string& value, int days) {
    tm date = {};
    date.tm_year = atoi(value.substr(0, 4).c_str()) - 1900;
    date.tm_mon = atoi(value.substr(5, 2).c_str()) - 1;
    date.tm_mday = atoi(value.substr(8, 2).c_str()) + days;
    date.tm_hour = 12;
    date.tm_isdst = -1;
    mktime(&date);

    char output[11];
    strftime(output, sizeof(output), "%Y-%m-%d", &date);
    return output;
}

string chooseMeetingDate(const Teacher& teacher,
                        const string& earliest,
                        const string& latest) {
    vector<string> availableDates;
    time_t now = time(NULL);
    tm cursor = *localtime(&now);
    cursor.tm_hour = 12;
    cursor.tm_min = 0;
    cursor.tm_sec = 0;
    cursor.tm_mday++;

    for (int day = 0; day < 180; day++) {
        if (availableDates.size() == 20) {
            break;
        }

        mktime(&cursor);
        char dateText[11];
        strftime(dateText, sizeof(dateText), "%Y-%m-%d", &cursor);
        string date = dateText;

        bool isWeekend = cursor.tm_wday == 0 || cursor.tm_wday == 6;
        if (date < earliest || date > latest || isWeekend) {
            cursor.tm_mday++;
            continue;
        }

        if (activeBookings(teacher.getId(), date) < 6) {
            availableDates.push_back(date);
        }

        cursor.tm_mday++;
    }

    if (availableDates.empty()) {
        return "";
    }

    cout << "\n Available Monday-Friday meeting dates."
         << " A teacher can have six meetings per date.\n";

    for (size_t i = 0; i < availableDates.size(); i++) {
        cout << " " << i + 1 << ". " << availableDates[i] << "\n";
    }

    int choice = getNumber("Choose a date: ", 1,
                           static_cast<int>(availableDates.size()));
    return availableDates[choice - 1];
}

bool addAppointment(const ProjectRecord& project,
                    const string& meetingType,
                    const string& date) {
    AppointmentRecord appointment;
    appointment.id = nextRecordId("A", appointments.size());
    appointment.projectId = project.id;
    appointment.teacherId = project.teacherId;
    appointment.studentId = project.leaderId;
    appointment.type = meetingType;
    appointment.date = date;
    appointment.status = "booked";

    vector<string> fields;
    fields.push_back(appointment.id);
    fields.push_back(appointment.projectId);
    fields.push_back(appointment.teacherId);
    fields.push_back(appointment.studentId);
    fields.push_back(appointment.type);
    fields.push_back(appointment.date);
    fields.push_back(appointment.status);

    bool saved = appendCsvRow(
        "database/appointments.csv",
        "id,project_id,teacher_id,student_id,type,date,status",
        join(fields, ',')
    );

    if (saved) {
        appointments.push_back(appointment);
    }

    return saved;
}

double teacherAverageRating(const string& teacherId) {
    int total = 0;
    int count = 0;

    for (const ReviewRecord& rating : reviews) {
        if (rating.teacherId == teacherId) {
            total += atoi(rating.rating.c_str());
            count++;
        }
    }

    if (count == 0) {
        return -1.0;
    }

    return static_cast<double>(total) / count;
}

bool hasReview(const string& projectId, const string& studentId) {
    for (const ReviewRecord& rating : reviews) {
        if (rating.projectId == projectId && rating.studentId == studentId) {
            return true;
        }
    }

    return false;
}

bool projectIncludesStudent(const ProjectRecord& project,
                            const string& studentId) {
    vector<string> studentIds = split(project.teamStudentIds, '|');

    for (const string& id : studentIds) {
        if (id == studentId) {
            return true;
        }
    }

    return false;
}

void startProjectForTeacher(const Student& student, const Teacher& teacher) {
    clearScreen();
    printTitle("START A MENTOR PROJECT");

    ProjectRecord project;
    project.id = nextRecordId("P", projectRecords.size());
    project.teacherId = teacher.getId();
    project.leaderId = student.getId();
    project.leaderName = student.getName();
    project.startDate = currentDateIso();

    cout << "\n Teacher: " << teacher.getName()
         << " (" << teacher.getId() << ")\n";

    int memberCount = getNumber(
        "Number of team members, including you (1-20): ", 1, 20
    );
    project.memberCount = to_string(memberCount);
    project.teamStudentIds = student.getId();
    project.teamNames = student.getName();

    for (int i = 1; i < memberCount; i++) {
        string memberId = toUpper(
            getText("Team member " + to_string(i + 1) + " student ID: ")
        );
        string memberName = "";
        bool foundMember = false;

        for (const Student& candidate : students) {
            if (candidate.getId() == memberId) {
                memberName = candidate.getName();
                foundMember = true;
                break;
            }
        }

        while (!foundMember || projectIncludesStudent(project, memberId)) {
            showError("Enter a registered student ID that is not already on this team.");
            memberId = toUpper(
                getText("Team member " + to_string(i + 1) + " student ID: ")
            );
            memberName = "";
            foundMember = false;

            for (const Student& candidate : students) {
                if (candidate.getId() == memberId) {
                    memberName = candidate.getName();
                    foundMember = true;
                    break;
                }
            }
        }

        project.teamStudentIds += "|" + memberId;
        project.teamNames += "|" + memberName;
    }

    project.semester = getCsvSafeText("Team semester: ");
    project.course = getCsvSafeText("Course: ");
    project.projectName = getCsvSafeText("Project name: ");
    project.reason = getCsvSafeText("What guidance do you need from this teacher?: ");

    vector<string> meetingDates;
    string earliestDate = addDaysIso(project.startDate, 1);
    string latestDate = addMonthsIso(project.startDate, 2);

    for (int meeting = 0; meeting < 6; meeting++) {
        int weeksLeft = 5 - meeting;
        string lastPossibleDate = addDaysIso(latestDate, -7 * weeksLeft);
        string date = chooseMeetingDate(teacher, earliestDate, lastPossibleDate);

        if (date == "") {
            showError("Six weekday meetings could not be scheduled. No project was saved.");
            pauseScreen();
            return;
        }

        meetingDates.push_back(date);
        earliestDate = addDaysIso(date, 7);
    }

    project.status = "in_progress";

    vector<string> projectFields;
    projectFields.push_back(project.id);
    projectFields.push_back(project.teacherId);
    projectFields.push_back(project.leaderId);
    projectFields.push_back(project.teamStudentIds);
    projectFields.push_back(project.teamNames);
    projectFields.push_back(project.memberCount);
    projectFields.push_back(project.leaderName);
    projectFields.push_back(project.semester);
    projectFields.push_back(project.course);
    projectFields.push_back(project.projectName);
    projectFields.push_back(project.reason);
    projectFields.push_back(project.startDate);
    projectFields.push_back(project.status);

    bool projectSaved = appendCsvRow(
        "database/projects.csv",
        "id,teacher_id,leader_id,team_student_ids,team_names,member_count,leader_name,semester,course,project_name,reason,start_date,status",
        join(projectFields, ',')
    );

    if (!projectSaved) {
        pauseScreen();
        return;
    }

    projectRecords.push_back(project);
    bool allMeetingsSaved = true;

    for (size_t i = 0; i < meetingDates.size(); i++) {
        string meetingType;
        if (i == 0) {
            meetingType = "initial";
        } else {
            meetingType = "progress_" + to_string(i);
        }

        if (!addAppointment(project, meetingType, meetingDates[i])) {
            allMeetingsSaved = false;
            break;
        }
    }

    if (allMeetingsSaved) {
        showSuccess("Project shared with " + teacher.getName()
                    + ". All six progress meetings are booked.");
    } else {
        showError("Project saved, but a meeting could not be saved.");
    }

    pauseScreen();
}

void showProject(const ProjectRecord& project) {
    showField("Project ID", project.id);
    showField("Project", project.projectName);
    showField("Teacher ID", project.teacherId);
    showField("Team leader", project.leaderName);
    showField("Team members", project.memberCount);
    showField("Student IDs", project.teamStudentIds);
    showField("Names", project.teamNames);
    showField("Course / semester", project.course + " / " + project.semester);
    showField("Reason", project.reason);
    showField("Started", project.startDate);
    showField("Status", project.status);

    cout << " Meetings:\n";
    for (const AppointmentRecord& appointment : appointments) {
        if (appointment.projectId == project.id) {
            cout << "  - " << appointment.type << ": "
                 << appointment.date << " (" << appointment.status << ")\n";
        }
    }

    double rating = teacherAverageRating(project.teacherId);
    if (rating < 0) {
        showField("Teacher rating", "No ratings yet");
    } else {
        showField("Teacher rating", to_string(rating).substr(0, 4) + " / 5");
    }
}

void studentProjectsMenu(Student& student) {
    while (true) {
        clearScreen();
        printTitle("MY PROJECTS & RATINGS");

        vector<size_t> myProjectIndexes;
        for (size_t i = 0; i < projectRecords.size(); i++) {
            if (projectIncludesStudent(projectRecords[i], student.getId())) {
                myProjectIndexes.push_back(i);
                cout << "\n " << myProjectIndexes.size() << ". "
                     << projectRecords[i].projectName
                     << " with " << projectRecords[i].teacherId
                     << " [" << projectRecords[i].status << "]\n";
            }
        }

        if (myProjectIndexes.empty()) {
            showNote("No project teams have been created yet. Find a mentor to start one.");
        }

        menuItem("1", "Mark a project completed");
        menuItem("2", "Submit an eligible star rating");
        menuItem("0", "Back");

        int choice = getNumber("Choose an option: ", 0, 2);
        if (choice == 0) {
            return;
        }

        if (myProjectIndexes.empty()) {
            continue;
        }

        int projectNumber = getNumber(
            "Project number: ", 1, static_cast<int>(myProjectIndexes.size())
        );
        ProjectRecord& project = projectRecords[myProjectIndexes[projectNumber - 1]];

        if (choice == 1) {
            project.status = "completed";
            if (saveProjectRecords()) {
                showSuccess("Project marked completed. You can now leave a star rating.");
            }
            pauseScreen();
            continue;
        }

        bool twoMonthsPassed = currentDateIso() >= addMonthsIso(project.startDate, 2);
        if (project.status != "completed" && !twoMonthsPassed) {
            showNote("Ratings unlock when the project is completed or two months have passed.");
            pauseScreen();
            continue;
        }

        if (hasReview(project.id, student.getId())) {
            showNote("You have already rated this project.");
            pauseScreen();
            continue;
        }

        ReviewRecord rating;
        rating.id = nextRecordId("R", reviews.size());
        rating.projectId = project.id;
        rating.teacherId = project.teacherId;
        rating.studentId = student.getId();
        rating.rating = to_string(getNumber("Star rating (1-5): ", 1, 5));
        rating.createdDate = currentDateIso();

        vector<string> ratingFields;
        ratingFields.push_back(rating.id);
        ratingFields.push_back(rating.projectId);
        ratingFields.push_back(rating.teacherId);
        ratingFields.push_back(rating.studentId);
        ratingFields.push_back(rating.rating);
        ratingFields.push_back(rating.createdDate);

        bool ratingSaved = appendCsvRow(
            "database/ratings.csv",
            "id,project_id,teacher_id,student_id,rating,created_date",
            join(ratingFields, ',')
        );

        if (ratingSaved) {
            reviews.push_back(rating);
            showSuccess("Thank you. Your star rating is now included in the teacher's average.");
        }

        pauseScreen();
    }
}

void teacherProjectsMenu(Teacher& teacher) {
    clearScreen();
    printTitle("SHARED STUDENT PROJECTS");

    bool foundProject = false;
    for (const ProjectRecord& project : projectRecords) {
        if (project.teacherId == teacher.getId()) {
            foundProject = true;
            cout << "\n";
            showProject(project);
        }
    }

    if (!foundProject) {
        showNote("No student project teams have selected you yet.");
    }

    cout << "\n Student ratings:\n";
    for (const ReviewRecord& rating : reviews) {
        if (rating.teacherId == teacher.getId()) {
            cout << " - Student " << rating.studentId << ": "
                 << rating.rating << "/5 (" << rating.createdDate << ")\n";
        }
    }

    pauseScreen();
}
