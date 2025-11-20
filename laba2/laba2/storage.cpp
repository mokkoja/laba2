#include "storage.h"
#include "utils.h"
#include <algorithm>
#include <fstream>

using namespace std;

// IdGenerator остается без изменений
IdGenerator::IdGenerator() : nextId(1) {}
int IdGenerator::next() { return nextId++; }
void IdGenerator::setNext(int v) { if (v > nextId) nextId = v; }

// EntityManager с map
template<typename T>
int EntityManager<T>::add(const T& entity) {
    entities[entity.getId()] = entity;
    return entity.getId();
}

template<typename T>
T* EntityManager<T>::findById(int id) {
    auto it = entities.find(id);
    if (it != entities.end()) {
        return &(it->second);
    }
    return nullptr;
}

template<typename T>
bool EntityManager<T>::removeById(int id) {
    return entities.erase(id) > 0;
}

template<typename T>
map<int, T> EntityManager<T>::getAll() const {
    return entities;
}

template<typename T>
map<int, T*> EntityManager<T>::findByName(const std::string& nameSubstr) {
    map<int, T*> result;
    string lowerSubstr = nameSubstr;
    transform(lowerSubstr.begin(), lowerSubstr.end(), lowerSubstr.begin(), ::tolower);

    for (auto& pair : entities) {
        T& entity = pair.second;
        string lowerName = entity.getName();
        transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (lowerName.find(lowerSubstr) != string::npos) {
            result[pair.first] = &entity;
        }
    }
    return result;
}

template<typename T>
int EntityManager<T>::getNextId() {
    return idGen.next();
}

template<typename T>
void EntityManager<T>::updateIdGeneratorFromData() {
    int maxId = 0;
    if (!entities.empty()) {
        maxId = entities.rbegin()->first;
    }
    idGen.setNext(maxId + 1);
}

template<typename T>
void EntityManager<T>::saveToStream(ostream& os, const string& header) const {
    os << header << "\n";
    for (const auto& pair : entities) {
        os << pair.second.toSingleLine() << "\n";
    }
    os << "END" << header << "\n";
}

template<typename T>
bool EntityManager<T>::loadFromStream(istream& is, const string& header) {
    map<int, T> newEntities;
    string line;
    bool inSection = false;

    while (getline(is, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line == header) {
            inSection = true;
            continue;
        }
        if (line == "END" + header) {
            inSection = false;
            continue;
        }

        if (inSection) {
            T entity;
            if (T::fromSingleLine(line, entity)) {
                newEntities[entity.getId()] = entity;
            }
        }
    }

    if (!newEntities.empty()) {
        entities = newEntities;
        updateIdGeneratorFromData();
        return true;
    }
    return false;
}

// Storage методы
int Storage::addPipe(const Pipe& p) { return pipeManager.add(p); }
Pipe* Storage::findPipeById(int id) { return pipeManager.findById(id); }
bool Storage::removePipeById(int id) { return pipeManager.removeById(id); }
map<int, Pipe> Storage::getAllPipes() const { return pipeManager.getAll(); }
map<int, Pipe*> Storage::findPipesByName(const string& name) { return pipeManager.findByName(name); }
int Storage::getNextPipeId() { return pipeManager.getNextId(); }

int Storage::addCS(const CS& s) { return csManager.add(s); }
CS* Storage::findCSById(int id) { return csManager.findById(id); }
bool Storage::removeCSById(int id) { return csManager.removeById(id); }
map<int, CS> Storage::getAllCS() const { return csManager.getAll(); }
map<int, CS*> Storage::findCSByName(const string& name) { return csManager.findByName(name); }
int Storage::getNextCSId() { return csManager.getNextId(); }

// Добавляем недостающие методы
map<int, Pipe> Storage::getAllPipe3() {
    return pipeManager.getAll();
}

map<int, CS> Storage::getAUC5() {
    return csManager.getAll();
}

map<int, Pipe*> Storage::searchPipe3(const string& nameSubstr, int inRepairFlag) {
    return searchPipes(nameSubstr, inRepairFlag);
}

map<int, CS*> Storage::searchC5(const string& nameSubstr, double minPercentIdle) {
    return searchCS(nameSubstr, minPercentIdle);
}

// Остальные методы с map
map<int, Pipe*> Storage::searchPipes(const string& nameSubstr, int inRepairFlag) {
    map<int, Pipe*> result;
    map<int, Pipe*> byName;

    if (nameSubstr.empty()) {
        map<int, Pipe> allPipes = getAllPipes();
        for (auto& pair : allPipes) {
            byName[pair.first] = &pair.second;
        }
    }
    else {
        byName = findPipesByName(nameSubstr);
    }

    for (const auto& pair : byName) {
        Pipe* pipe = pair.second;
        if (inRepairFlag == -1 || (pipe->isInRepair() ? 1 : 0) == inRepairFlag) {
            result[pair.first] = pipe;
        }
    }
    return result;
}

map<int, CS*> Storage::searchCS(const string& nameSubstr, double minPercentIdle) {
    map<int, CS*> result;
    map<int, CS*> byName;

    if (nameSubstr.empty()) {
        map<int, CS> allCS = getAllCS();
        for (auto& pair : allCS) {
            byName[pair.first] = &pair.second;
        }
    }
    else {
        byName = findCSByName(nameSubstr);
    }

    for (const auto& pair : byName) {
        CS* cs = pair.second;
        if (minPercentIdle < 0.0 || cs->getIdlePercent() >= minPercentIdle) {
            result[pair.first] = cs;
        }
    }
    return result;
}

bool Storage::saveToFile(const string& filename) {
    ofstream f(filename);
    if (!f) return false;

    pipeManager.saveToStream(f, "PIPES");
    csManager.saveToStream(f, "CS");
    return true;
}

bool Storage::loadFromFile(const string& filename) {
    ifstream f(filename);
    if (!f) return false;

    bool pipesLoaded = pipeManager.loadFromStream(f, "PIPES");
    bool csLoaded = csManager.loadFromStream(f, "CS");

    return pipesLoaded || csLoaded;
}

Pipe Storage::createPipeInteractive(int id) {
    Pipe p;
    p.setId(id);
    p.setName(InputHelper::inputLineNonEmpty("Введите название трубы: "));
    p.setLength(InputHelper::inputDoublePositive("Введите длину (в км): "));
    p.setDiameter(InputHelper::inputIntegerPositive("Введите диаметр (в мм): "));
    p.setInRepair(InputHelper::inputZeroOne("Труба в ремонте? (1 - да, 0 - нет): ") == 1);
    return p;
}

CS Storage::createCSInteractive(int id) {
    CS s;
    s.setId(id);
    s.setName(InputHelper::inputLineNonEmpty("Введите название КС: "));
    s.setWorkshopsTotal(InputHelper::inputIntegerPositive("Введите общее количество цехов: "));

    while (true) {
        int working = InputHelper::inputIntegerPositive("Введите количество работающих цехов: ");
        if (working <= s.getWorkshopsTotal()) {
            s.setWorkshopsWorking(working);
            break;
        }
        cout << "Ошибка! Количество работающих не может превышать общее (" << s.getWorkshopsTotal() << ").\n";
    }

    s.setStationClass(InputHelper::inputLineNonEmpty("Введите класс станции: "));
    return s;
}

// Явная инстанциация шаблонов
template class EntityManager<Pipe>;
template class EntityManager<CS>;