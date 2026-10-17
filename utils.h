
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <cstdlib>
#include <cctype>
using namespace std;

const string RESET = "\033[0m", BOLD = "\033[1m",
             RED = "\033[31m", GREEN = "\033[32m",
             YELLOW = "\033[33m", CYAN = "\033[36m",
             BLUE = "\033[94m";

string readLine() {
    string line;
    if (!getline(cin, line)) exit(0);

    size_t a = 0, b = line.size();
    while (a < b && isspace((unsigned char)line[a])) a++;
    while (b > a && isspace((unsigned char)line[b - 1])) b--;

    return line.substr(a, b - a);
}

string toUpper(string s) {
    for (char& c : s)
        c = toupper((unsigned char)c);
    return s;
}

string toLower(string s) {
    for (char& c : s)
        c = tolower((unsigned char)c);
    return s;
}

void trimRight(string& s) {
    while (!s.empty() && isspace((unsigned char)s.back()))
        s.pop_back();
}

vector<string> split(const string& s, char sep) {
    vector<string> parts(1);

    for (char c : s) {
        if (c == sep)
            parts.push_back("");
        else
            parts.back() += c;
    }

    return parts;
}

string join(const vector<string>& v, char sep) {
    string r;

    for (size_t i = 0; i < v.size(); i++)
        r += (i ? string(1, sep) : "") + v[i];

    return r;
}

vector<vector<string>> readCsv(const string& file) {
    vector<vector<string>> rows;
    ifstream in(file);
    string line;

    getline(in, line);

    while (getline(in, line)) {
        vector<string> f = split(line, ',');
        for (string& x : f)
            trimRight(x);
        rows.push_back(f);
    }

    return rows;
}

void clearScreen() {
    cout << "\033[2J\033[H";
}

void pauseScreen() {
    cout << "\n Press ENTER to continue...";
    readLine();
}

void printTitle(string text) {
    cout << "\n" << CYAN
         << "-----------------------------------" << RESET << "\n"
         << BOLD << CYAN << " " << text << RESET << "\n"
         << CYAN << "-----------------------------------" << RESET << "\n";
}

void showError(string m) {
    cout << "\n " << RED << "ERROR: " << m << RESET << "\n";
}

void showSuccess(string m) {
    cout << "\n " << GREEN << "SUCCESS: " << m << RESET << "\n";
}

void showNote(string m) {
    cout << "\n " << YELLOW << "NOTE: " << m << RESET << "\n";
}

void menuItem(string n, string t) {
    cout << " " << CYAN << "[" << n << "]" << RESET
         << " " << t << "\n";
}

void showField(string label, string value) {
    cout << " " << YELLOW << label << RESET
         << string(label.length() < 15 ? 15 - label.length() : 1, ' ')
         << ": " << value << "\n";
}

int getNumber(string prompt, int low, int high) {
    while (true) {
        cout << "\n " << CYAN << prompt << RESET;
        string line = readLine();
        int n = atoi(line.c_str());

        if (isdigit((unsigned char)line[0]) &&
            n >= low && n <= high)
            return n;

        showError("Please enter a number between " +
                  to_string(low) + " and " + to_string(high) + ".");
    }
}

string getText(string prompt) {
    while (true) {
        cout << "\n " << CYAN << prompt << RESET;
        string t = readLine();

        if (t != "")
            return t;

        showError("This field cannot be empty.");
    }
}

vector<int> readNumbers(int maxNumber) {
    cout << "\n " << CYAN
         << "Enter numbers separated by spaces (e.g. 1 4 7), or ENTER to skip: "
         << RESET;

    istringstream in(readLine());
    vector<int> picked;
    string word;

    while (in >> word) {
        int n = atoi(word.c_str());

        if (n >= 1 && n <= maxNumber)
            picked.push_back(n);
        else
            showNote("'" + word + "' is not valid, ignored.");
    }

    return picked;
}

void printBanner() {
    cout << BOLD << YELLOW
         << "              GRAPHIC ERA HILL UNIVERSITY"
         << RESET << "\n\n";
cout << BLUE;
cout << R"(
███╗   ███╗███████╗███╗   ██╗████████╗ ██████╗ ██████╗
████╗ ████║██╔════╝████╗  ██║╚══██╔══╝██╔═══██╗██╔══██╗
██╔████╔██║█████╗  ██╔██╗ ██║   ██║   ██║   ██║██████╔╝
██║╚██╔╝██║██╔══╝  ██║╚██╗██║   ██║   ██║   ██║██╔══██╗
██║ ╚═╝ ██║███████╗██║ ╚████║   ██║   ╚██████╔╝██║  ██║
╚═╝     ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝
)" << RESET;

cout << YELLOW;
cout << R"(
     ███╗   ███╗ █████╗ ████████╗ ██████╗██╗  ██╗
     ████╗ ████║██╔══██╗╚══██╔══╝██╔════╝██║  ██║
     ██╔████╔██║███████║   ██║   ██║     ███████║
     ██║╚██╔╝██║██╔══██║   ██║   ██║     ██╔══██║
     ██║ ╚═╝ ██║██║  ██║   ██║   ╚██████╗██║  ██║
     ╚═╝     ╚═╝╚═╝  ╚═╝   ╚═╝    ╚═════╝╚═╝  ╚═╝
)" << RESET;
    cout << "\n " << BOLD
         << "Find the right faculty mentor" << RESET << "\n\n";
}

vector<string> sorted(const unordered_set<string>& s) {
    vector<string> v(s.begin(), s.end());
    sort(v.begin(), v.end());
    return v;
}
