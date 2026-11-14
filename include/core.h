
#pragma once
#include "utils.h"

vector<string> allTopics;

void loadTopics() {
    for (auto& f : readCsv("database/topics.csv"))
        if (f[0] != "")
            allTopics.push_back(f[0]);
}

class User {
protected:
    string id, name, password;

public:
    User(string i, string n, string p)
        : id(toUpper(i)), name(n), password(p) {}

    virtual ~User() {}

    string getId() const { return id; }
    string getName() const { return name; }

    bool checkPassword(const string& p) const {
        return password == p;
    }

    bool nameMatches(const string& n) const {
        return toUpper(name) == toUpper(n);
    }

    virtual void dashboard() = 0;
};



class TopicUser : public User {
protected:
    vector<string> info;
    unordered_set<string> high, low;
    string word[3], noun;

    string levelText(int p) const {
        return word[p] + " " + noun;
    }

public:
    TopicUser(string i, string n, string p,
              vector<string> inf, string lowWord,
              string highWord, string nounText)
        : User(i, n, p), info(inf), noun(nounText) {
        info[0] = id;
        word[1] = lowWord;
        word[2] = highWord;
    }

    int priority(const string& t) const {
        return high.count(t) ? 2 : (low.count(t) ? 1 : 0);
    }

    void removeTopic(const string& t) {
        high.erase(t);
        low.erase(t);
    }

    bool addTopic(const string& t, int p) {
        if (priority(t) == p)
            return false;

        removeTopic(t);
        (p == 2 ? high : low).insert(t);
        return true;
    }

    vector<string> highList() const {
        return sorted(high);
    }

    vector<string> lowList() const {
        return sorted(low);
    }

    string csvRow() const {
        vector<string> f = info;
        f.push_back(password);
        f.push_back(join(highList(), '|'));
        f.push_back(join(lowList(), '|'));
        return join(f, ',');
    }

    virtual void save(int changed) = 0;

    void showMyTopics();
    int pickTopics(int p);
    void removeTopics();
    void updateTopics();
    void topicScreen(bool choose);
};

template <class T>
bool saveTable(const string& file, const string& header,
               const vector<T>& list, int changed) {
    if (changed == 0)
        return true;

    ofstream out(file);

    if (!out) {
        showError("Could not save to " + file);
        return false;
    }

    out << header << "\n";

    for (const T& x : list)
        out << x.csvRow() << "\n";

    out.flush();

    if (!out) {
        showError("Error occurred while writing to " + file);
        return false;
    }

    showSuccess(to_string(changed) + " record(s) saved to " + file);
    return true;
}

template <class T>
void addTopicsFrom(T& person, const vector<string>& f, size_t col) {
    for (int level = 2; level >= 1; level--, col++)
        if (col < f.size())
            for (const string& t : split(f[col], '|'))
                if (t != "")
                    person.addTopic(t, level);
}

template <class T>
void login(vector<T>& list, string title, string idLabel,
           bool askName, string fileName, bool requireToday = false) {
    if (list.empty()) {
        showError(fileName + " was not found or is empty.");
        pauseScreen();
        return;
    }

    for (int attempt = 1; attempt <= 3; attempt++) {
        clearScreen();
        printTitle(title);

        string name = askName ? getText("Full name: ") : "";
        string id = toUpper(getText(idLabel));
        string pw = getText("Password: ");
        bool dateMatches = true;
        if (requireToday) {
            string today = currentDateIso();
            cout << "\n Today's date is " << today << ".";
            dateMatches = getText("Enter today's date (YYYY-MM-DD): ") == today;
            if (!dateMatches) showError("The date must match today's date.");
        }

        for (T& item : list) {
            User* u = &item;

            if (dateMatches && u->getId() == id &&
                u->checkPassword(pw) &&
                (!askName || u->nameMatches(name))) {
                showSuccess("Login successful. Welcome, " +
                            u->getName() + "!");
                pauseScreen();
                u->dashboard();
                return;
            }
        }

        showError("Invalid login details. Attempt " +
                  to_string(attempt) + " of 3.");
        pauseScreen();
    }

    showNote("Too many failed attempts. Returning to the main screen.");
    pauseScreen();
}
