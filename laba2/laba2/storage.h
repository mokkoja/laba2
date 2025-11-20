#pragma once
#ifndef STORAGE_H
#define STORAGE_H

#include "entities.h"
#include <map>
#include <string>
#include <fstream>
#include <algorithm>

class IdGenerator {
    int nextId;
public:
    IdGenerator();
    int next();
    void setNext(int v);
};

template<typename T>
class EntityManager {
    std::map<int, T> entities;
    IdGenerator idGen;

public:
    int add(const T& entity);
    T* findById(int id);
    bool removeById(int id);
    std::map<int, T> getAll() const;
    std::map<int, T*> findByName(const std::string& nameSubstr);
    int getNextId();
    void updateIdGeneratorFromData();
    void saveToStream(std::ostream& os, const std::string& header) const;
    bool loadFromStream(std::istream& is, const std::string& header);
};

class Storage {
    EntityManager<Pipe> pipeManager;
    EntityManager<CS> csManager;

public:
    // Методы для работы с трубами
    int addPipe(const Pipe& p);
    Pipe* findPipeById(int id);
    bool removePipeById(int id);
    std::map<int, Pipe> getAllPipes() const;
    std::map<int, Pipe*> findPipesByName(const std::string& name);
    int getNextPipeId();

    // Методы для работы с КС
    int addCS(const CS& s);
    CS* findCSById(int id);
    bool removeCSById(int id);
    std::map<int, CS> getAllCS() const;
    std::map<int, CS*> findCSByName(const std::string& name);
    int getNextCSId();

    // Поиск с фильтрами
    std::map<int, Pipe*> searchPipes(const std::string& nameSubstr, int inRepairFlag);
    std::map<int, CS*> searchCS(const std::string& nameSubstr, double minPercentIdle);

    // ДОБАВЛЕННЫЕ МЕТОДЫ
    std::map<int, Pipe> getAllPipe3();
    std::map<int, CS> getAUC5();
    std::map<int, Pipe*> searchPipe3(const std::string& nameSubstr, int inRepairFlag);
    std::map<int, CS*> searchC5(const std::string& nameSubstr, double minPercentIdle);

    // Сохранение и загрузка
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);

    // Создание объектов через интерактивный ввод
    Pipe createPipeInteractive(int id);
    CS createCSInteractive(int id);
};

#endif