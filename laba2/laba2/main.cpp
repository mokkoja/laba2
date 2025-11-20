#include "ui.h"
#include <iostream>
#include <locale.h>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    setlocale(LC_ALL, "Russian");

    Storage storage;
    UserInterface ui(storage);

    cout << "Программа управления трубами и КС \n";
    LOG.log("Program started.");

    while (true) {
        ui.printMenu();
        int choice = InputHelper::inputMenuChoice();

        if (choice == 0) {
            LOG.log("Program exited by user.");
            cout << "Выход.\n";
            break;
        }

        try {
            switch (choice) {
            case 1: addPipe(storage); break;
            case 2: addCS(storage); break;
            case 3: ui.listAllObjects(); LOG.log("Listed all objects."); break;
            case 4: viewPipeById(storage); break;     
            case 5: viewCSById(storage); break;       
            case 6: editPipeById(storage); break;
            case 7: editCSById(storage); break;
            case 8: removePipeById(storage); break;
            case 9: removeCSById(storage); break;
            case 10: searchPipes(storage); break;
            case 11: searchCS(storage); break;
            case 12: ui.searchAndBatchEditPipes(); break;
            case 13: saveData(storage); break;
            case 14: loadData(storage); break;
            case 15: setLogFile(); break;
            default: cout << "Неподдерживаемый пункт меню.\n"; break;
            }
        }
        catch (const exception& e) {
            cout << "Произошла ошибка: " << e.what() << "\n";
            LOG.log(string("Error: ") + e.what());
        }
    }

    return 0;
}