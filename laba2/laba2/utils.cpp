#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

// Вспомогательные функции
string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool parseInt(const string& s, int& out) {
    string t = trim(s);
    if (t.empty()) return false;
    try {
        size_t idx = 0;
        long long v = stoll(t, &idx);
        if (idx != t.size()) return false;
        out = static_cast<int>(v);
        return true;
    }
    catch (...) { return false; }
}

bool parseDouble(const string& s, double& out) {
    string t = trim(s);
    if (t.empty()) return false;
    try {
        size_t idx = 0;
        double v = stod(t, &idx);
        if (idx != t.size()) return false;
        out = v;
        return true;
    }
    catch (...) { return false; }
}

string currentTimestamp() {
    using namespace chrono;
    auto now = system_clock::now();
    time_t t = system_clock::to_time_t(now);
    tm buf;
#if defined(_WIN32)
    localtime_s(&buf, &t);
#else
    localtime_r(&t, &buf);
#endif
    ostringstream oss;
    oss << put_time(&buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Logger implementation
Logger::Logger() : filename("log.txt") {}
void Logger::setFile(const string& fname) { filename = fname; }
void Logger::log(const string& entry) {
    ofstream f(filename.c_str(), ios::app);
    if (!f) return;
    f << "[" << currentTimestamp() << "] " << entry << "\n";
}

Logger LOG;

// InputHelper implementation
int InputHelper::inputIntegerPositive(const string& prompt) {
    string line; int v;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) exit(0);
        if (parseInt(line, v) && v > 0) return v;
        cout << "Ошибка! Введите целое положительное число.\n";
    }
}

double InputHelper::inputDoublePositive(const string& prompt) {
    string line; double v;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) exit(0);
        if (parseDouble(line, v) && v > 0) return v;
        cout << "Ошибка! Введите положительное число (можно с дробной частью).\n";
    }
}

int InputHelper::inputZeroOne(const string& prompt) {
    string line; int v;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) exit(0);
        if (parseInt(line, v) && (v == 0 || v == 1)) return v;
        cout << "Ошибка! Введите 1 (да) или 0 (нет).\n";
    }
}

int InputHelper::inputIntInRange(const string& prompt, int minv, int maxv) {
    string line; int v;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) exit(0);
        if (parseInt(line, v) && v >= minv && v <= maxv) return v;
        cout << "Ошибка! Введите целое число от " << minv << " до " << maxv << ".\n";
    }
}

string InputHelper::inputLineNonEmpty(const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        s = trim(s);
        if (!s.empty()) return s;
        cout << "Строка не должна быть пустой.\n";
    }
}

int InputHelper::inputMenuChoice() {
    string line; int v;
    while (true) {
        if (!getline(cin, line)) exit(0);
        if (parseInt(line, v) && v >= 0 && v <= 15) return v;
        cout << "Ошибка ввода! Введите число от 0 до 15: ";
    }
}

// BatchProcessor implementation
map<int, int> BatchProcessor::askIdsFromUserSelection(const map<int, int>& candidates) {
    map<int, int> result;
    if (candidates.empty()) return result;

    cout << "Найденные ID: ";
    for (const auto& pair : candidates) cout << pair.first << " ";
    cout << "\nВыберите:\n"
        << " 1) Применить ко всем найденным\n"
        << " 2) Ввести список ID через пробел/запятую (подмножество)\n"
        << " 0) Отмена\n"
        << "Ваш выбор: ";

    string opt;
    getline(cin, opt);
    opt = trim(opt);

    if (opt == "1") {
        return candidates;
    }
    else if (opt == "0" || opt.empty()) {
        return {};
    }
    else {
        map<int, int> chosen;
        istringstream iss(opt);
        string tok;

        while (iss >> tok) {
            tok.erase(remove(tok.begin(), tok.end(), ','), tok.end());
            int v;
            if (parseInt(tok, v)) chosen[v] = v;
        }

        map<int, int> filtered;
        for (const auto& pair : chosen) {
            int id = pair.first;
            if (candidates.find(id) != candidates.end()) {
                filtered[id] = id;
            }
        }
        return filtered;
    }
}