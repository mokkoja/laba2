#pragma once

#include "storage.h"
#include "utils.h"
#include <map>

class UserInterface {
    Storage& storage;

public:
    UserInterface(Storage& st);
    void printMenu();
    void listAllObjects();
    void searchAndBatchEditPipes();
    void run();
    void processBatchOperation(const std::map<int, int>& chosenIds);
};

void addPipe(Storage& storage);
void addCS(Storage& storage);
void viewObjectById(Storage& storage);
void viewPipeById(Storage& storage);
void viewCSById(Storage& storage);
void editPipeById(Storage& storage);
void editCSById(Storage& storage);
void removePipeById(Storage& storage);
void removeCSById(Storage& storage);
void searchPipes(Storage& storage);
void searchCS(Storage& storage);
void saveData(Storage& storage);
void loadData(Storage& storage);
void setLogFile();