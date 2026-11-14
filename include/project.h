#pragma once
#include "matching.h"

struct ProjectRecord {
    string id, teacherId, leaderId, teamStudentIds, teamNames, memberCount, leaderName;
    string semester, course, projectName, reason, startDate, status;
};

struct AppointmentRecord {
    string id, projectId, teacherId, studentId, type, date, status;
};

struct ReviewRecord {
    string id, projectId, teacherId, studentId, rating, createdDate;
};

vector<ProjectRecord> projectRecords;
vector<AppointmentRecord> appointments;
vector<ReviewRecord> reviews;

ProjectRecord projectFrom(const vector<string>& f) {
    ProjectRecord p;
    if (f.size() >= 13) {
        p.id=f[0]; p.teacherId=f[1]; p.leaderId=f[2]; p.teamStudentIds=f[3]; p.teamNames=f[4];
        p.memberCount=f[5]; p.leaderName=f[6]; p.semester=f[7]; p.course=f[8];
        p.projectName=f[9]; p.reason=f[10]; p.startDate=f[11];
        p.status=f[12];
    }
    return p;
}

AppointmentRecord appointmentFrom(const vector<string>& f) {
    AppointmentRecord a;
    if (f.size() >= 7) {
        a.id=f[0]; a.projectId=f[1]; a.teacherId=f[2]; a.studentId=f[3];
        a.type=f[4]; a.date=f[5]; a.status=f[6];
    }
    return a;
}

ReviewRecord reviewFrom(const vector<string>& f) {
    ReviewRecord r;
    if (f.size() >= 6) {
        r.id=f[0]; r.projectId=f[1]; r.teacherId=f[2]; r.studentId=f[3];
        r.rating=f[4]; r.createdDate=f[5];
    }
    return r;
}

void loadProjectData() {
    for (const auto& f : readCsv("database/projects.csv"))
        if (f.size() >= 13) projectRecords.push_back(projectFrom(f));
    for (const auto& f : readCsv("database/appointments.csv"))
        if (f.size() >= 7) appointments.push_back(appointmentFrom(f));
    for (const auto& f : readCsv("database/ratings.csv"))
        if (f.size() >= 6) reviews.push_back(reviewFrom(f));
}

string nextRecordId(const string& prefix, size_t count) {
    ostringstream out;
    out << prefix;
    out.width(5); out.fill('0'); out << count + 1;
    return out.str();
}

string projectCsvRow(const ProjectRecord& p) {
    vector<string> f = {p.id,p.teacherId,p.leaderId,p.teamStudentIds,p.teamNames,p.memberCount,
        p.leaderName,p.semester,p.course,p.projectName,p.reason,p.startDate,p.status};
    return join(f, ',');
}

string appointmentCsvRow(const AppointmentRecord& a) {
    return join(vector<string>{a.id,a.projectId,a.teacherId,a.studentId,a.type,a.date,a.status}, ',');
}

string reviewCsvRow(const ReviewRecord& r) {
    return join(vector<string>{r.id,r.projectId,r.teacherId,r.studentId,r.rating,r.createdDate}, ',');
}

bool saveProjectRecords() {
    ofstream out("database/projects.csv");
    if (!out) { showError("Could not save database/projects.csv"); return false; }
    out << "id,teacher_id,leader_id,team_student_ids,team_names,member_count,leader_name,semester,course,project_name,reason,start_date,status\n";
    for (const ProjectRecord& p : projectRecords) out << projectCsvRow(p) << "\n";
    return (bool)out;
}

int activeBookings(const string& teacherId, const string& date) {
    int count = 0;
    for (const AppointmentRecord& a : appointments)
        if (a.teacherId == teacherId && a.date == date && a.status == "booked") count++;
    return count;
}

string addDaysIso(const string& value, int days) {
    tm date = {};
    date.tm_year=atoi(value.substr(0,4).c_str())-1900;
    date.tm_mon=atoi(value.substr(5,2).c_str())-1;
    date.tm_mday=atoi(value.substr(8,2).c_str())+days;
    date.tm_hour=12; date.tm_isdst=-1;
    mktime(&date);
    char output[11]; strftime(output,sizeof(output),"%Y-%m-%d",&date);
    return output;
}

