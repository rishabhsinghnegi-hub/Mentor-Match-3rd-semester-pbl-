
#pragma once
#include "staff.h"
#include <queue>

struct MentorMatch {
    const Teacher* teacher;
    int score;
    vector<pair<string, int>> matchedTopics;
};

struct MentorMatchCompare {
    bool operator()(const MentorMatch& a, const MentorMatch& b) const {
        if (a.score != b.score)
            return a.score < b.score;
        return a.teacher->getName() > b.teacher->getName();
    }
};

void showMentorMatches(const Student& student) {
    if (teachers.empty()) {
        showNote("No teachers are registered in the system.");
        return;
    }

    priority_queue<MentorMatch, vector<MentorMatch>, MentorMatchCompare> ranking;

    vector<string> studentHigh = student.highList();
    vector<string> studentLow = student.lowList();

    for (const Teacher& teacher : teachers) {
        vector<string> primary = teacher.highList();
        vector<string> secondary = teacher.lowList();

        unordered_set<string> primarySet(primary.begin(), primary.end());
        unordered_set<string> secondarySet(secondary.begin(), secondary.end());

        MentorMatch match;
        match.teacher = &teacher;
        match.score = 0;

        for (const string& topic : studentHigh) {
            int points = 0;

            if (primarySet.count(topic))
                points = 4;
            else if (secondarySet.count(topic))
                points = 2;

            if (points > 0) {
                match.score += points;
                match.matchedTopics.push_back({topic, points});
            }
        }

        for (const string& topic : studentLow) {
            int points = 0;

            if (primarySet.count(topic))
                points = 2;
            else if (secondarySet.count(topic))
                points = 1;

            if (points > 0) {
                match.score += points;
                match.matchedTopics.push_back({topic, points});
            }
        }

        if (match.score > 0)
            ranking.push(match);
    }

    if (ranking.empty()) {
        showNote("No mentors currently match your selected topics.");
        return;
    }

    printTitle("TOP MENTOR RECOMMENDATIONS");
    cout << "\n Ranked mentors based on your topic preferences:\n";

    int rank = 1;

    while (!ranking.empty() && rank <= 3) {
        MentorMatch match = ranking.top();
        ranking.pop();

        cout << "\n " << BOLD << GREEN << "RANK " << rank << RESET << "\n";

        showField("Teacher Name", match.teacher->getName());
        showField("Teacher ID", match.teacher->getId());

        cout << "\n " << BOLD << "Score Breakdown:" << RESET << "\n";

        for (const auto& topic : match.matchedTopics) {
            cout << "   - " << topic.first
                 << " : +" << topic.second << " points\n";
        }

        cout << "\n";
        showField("Total Matching Score", to_string(match.score));

        rank++;
    }

    cout << "\n";
}
