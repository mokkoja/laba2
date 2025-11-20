#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

class BaseEntity {
protected:
    int id;
    std::string name;
public:
    BaseEntity();
    virtual ~BaseEntity() = default;

    int getId() const;
    void setId(int newId);
    std::string getName() const;
    void setName(const std::string& newName);

    virtual std::string toSingleLine() const = 0;
    virtual void printDetails() const = 0;
    virtual void editInteractive() = 0;
};

class Pipe : public BaseEntity {
    double length; // ךל
    int diameter;    // לל
    bool inRepair;

public:
    Pipe();

    double getLength() const;
    int getDiameter() const;
    bool isInRepair() const;

    void setLength(double l);
    void setDiameter(int d);
    void setInRepair(bool repair);

    std::string toSingleLine() const override;
    static bool fromSingleLine(const std::string& line, Pipe& out);
    void printDetails() const override;
    void editInteractive() override;
};

class CS : public BaseEntity {
    int workshopsTotal;
    int workshopsWorking;
    std::string stationClass;
    double efficiency; // ןנמצוםע

public:
    CS();

    int getWorkshopsTotal() const;
    int getWorkshopsWorking() const;
    std::string getStationClass() const;
    double getEfficiency() const;
    double getIdlePercent() const;

    void setWorkshopsTotal(int total);
    void setWorkshopsWorking(int working);
    void setStationClass(const std::string& cls);

    std::string toSingleLine() const override;
    static bool fromSingleLine(const std::string& line, CS& out);
    void printDetails() const override;
    void editInteractive() override;

private:
    void updateEfficiency();
};