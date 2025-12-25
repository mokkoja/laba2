#include "network.h"
#include "storage.h"
#include "utils.h"
#include <algorithm>
#include <fstream>
#include <stack>
#include <queue>
#include <cctype>

using namespace std;

// Connection implementation
Connection::Connection() : id(0), pipeId(0), csInId(0), csOutId(0), isActive(true) {}

Connection::Connection(int id, int pipeId, int csInId, int csOutId)
    : id(id), pipeId(pipeId), csInId(csInId), csOutId(csOutId), isActive(true) {
}

string Connection::toSingleLine() const {
    ostringstream oss;
    oss << id << "|" << pipeId << "|" << csInId << "|" << csOutId << "|" << (isActive ? 1 : 0);
    return oss.str();
}

bool Connection::fromSingleLine(const string& line, Connection& out) {
    vector<string> parts;
    string tmp;
    istringstream iss(line);
    while (getline(iss, tmp, '|')) parts.push_back(tmp);
    if (parts.size() != 5) return false;
    try {
        out.id = stoi(parts[0]);
        out.pipeId = stoi(parts[1]);
        out.csInId = stoi(parts[2]);
        out.csOutId = stoi(parts[3]);
        out.isActive = (parts[4] == "1");
        return true;
    }
    catch (...) { return false; }
}

void Connection::printDetails() const {
    cout << "\n--- Соединение [ID=" << id << "] ---\n";
    cout << "Труба ID: " << pipeId << "\n";
    cout << "КС входа ID: " << csInId << "\n";
    cout << "КС выхода ID: " << csOutId << "\n";
    cout << "Статус: " << (isActive ? "Активно" : "Неактивно") << "\n";
}

// GasNetwork implementation
GasNetwork::GasNetwork() : nextId(1) {}

bool GasNetwork::isValidDiameter(int diameter) const {
    return diameter == 500 || diameter == 700 ||
        diameter == 1000 || diameter == 1400;
}

int GasNetwork::getNextId() {
    return nextId++;
}

int GasNetwork::addConnection(const Connection& conn) {
    connections[conn.id] = conn;
    if (conn.id >= nextId) {
        nextId = conn.id + 1;
    }
    return conn.id;
}

bool GasNetwork::removeConnection(int id) {
    return connections.erase(id) > 0;
}

map<int, Connection> GasNetwork::getAllConnections() const {
    return connections;
}

Connection* GasNetwork::findConnectionById(int id) {
    auto it = connections.find(id);
    return (it != connections.end()) ? &it->second : nullptr;
}

map<int, Pipe*> GasNetwork::findAvailablePipesByDiameter(int diameter, Storage& storage) {
    if (!isValidDiameter(diameter)) {
        return {};
    }

    // Получаем все трубы нужного диаметра
    map<int, Pipe*> allPipes = storage.getAllPipesMap();
    map<int, Pipe*> result;

    // Собираем ID труб, уже используемых в соединениях
    set<int> usedPipeIds;
    for (const auto& pair : connections) {
        if (pair.second.isActive) {
            usedPipeIds.insert(pair.second.pipeId);
        }
    }

    // Фильтруем: нужный диаметр, не в ремонте, не используется
    for (const auto& pair : allPipes) {
        Pipe* pipe = pair.second;
        if (pipe->getDiameter() == diameter &&
            !pipe->isInRepair() &&
            usedPipeIds.find(pipe->getId()) == usedPipeIds.end()) {
            result[pipe->getId()] = pipe;
        }
    }

    return result;
}

bool GasNetwork::createConnectionInteractive(Storage& storage) {
    cout << "\n=== СОЗДАНИЕ СОЕДИНЕНИЯ ===\n";

    // Выбор диаметра с валидацией
    cout << "Допустимые диаметры: 500, 700, 1000, 1400 мм\n";
    int diameter = InputHelper::inputIntegerPositive("Введите диаметр трубы (мм): ");

    if (!isValidDiameter(diameter)) {
        cout << "Ошибка! Допустимые диаметры: 500, 700, 1000, 1400 мм\n";
        return false;
    }

    // Поиск доступных труб
    map<int, Pipe*> availablePipes = findAvailablePipesByDiameter(diameter, storage);
    int pipeId = 0;

    if (!availablePipes.empty()) {
        // Берем первую доступную трубу (по заданию)
        pipeId = availablePipes.begin()->first;
        Pipe* pipe = availablePipes.begin()->second;
        cout << "Найдена свободная труба ID=" << pipeId
            << " (длина: " << pipe->getLength() << " км)\n";
    }
    else {
        // Создание новой трубы
        cout << "Свободных труб диаметром " << diameter << " мм не найдено.\n";
        cout << "Создаем новую трубу...\n";

        pipeId = storage.getNextPipeId();
        Pipe newPipe = storage.createPipeInteractive(pipeId);
        newPipe.setDiameter(diameter);  // Устанавливаем нужный диаметр

        // Сохраняем трубу
        storage.addPipe(newPipe);
        cout << "Создана новая труба ID=" << pipeId << " диаметром " << diameter << " мм\n";
    }

    // Ввод КС входа
    int csInId = InputHelper::inputIntegerPositive("Введите ID КС входа: ");
    CS* csIn = storage.findCSById(csInId);
    if (!csIn) {
        cout << "КС входа с ID=" << csInId << " не найдена.\n";
        return false;
    }

    // Ввод КС выхода
    int csOutId = InputHelper::inputIntegerPositive("Введите ID КС выхода: ");
    CS* csOut = storage.findCSById(csOutId);
    if (!csOut) {
        cout << "КС выхода с ID=" << csOutId << " не найдена.\n";
        return false;
    }

    // Проверка что КС разные
    if (csInId == csOutId) {
        cout << "Ошибка! КС входа и выхода не могут быть одинаковыми.\n";
        return false;
    }

    // Создание соединения
    int connId = getNextId();
    Connection conn(connId, pipeId, csInId, csOutId);
    addConnection(conn);

    cout << "\n Соединение успешно создано!\n";
    cout << "ID соединения: " << connId << "\n";
    cout << "Использует трубу ID=" << pipeId << " диаметром " << diameter << " мм\n";
    cout << "КС входа -> КС выхода: " << csInId << " -> " << csOutId << "\n";

    LOG.log("Created connection: ID=" + to_string(connId) +
        ", pipe=" + to_string(pipeId) +
        ", CS_in=" + to_string(csInId) +
        ", CS_out=" + to_string(csOutId));

    return true;
}

