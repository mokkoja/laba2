#pragma once
#ifndef NETWORK_H
#define NETWORK_H

#include "entities.h"
#include <map>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <sstream>

// Предварительное объявление
class Storage;
class Pipe;

class Connection {
public:
    int id;
    int pipeId;      // ID трубы
    int csInId;      // ID КС входа
    int csOutId;     // ID КС выхода
    bool isActive;

    Connection();
    Connection(int id, int pipeId, int csInId, int csOutId);

    std::string toSingleLine() const;
    static bool fromSingleLine(const std::string& line, Connection& out);
    void printDetails() const;

    // Геттеры
    int getId() const { return id; }
    int getPipeId() const { return pipeId; }
    int getCsInId() const { return csInId; }
    int getCsOutId() const { return csOutId; }
    bool getIsActive() const { return isActive; }
};

class GasNetwork {
    std::map<int, Connection> connections;
    int nextId;

public:
    GasNetwork();

    // Основные операции
    int addConnection(const Connection& conn);
    bool removeConnection(int id);
    std::map<int, Connection> getAllConnections() const;
    Connection* findConnectionById(int id);

    // Поиск и создание соединений
    std::map<int, Pipe*> findAvailablePipesByDiameter(int diameter, Storage& storage);
    bool createConnectionInteractive(Storage& storage);

    // Работа с графом
    std::vector<int> topologicalSort(Storage& storage) const;
    bool hasCycles(Storage& storage) const;
    void printNetworkGraph(Storage& storage) const;

    // Статистика
    int getConnectionCount() const { return (int)connections.size(); }

    // Сохранение/загрузка
    void saveToStream(std::ostream& os) const;
    bool loadFromStream(std::istream& is);

private:
    bool isValidDiameter(int diameter) const;
    int getNextId();
};

#endif // NETWORK_H