string chooseMeetingDate(const Teacher& teacher, const string& earliest, const string& latest) {
    vector<string> options;
    time_t now = time(NULL);
    tm cursor = *localtime(&now);
    cursor.tm_hour=12; cursor.tm_min=0; cursor.tm_sec=0; cursor.tm_mday++;
    for (int day = 0; day < 180 && options.size() < 20; day++, cursor.tm_mday++) {
        mktime(&cursor);
        char buf[11]; strftime(buf, sizeof(buf), "%Y-%m-%d", &cursor);
        string date(buf);
        if (date < earliest || date > latest || cursor.tm_wday == 0 || cursor.tm_wday == 6) continue;
        if (activeBookings(teacher.getId(), date) < 6) options.push_back(date);
    }
    if (options.empty()) return "";
    cout << "\n Available Monday-Friday meeting dates (maximum six bookings per teacher/day):\n";
    for (size_t i=0; i<options.size(); i++) cout << " " << i+1 << ". " << options[i] << "\n";
    return options[getNumber("Choose a date: ", 1, static_cast<int>(options.size()))-1];
}

bool addAppointment(const ProjectRecord& p, const string& type, const string& date) {
    AppointmentRecord a;
    a.id=nextRecordId("A", appointments.size()); a.projectId=p.id;
    a.teacherId=p.teacherId; a.studentId=p.leaderId; a.type=type; a.date=date; a.status="booked";
    if (!appendCsvRow("database/appointments.csv",
        "id,project_id,teacher_id,student_id,type,date,status", appointmentCsvRow(a))) return false;
    appointments.push_back(a);
    return true;
}

double teacherAverageRating(const string& teacherId) {
    int total=0, count=0;
    for (const ReviewRecord& r : reviews) if (r.teacherId == teacherId) {
        total += atoi(r.rating.c_str()); count++;
    }
    return count ? (double)total/count : -1.0;
}

bool hasReview(const string& projectId, const string& studentId) {
    for (const ReviewRecord& r : reviews)
        if (r.projectId == projectId && r.studentId == studentId) return true;
    return false;
}

bool projectIncludesStudent(const ProjectRecord& project, const string& studentId) {
    vector<string> ids=split(project.teamStudentIds,'|');
    return find(ids.begin(),ids.end(),studentId)!=ids.end();
}

void startProjectForTeacher(const Student& student, const Teacher& teacher) {
    clearScreen(); printTitle("START A MENTOR PROJECT");
    ProjectRecord p;
    p.id=nextRecordId("P", projectRecords.size()); p.teacherId=teacher.getId();
    p.leaderId=student.getId(); p.leaderName=student.getName(); p.startDate=currentDateIso();
    cout << "\n Teacher: " << teacher.getName() << " (" << teacher.getId() << ")\n";
    p.memberCount=to_string(getNumber("Number of team members, including you (1-20): ",1,20));
    p.teamStudentIds=student.getId(); p.teamNames=student.getName();
    for (int i=1; i<atoi(p.memberCount.c_str()); i++) {
        string memberId=toUpper(getText("Team member " + to_string(i+1) + " student ID: "));
        const Student* member=NULL;
        for (const Student& candidate:students) if(candidate.getId()==memberId) member=&candidate;
        while (!member || projectIncludesStudent(p,memberId)) {
            showError("Enter a registered student ID that is not already on this team.");
            memberId=toUpper(getText("Team member " + to_string(i+1) + " student ID: "));
            member=NULL;
            for (const Student& candidate:students) if(candidate.getId()==memberId) member=&candidate;
        }
        p.teamStudentIds += "|" + member->getId();
        p.teamNames += "|" + member->getName();
    }
    p.semester=getCsvSafeText("Team semester: ");
    p.course=getCsvSafeText("Course: ");
    p.projectName=getCsvSafeText("Project name: ");
    p.reason=getCsvSafeText("What guidance do you need from this teacher?: ");
    vector<string> meetingDates;
    string earliest=addDaysIso(p.startDate,1);
    string latest=addMonthsIso(p.startDate,2);
    for (int meeting=0; meeting<6; meeting++) {
        string meetingLatest=addDaysIso(latest,-7*(5-meeting));
        string date=chooseMeetingDate(teacher,earliest,meetingLatest);
        if (date.empty()) {
            showError("Six weekday meeting dates could not be scheduled. No project was saved.");
            pauseScreen(); return;
        }
        meetingDates.push_back(date);
        earliest=addDaysIso(date,7);
    }
    p.status="in_progress";
    if (!appendCsvRow("database/projects.csv",
        "id,teacher_id,leader_id,team_student_ids,team_names,member_count,leader_name,semester,course,project_name,reason,start_date,status",
        projectCsvRow(p))) { pauseScreen(); return; }
    projectRecords.push_back(p);
    bool allSaved=true;
    for (size_t i=0;i<meetingDates.size();i++) {
        string type=i==0 ? "initial" : "progress_"+to_string(i);
        if (!addAppointment(p,type,meetingDates[i])) { allSaved=false; break; }
    }
    if (!allSaved) {
        showError("Project saved, but an appointment could not be saved. Contact an administrator.");
    } else {
        showSuccess("Project team shared with " + teacher.getName() + "; all six progress meetings are booked.");
    }
    pauseScreen();
}

