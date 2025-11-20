#include "entities.h"
#include "utils.h"
#include <algorithm>
#include <vector>

using namespace std;

// BaseEntity implementation
BaseEntity::BaseEntity() : id(0), name("") {}

int BaseEntity::getId() const { return id; }
void BaseEntity::setId(int newId) { id = newId; }
string BaseEntity::getName() const { return name; }
void BaseEntity::setName(const string& newName) { name = newName; }

// Pipe implementation
Pipe::Pipe() : length(0.0), diameter(0), inRepair(false) {}

double Pipe::getLength() const { return length; }
int Pipe::getDiameter() const { return diameter; }
bool Pipe::isInRepair() const { return inRepair; }

void Pipe::setLength(double l) { length = l; }
void Pipe::setDiameter(int d) { diameter = d; }
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
CS::CS() : workshopsTotal(0), workshopsWorking(0), stationClass(""), efficiency(0.0) {}

int CS::getWorkshopsTotal() const { return workshopsTotal; }
int CS::getWorkshopsWorking() const { return workshopsWorking; }
string CS::getStationClass() const { return stationClass; }
double CS::getEfficiency() const { return efficiency; }
double CS::getIdlePercent() const {
    return workshopsTotal > 0 ? 100.0 * (workshopsTotal - workshopsWorking) / workshopsTotal : 0.0;
}

void CS::setWorkshopsTotal(int total) {
    workshopsTotal = total;
    updateEfficiency();
}

void CS::setWorkshopsWorking(int working) {
    workshopsWorking = working;
    updateEfficiency();
}

void CS::setStationClass(const string& cls) { stationClass = cls; }

void CS::updateEfficiency() {
    efficiency = workshopsTotal > 0 ? 100.0 * workshopsWorking / workshopsTotal : 0.0;
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