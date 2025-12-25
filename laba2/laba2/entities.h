#pragma once
#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

class Pipe {
    int id;
    std::string name;
    double length;
    int diameter;
    bool inRepair;

public:
    Pipe();
    Pipe(int id, std::string name, double length, int diameter, bool inRepair);

    int getId() const;
    void setId(int newId);
    std::string getName() const;
    void setName(const std::string& newName);
    double getLength() const;
    void setLength(double newLength);
    int getDiameter() const;
    void setDiameter(int newDiameter);
    bool isInRepair() const;
    void setInRepair(bool repair);

    std::string toSingleLine() const;
    static bool fromSingleLine(const std::string& line, Pipe& out);

    void printDetails() const;
    void editInteractive();
};

class CS {
    int id;
    std::string name;
    int workshopsTotal;
    int workshopsWorking;
    std::string stationClass;
    double efficiency;

public:
    CS();
    CS(int id, std::string name, int workshopsTotal, int workshopsWorking, std::string stationClass);

    int getId() const;
    void setId(int newId);
    std::string getName() const;
    void setName(const std::string& newName);
    int getWorkshopsTotal() const;
    void setWorkshopsTotal(int total);
    int getWorkshopsWorking() const;
    void setWorkshopsWorking(int working);
    double getIdlePercent() const;
    std::string getStationClass() const;
    void setStationClass(const std::string& cls);

    double getEfficiency() const;
    void setEfficiency(double eff);
    void updateEfficiency();

    std::string toSingleLine() const;
    static bool fromSingleLine(const std::string& line, CS& out);

    void printDetails() const;
    void editInteractive();
};

#endif