#include "entities.h"
#include "utils.h"
#include <algorithm>
#include <vector>
#include <iomanip>

using namespace std;

// Pipe implementation
Pipe::Pipe() : id(0), name(""), length(0.0), diameter(0), inRepair(false) {}

Pipe::Pipe(int id, string name, double length, int diameter, bool inRepair)
    : id(id), name(name), length(length), diameter(diameter), inRepair(inRepair) {
}

int Pipe::getId() const { return id; }
void Pipe::setId(int newId) { id = newId; }
string Pipe::getName() const { return name; }
void Pipe::setName(const string& newName) { name = newName; }
double Pipe::getLength() const { return length; }
void Pipe::setLength(double newLength) { length = newLength; }
int Pipe::getDiameter() const { return diameter; }
void Pipe::setDiameter(int newDiameter) { diameter = newDiameter; }
bool Pipe::isInRepair() const { return inRepair; }
void Pipe::setInRepair(bool repair) { inRepair = repair; }

string Pipe::toSingleLine() const {
    ostringstream oss;
    oss << id << "|" << name << "|" << length << "|" << diameter << "|" << (inRepair ? 1 : 0);
    return oss.str();
}

bool Pipe::fromSingleLine(const string& line, Pipe& out) {
    vector<string> parts;
    string tmp;
    istringstream iss(line);
    while (getline(iss, tmp, '|')) parts.push_back(tmp);
    if (parts.size() != 5) return false;
    try {
        out.id = stoi(parts[0]);
        out.name = parts[1];
        out.length = stod(parts[2]);
        out.diameter = stoi(parts[3]);
        out.inRepair = (parts[4] == "1");
        return true;
    }
    catch (...) { return false; }
}

void Pipe::printDetails() const {
    cout << "\n--- Труба (ID=" << id << ") ---\n";
    cout << "Название: " << name << "\n";
    cout << "Длина: " << length << " км\n";
    cout << "Диаметр: " << diameter << " мм\n";
    cout << "Состояние: " << (inRepair ? "В ремонте" : "Работает") << "\n";
}

void Pipe::editInteractive() {
    cout << "Редактирование трубы ID=" << id << ". Оставьте строку пустой, чтобы не менять.\n";
    cout << "Текущее название: " << name << "\n";
    cout << "Новое название: ";
    string s; getline(cin, s); s = trim(s);
    if (!s.empty()) name = s;

    cout << "Текущая длина: " << length << " км\n";
    cout << "Новая длина (положительное число): ";
    getline(cin, s);
    if (!trim(s).empty()) {
        double v;
        if (parseDouble(s, v) && v > 0) length = v;
        else cout << "Игнорирую неверное значение длины.\n";
    }

    cout << "Текущий диаметр: " << diameter << " мм\n";
    cout << "Новый диаметр (целое положительное): ";
    getline(cin, s);
    if (!trim(s).empty()) {
        int v;
        if (parseInt(s, v) && v > 0) diameter = v;
        else cout << "Игнорирую неверное значение диаметра.\n";
    }

    cout << "Текущее состояние: " << (inRepair ? "В ремонте" : "Работает") << "\n";
    cout << "Изменить состояние? (1 - в ремонт, 0 - работает, пустая строка - не менять): ";
    getline(cin, s);
    if (!trim(s).empty()) {
        int v;
        if (parseInt(s, v) && (v == 0 || v == 1)) inRepair = (v == 1);
        else cout << "Игнорирую неверное значение состояния.\n";
    }
}

// CS implementation
CS::CS() : id(0), name(""), workshopsTotal(0), workshopsWorking(0), stationClass(""), efficiency(0.0) {}

CS::CS(int id, string name, int workshopsTotal, int workshopsWorking, string stationClass)
    : id(id), name(name), workshopsTotal(workshopsTotal), workshopsWorking(workshopsWorking), stationClass(stationClass)
{
    updateEfficiency();
}

int CS::getId() const { return id; }
void CS::setId(int newId) { id = newId; }
string CS::getName() const { return name; }
void CS::setName(const string& newName) { name = newName; }
int CS::getWorkshopsTotal() const { return workshopsTotal; }
void CS::setWorkshopsTotal(int total) {
    workshopsTotal = total;
    updateEfficiency();
}
int CS::getWorkshopsWorking() const { return workshopsWorking; }
void CS::setWorkshopsWorking(int working) {
    workshopsWorking = working;
    updateEfficiency();
}
double CS::getIdlePercent() const {
    if (workshopsTotal == 0) return 0.0;
    return 100.0 * (workshopsTotal - workshopsWorking) / workshopsTotal;
}
string CS::getStationClass() const { return stationClass; }
void CS::setStationClass(const string& cls) { stationClass = cls; }

double CS::getEfficiency() const { return efficiency; }
void CS::setEfficiency(double eff) { efficiency = eff; }
void CS::updateEfficiency() {
    efficiency = (workshopsTotal > 0) ? (100.0 * workshopsWorking / workshopsTotal) : 0.0;
}

string CS::toSingleLine() const {
    ostringstream oss;
    oss << id << "|" << name << "|" << workshopsTotal << "|" << workshopsWorking << "|" << stationClass << "|" << efficiency;
    return oss.str();
}

bool CS::fromSingleLine(const string& line, CS& out) {
    vector<string> parts;
    string tmp;
    istringstream iss(line);
    while (getline(iss, tmp, '|')) parts.push_back(tmp);
    if (parts.size() != 6) return false;
    try {
        out.id = stoi(parts[0]);
        out.name = parts[1];
        out.workshopsTotal = stoi(parts[2]);
        out.workshopsWorking = stoi(parts[3]);
        out.stationClass = parts[4];
        out.efficiency = stod(parts[5]);
        return true;
    }
    catch (...) { return false; }
}

void CS::printDetails() const {
    cout << "\n--- КС (ID=" << id << ") ---\n";
    cout << "Название: " << name << "\n";
    cout << "Класс станции: " << stationClass << "\n";
    cout << "Всего цехов: " << workshopsTotal << "\n";
    cout << "Работает цехов: " << workshopsWorking << "\n";
    cout << "Эффективность: " << fixed << setprecision(2) << efficiency << "%\n";
    cout << "Процент незадействованных цехов: " << fixed << setprecision(2) << getIdlePercent() << "%\n";
}

void CS::editInteractive() {
    cout << "Редактирование КС ID=" << id << ". Оставьте строку пустой, чтобы не менять.\n";
    cout << "Текущее название: " << name << "\n";
    cout << "Новое название: ";
    string line; getline(cin, line); line = trim(line);
    if (!line.empty()) name = line;

    cout << "Текущее общее число цехов: " << workshopsTotal << "\n";
    cout << "Новое общее число цехов (положительное, пустая строка - не менять): ";
    getline(cin, line);
    if (!trim(line).empty()) {
        int v;
        if (parseInt(line, v) && v > 0) setWorkshopsTotal(v);
        else cout << "Игнорирую неверное значение.\n";
    }

    cout << "Текущее число работающих цехов: " << workshopsWorking << "\n";
    cout << "Новое число работающих цехов (<= общее): ";
    getline(cin, line);
    if (!trim(line).empty()) {
        int v;
        if (parseInt(line, v) && v >= 0 && v <= workshopsTotal) setWorkshopsWorking(v);
        else cout << "Игнорирую неверное значение.\n";
    }

    cout << "Текущий класс станции: " << stationClass << "\n";
    cout << "Новый класс станции (пустая строка - не менять): ";
    getline(cin, line);
    if (!trim(line).empty()) stationClass = line;
}