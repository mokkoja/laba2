#pragma once

#include <string>
#include <map>

// Вспомогательные функции
std::string trim(const std::string& s);
bool parseInt(const std::string& s, int& out);
bool parseDouble(const std::string& s, double& out);
std::string currentTimestamp();

// Логгер
class Logger {
    std::string filename;
public:
    Logger();
    void setFile(const std::string& fname);
    void log(const std::string& entry);
};

extern Logger LOG;

// Помощник ввода
class InputHelper {
public:
    static int inputIntegerPositive(const std::string& prompt);
    static double inputDoublePositive(const std::string& prompt);
    static int inputZeroOne(const std::string& prompt);
    static int inputIntInRange(const std::string& prompt, int minv, int maxv);
    static std::string inputLineNonEmpty(const std::string& prompt);
    static int inputMenuChoice();
};

// Пакетные операции
class BatchProcessor {
public:
    static std::map<int, int> askIdsFromUserSelection(const std::map<int, int>& candidates);
};