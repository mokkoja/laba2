#include "ui.h"
#include <iostream>
#include <locale.h>
#include "network.h"

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    setlocale(LC_ALL, "Russian");

    Storage storage;
    UserInterface ui(storage);

    cout << "Программа управления трубами и КС \n";
    LOG.log("Program started.");

    ui.run();  // ← Запускаем интерфейс

    return 0;
}