vector<int> GasNetwork::topologicalSort(Storage& storage) const {
    vector<int> result;
    map<int, int> inDegree;
    map<int, vector<int>> adjList;

    // Инициализируем все КС из соединений
    for (const auto& connPair : connections) {
        const Connection& conn = connPair.second;
        if (conn.isActive) {
            // Убедимся что обе КС существуют
            CS* csIn = storage.findCSById(conn.csInId);
            CS* csOut = storage.findCSById(conn.csOutId);

            if (csIn && csOut) {
                adjList[conn.csInId].push_back(conn.csOutId);
                inDegree[conn.csOutId]++;

                // Добавляем КС входа если её еще нет
                if (inDegree.find(conn.csInId) == inDegree.end()) {
                    inDegree[conn.csInId] = 0;
                }
            }
        }
    }

    // Очередь для вершин с нулевой входящей степенью
    queue<int> zeroInDegree;
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {
            zeroInDegree.push(pair.first);
        }
    }

    // Алгоритм Кана
    while (!zeroInDegree.empty()) {
        int csId = zeroInDegree.front();
        zeroInDegree.pop();
        result.push_back(csId);

        if (adjList.find(csId) != adjList.end()) {
            for (int neighbor : adjList[csId]) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    zeroInDegree.push(neighbor);
                }
            }
        }
    }

    return result;
}

bool GasNetwork::hasCycles(Storage& storage) const {
    vector<int> sorted = topologicalSort(storage);
    map<int, int> inDegree;

    // Подсчитываем КС с ненулевой входящей степенью
    for (const auto& connPair : connections) {
        if (connPair.second.isActive) {
            inDegree[connPair.second.csOutId]++;
        }
    }

    // Если отсортировали не все вершины - есть циклы
    return sorted.size() != inDegree.size();
}

void GasNetwork::printNetworkGraph(Storage& storage) const {
    cout << "\n=== ГРАФ ГАЗОТРАНСПОРТНОЙ СЕТИ ===\n";

    if (connections.empty()) {
        cout << "Сеть пуста. Нет соединений.\n";
        return;
    }

    // Группируем соединения по КС входа
    map<int, vector<int>> adjacency;
    for (const auto& connPair : connections) {
        if (connPair.second.isActive) {
            adjacency[connPair.second.csInId].push_back(connPair.second.csOutId);
        }
    }

    // Выводим граф
    for (const auto& pair : adjacency) {
        CS* cs = storage.findCSById(pair.first);
        if (cs) {
            cout << "КС " << pair.first << " \"" << cs->getName() << "\" -> ";

            for (size_t i = 0; i < pair.second.size(); ++i) {
                CS* target = storage.findCSById(pair.second[i]);
                if (target) {
                    cout << "КС " << pair.second[i] << " \"" << target->getName() << "\"";
                    if (i < pair.second.size() - 1) cout << ", ";
                }
            }
            cout << "\n";
        }
    }

    // Проверка на циклы
    if (hasCycles(storage)) {
        cout << "\n ВНИМАНИЕ: Сеть содержит циклы!\n";
    }
    else {
        cout << "\n Сеть ациклична (не содержит циклов)\n";
    }
}

void GasNetwork::saveToStream(ostream& os) const {
    os << "CONNECTIONS\n";
    for (const auto& pair : connections) {
        os << pair.second.toSingleLine() << "\n";
    }
    os << "ENDCONNECTIONS\n";
}

bool GasNetwork::loadFromStream(istream& is) {
    map<int, Connection> newConnections;
    string line;
    bool inSection = false;
    int maxId = 0;

    while (getline(is, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line == "CONNECTIONS") {
            inSection = true;
            continue;
        }
        if (line == "ENDCONNECTIONS") {
            inSection = false;
            continue;
        }

        if (inSection) {
            Connection conn;
            if (Connection::fromSingleLine(line, conn)) {
                newConnections[conn.id] = conn;
                if (conn.id > maxId) maxId = conn.id;
            }
        }
    }

    if (!newConnections.empty()) {
        connections = newConnections;
        nextId = maxId + 1;
        return true;
    }
    return false;
}