void showProject(const ProjectRecord& p) {
    showField("Project ID",p.id); showField("Project",p.projectName);
    showField("Teacher ID",p.teacherId); showField("Team leader",p.leaderName);
    showField("Team members",p.memberCount); showField("Student IDs",p.teamStudentIds); showField("Names",p.teamNames);
    showField("Course / semester",p.course + " / " + p.semester);
    showField("Reason",p.reason); showField("Started",p.startDate); showField("Status",p.status);
    cout << " Meetings:\n";
    for (const AppointmentRecord& a : appointments)
        if (a.projectId == p.id) cout << "  - " << a.type << ": " << a.date << " (" << a.status << ")\n";
    double rating=teacherAverageRating(p.teacherId);
    showField("Teacher rating",rating < 0 ? "No ratings yet" : to_string(rating).substr(0,4) + " / 5");
}

void studentProjectsMenu(Student& student) {
    while (true) {
        clearScreen(); printTitle("MY PROJECTS & REVIEWS");
        vector<size_t> mine;
        for (size_t i=0;i<projectRecords.size();i++) if(projectIncludesStudent(projectRecords[i],student.getId())) {
            mine.push_back(i); cout << "\n " << mine.size() << ". " << projectRecords[i].projectName << " with " << projectRecords[i].teacherId << " [" << projectRecords[i].status << "]\n";
        }
        if (mine.empty()) showNote("No project teams have been created yet. Find a mentor to start one.");
        menuItem("1","Mark a project completed"); menuItem("2","Submit an eligible star rating"); menuItem("0","Back");
        int choice=getNumber("Choose an option: ",0,2); if(!choice) return;
        if(mine.empty()) continue;
        int index=getNumber("Project number: ",1,static_cast<int>(mine.size()));
        ProjectRecord& p=projectRecords[mine[index-1]];
        if(choice==1) {
            p.status="completed";
            if(saveProjectRecords()) showSuccess("Project marked completed. You can now leave a star rating.");
            pauseScreen();
        } else {
            bool eligible=p.status=="completed" || currentDateIso()>=addMonthsIso(p.startDate,2);
            if(!eligible) { showNote("Reviews unlock when the project is marked completed or two months have passed."); pauseScreen(); continue; }
            if(hasReview(p.id,student.getId())) { showNote("You have already reviewed this project."); pauseScreen(); continue; }
            ReviewRecord r; r.id=nextRecordId("R",reviews.size()); r.projectId=p.id; r.teacherId=p.teacherId; r.studentId=student.getId();
            r.rating=to_string(getNumber("Star rating (1-5): ",1,5)); r.createdDate=currentDateIso();
            if(appendCsvRow("database/ratings.csv","id,project_id,teacher_id,student_id,rating,created_date",reviewCsvRow(r))) { reviews.push_back(r); showSuccess("Thank you. Your star rating is now included in the teacher's displayed rating."); }
            pauseScreen();
        }
    }
}

void teacherProjectsMenu(Teacher& teacher) {
    clearScreen(); printTitle("SHARED STUDENT PROJECTS");
    bool found=false;
    for(const ProjectRecord& p:projectRecords) if(p.teacherId==teacher.getId()) { found=true; cout << "\n"; showProject(p); }
    if(!found) showNote("No student project teams have selected you yet.");
    cout << "\n Student ratings:\n";
    for(const ReviewRecord& r:reviews) if(r.teacherId==teacher.getId())
        cout << " - Student " << r.studentId << ": " << r.rating << "/5 (" << r.createdDate << ")\n";
    pauseScreen();
}
