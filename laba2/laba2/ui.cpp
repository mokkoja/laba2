#include "ui.h"
#include <iostream>
#include <exception>
#include <iomanip>
#include "network.h"

using namespace std;

UserInterface::UserInterface(Storage& st) : storage(st) {}

void UserInterface::printMenu() {
    cout << "\n=== МЕНЮ УПРАВЛЕНИЯ ===\n"
        << "1. Добавить трубу\n"
        << "2. Добавить КС\n"
        << "3. Просмотреть все объекты\n"
        << "4. Просмотреть трубу по ID\n"
        << "5. Просмотреть КС по ID\n"
        << "6. Редактировать трубу по ID\n"
        << "7. Редактировать КС по ID\n"
        << "8. Удалить трубу по ID\n"
        << "9. Удалить КС по ID\n"
        << "10. Поиск труб (фильтр)\n"
        << "11. Поиск КС (фильтр)\n"
        << "12. Пакетное редактирование труб\n"
        << "13. Сохранить данные в файл\n"
        << "14. Загрузить данные из файла\n"
        << "15. Задать файл логов\n"
        << "\n--- ГАЗОТРАНСПОРТНАЯ СЕТЬ ---\n"
        << "16. Создать соединение\n"
        << "17. Просмотреть все соединения\n"
        << "18. Удалить соединение\n"
        << "19. Топологическая сортировка\n"
        << "20. Просмотреть граф сети\n"
        << "0. Выход\n"
        << "Ваш выбор: ";
}

void UserInterface::listAllObjects() {
    cout << "\n-- Трубы: --\n";
    map<int, Pipe> pipes = storage.getAllPipes();
    if (pipes.empty()) cout << "<нет труб>\n";
    for (const auto& pair : pipes) {
        const Pipe& pipe = pair.second;
        cout << "ID=" << pipe.getId() << " | " << pipe.getName() << " | "
            << pipe.getLength() << " км | " << pipe.getDiameter() << " мм | "
            << (pipe.isInRepair() ? "В ремонте" : "Работает") << "\n";
    }

    cout << "\n-- КС: --\n";
    map<int, CS> css = storage.getAllCS();
    if (css.empty()) cout << "<нет КС>\n";
    for (const auto& pair : css) {
        const CS& cs = pair.second;
        cout << "ID=" << cs.getId() << " | " << cs.getName() << " | класс "
            << cs.getStationClass() << " | " << cs.getWorkshopsWorking() << "/"
            << cs.getWorkshopsTotal() << " | эффективность " << fixed << setprecision(2)
            << cs.getEfficiency() << "% | незадействовано " << fixed << setprecision(2)
            << cs.getIdlePercent() << "%\n";
    }
}

void UserInterface::run() {
    string choice;
    while (true) {
        printMenu();
        getline(cin, choice);
        choice = trim(choice);

        try {
            if (choice == "1") addPipe(storage);
            else if (choice == "2") addCS(storage);
            else if (choice == "3") listAllObjects();
            else if (choice == "4") viewPipeById(storage);
            else if (choice == "5") viewCSById(storage);
            else if (choice == "6") editPipeById(storage);
            else if (choice == "7") editCSById(storage);
            else if (choice == "8") removePipeById(storage);
            else if (choice == "9") removeCSById(storage);
            else if (choice == "10") searchPipes(storage);
            else if (choice == "11") searchCS(storage);
            else if (choice == "12") searchAndBatchEditPipes();
            else if (choice == "13") saveData(storage);
            else if (choice == "14") loadData(storage);
            else if (choice == "15") setLogFile();
            else if (choice == "16") createConnection(storage);
            else if (choice == "17") listConnections(storage);
            else if (choice == "18") removeConnection(storage);
            else if (choice == "19") topologicalSort(storage);
            else if (choice == "20") printNetwork(storage);
            else if (choice == "0") break;
            else cout << "Неверный выбор.\n";
        }
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << "\n";
            LOG.log(string("Exception: ") + e.what());
        }
    }
}

void UserInterface::searchAndBatchEditPipes() {
    cout << "Пакетное редактирование труб: сначала зададим фильтр.\n";

    cout << "Фильтр по имени (подстрока, пустая строка - нет фильтра): ";
    string name;
    getline(cin, name);
    name = trim(name);

    int repairFilter = -1;
    string repairInput = InputHelper::inputLineNonEmpty(
        "Фильтр по состоянию ремонта? (1 - только в ремонте, 0 - только не в ремонте, пустая строка - нет фильтра): ");
    if (!repairInput.empty()) {
        if (parseInt(repairInput, repairFilter) && (repairFilter == 0 || repairFilter == 1)) {
            // OK
        }
        else {
            cout << "Игнорирую фильтр 'в ремонте'.\n";
            repairFilter = -1;
        }
    }

    map<int, Pipe*> found = storage.searchPipes(name, repairFilter);
    if (found.empty()) {
        cout << "Ничего не найдено по заданным фильтрам.\n";
        LOG.log("Batch edit pipes: search returned 0 results.");
        return;
    }

    map<int, int> foundIds;
    cout << "Найденные трубы:\n";
    for (const auto& pair : found) {
        Pipe* pipe = pair.second;
        pipe->printDetails();
        foundIds[pair.first] = pair.first;
    }

    map<int, int> chosenIds = BatchProcessor::askIdsFromUserSelection(foundIds);

    if (chosenIds.empty()) {
        cout << "Отмена пакетного редактирования.\n";
        LOG.log("Batch edit pipes: user cancelled selection.");
        return;
    }

    processBatchOperation(chosenIds);
}

void UserInterface::processBatchOperation(const map<int, int>& chosenIds) {
    cout << "Доступные операции:\n1) Редактировать выбранные\n2) Удалить выбранные\n0) Отмена\nВаш выбор: ";
    string op;
    getline(cin, op);
    op = trim(op);

    if (op == "1") {
        for (const auto& pair : chosenIds) {
            int id = pair.first;
            Pipe* pipe = storage.findPipeById(id);
            if (pipe) {
                cout << "\nРедактирование трубы ID=" << id << ":\n";
                pipe->editInteractive();
                LOG.log(string("Batch edited pipe ID=") + to_string(id));
            }
        }
        cout << "Пакетное редактирование завершено.\n";
        LOG.log(string("Batch edit pipes: completed edits for ") + to_string(chosenIds.size()) + " items.");
    }
    else if (op == "2") {
        for (const auto& pair : chosenIds) {
            int id = pair.first;
            if (storage.removePipeById(id)) {
                LOG.log(string("Batch removed pipe ID=") + to_string(id));
            }
        }
        cout << "Удаление выбранных труб выполнено.\n";
        LOG.log(string("Batch edit pipes: removed ") + to_string(chosenIds.size()) + " items.");
    }
    else {
        cout << "Отмена операции.\n";
        LOG.log("Batch edit pipes: user cancelled operation choice.");
    }
}

// Реализации свободных функций

void addPipe(Storage& storage) {
    int id = storage.getNextPipeId();
    Pipe p = storage.createPipeInteractive(id);
    storage.addPipe(p);
    cout << "Труба добавлена с ID=" << p.getId() << "\n";
    LOG.log(string("Added pipe ID=") + to_string(p.getId()) + " name=\"" + p.getName() + "\"");
}

void addCS(Storage& storage) {
    int id = storage.getNextCSId();
    CS s = storage.createCSInteractive(id);
    storage.addCS(s);
    cout << "КС добавлена с ID=" << s.getId() << "\n";
    LOG.log(string("Added CS ID=") + to_string(s.getId()) + " name=\"" + s.getName() + "\"");
}

void viewPipeById(Storage& storage) {
    int id = InputHelper::inputIntegerPositive("Введите ID трубы: ");
    Pipe* pipe = storage.findPipeById(id);
    if (pipe) {
        pipe->printDetails();
        LOG.log(string("Viewed pipe ID=") + to_string(id));
    }
    else {
        cout << "Труба с ID=" << id << " не найдена.\n";
        LOG.log(string("View pipe: ID not found: ") + to_string(id));
    }
}

void viewCSById(Storage& storage) {
    int id = InputHelper::inputIntegerPositive("Введите ID КС: ");
    CS* cs = storage.findCSById(id);
    if (cs) {
        cs->printDetails();
        LOG.log(string("Viewed CS ID=") + to_string(id));
    }
    else {
        cout << "КС с ID=" << id << " не найдена.\n";
        LOG.log(string("View CS: ID not found: ") + to_string(id));
    }
}

void viewObjectById(Storage& storage) {
    int id = InputHelper::inputIntegerPositive("Введите ID объекта: ");
    Pipe* pipe = storage.findPipeById(id);
    if (pipe) {
        pipe->printDetails();
        LOG.log(string("Viewed pipe ID=") + to_string(id));
        return;
    }

    CS* cs = storage.findCSById(id);
    if (cs) {
        cs->printDetails();
        LOG.log(string("Viewed CS ID=") + to_string(id));
        return;
    }

    cout << "Объект с таким ID не найден.\n";
    LOG.log(string("View object: ID not found: ") + to_string(id));
}

void editPipeById(Storage& storage) {
    int id = InputHelper::inputIntegerPositive("Введите ID трубы для редактирования: ");
    Pipe* pipe = storage.findPipeById(id);
    if (!pipe) {
        cout << "Труба не найдена.\n";
        LOG.log(string("Edit pipe failed - not found ID=") + to_string(id));
        return;
    }
    pipe->editInteractive();
    LOG.log(string("Edited pipe ID=") + to_string(id));
}

void editCSById(Storage& storage) {
    int id = InputHelper::inputIntegerPositive("Введите ID КС для редактирования: ");
    CS* cs = storage.findCSById(id);
    if (!cs) {
        cout << "КС не найдена.\n";
        LOG.log(string("Edit CS failed - not found ID=") + to_string(id));
        return;
    }
    cs->editInteractive();
    LOG.log(string("Edited CS ID=") + to_string(id));
}

void removePipeById(Storage& storage) {
    int id = InputHelper::inputIntegerPositive("Введите ID трубы для удаления: ");
    if (storage.removePipeById(id)) {
        cout << "Труба удалена.\n";
        LOG.log(string("Removed pipe ID=") + to_string(id));
    }
    else {
        cout << "Труба с таким ID не найдена.\n";
        LOG.log(string("Remove pipe failed - not found ID=") + to_string(id));
    }
}

void removeCSById(Storage& storage) {
    int id = InputHelper::inputIntegerPositive("Введите ID КС для удаления: ");
    if (storage.removeCSById(id)) {
        cout << "КС удалена.\n";
        LOG.log(string("Removed CS ID=") + to_string(id));
    }
    else {
        cout << "КС с таким ID не найдена.\n";
        LOG.log(string("Remove CS failed - not found ID=") + to_string(id));
    }
}

void searchPipes(Storage& storage) {
    cout << "Поиск труб. Введите фильтры.\n";
    cout << "По имени (подстрока, пустая строка - нет фильтра): ";
    string name;
    getline(cin, name);
    name = trim(name);

    int repairFilter = -1;
    string repairInput = InputHelper::inputLineNonEmpty(
        "Только в ремонте? (1 - только в ремонте, 0 - только не в ремонте, пустая строка - нет фильтра): ");
    if (!repairInput.empty()) {
        if (!parseInt(repairInput, repairFilter) || (repairFilter != 0 && repairFilter != 1)) {
            repairFilter = -1;
        }
    }

    map<int, Pipe*> result = storage.searchPipes(name, repairFilter);
    cout << "Найдено труб: " << result.size() << "\n";
    for (const auto& pair : result) {
        Pipe* pipe = pair.second;
        pipe->printDetails();
    }
    LOG.log(string("Searched pipes: found ") + to_string(result.size()));
}

void searchCS(Storage& storage) {
    cout << "Поиск КС. Введите фильтры.\n";
    cout << "По имени (подстрока, пустая строка - нет фильтра): ";
    string name;
    getline(cin, name);
    name = trim(name);

    double minIdlePercent = -1.0;
    string percentInput = InputHelper::inputLineNonEmpty(
        "Минимальный % незадействованных цехов (пустая строка - нет фильтра): ");
    if (!percentInput.empty()) {
        if (!parseDouble(percentInput, minIdlePercent) || minIdlePercent < 0) {
            minIdlePercent = -1.0;
            cout << "Игнорирую неверный процент.\n";
        }
    }

    map<int, CS*> result = storage.searchCS(name, minIdlePercent);
    cout << "Найдено КС: " << result.size() << "\n";
    for (const auto& pair : result) {
        CS* cs = pair.second;
        cs->printDetails();
    }
    LOG.log(string("Searched CS: found ") + to_string(result.size()));
}

void saveData(Storage& storage) {
    string filename = InputHelper::inputLineNonEmpty("Введите имя файла для сохранения (например data.txt): ");
    if (storage.saveToFile(filename)) {
        cout << "Сохранено в файл " << filename << "\n";
        LOG.log(string("Saved data to file \"") + filename + "\"");
    }
    else {
        cout << "Ошибка записи в файл " << filename << "\n";
        LOG.log(string("Failed to save data to file \"") + filename + "\"");
    }
}

void loadData(Storage& storage) {
    string filename = InputHelper::inputLineNonEmpty("Введите имя файла для загрузки (например data.txt): ");
    if (storage.loadFromFile(filename)) {
        cout << "Данные загружены из " << filename << "\n";
        LOG.log(string("Loaded data from file \"") + filename + "\"");
    }
    else {
        cout << "Ошибка чтения файла " << filename << "\n";
        LOG.log(string("Failed to load data from file \"") + filename + "\"");
    }
}

void setLogFile() {
    string filename = InputHelper::inputLineNonEmpty("Введите имя файла для логирования (например mylog.txt): ");
    LOG.setFile(filename);
    LOG.log(string("Log file changed to ") + filename);
    cout << "Файл логов изменён на: " << filename << "\n";
}

// Сетевые функции
void createConnection(Storage& storage) {
    if (storage.createConnection()) {
        cout << "Соединение успешно создано!\n";
    }
    else {
        cout << "Не удалось создать соединение.\n";
    }
}

void listConnections(Storage& storage) {
    storage.listAllConnections();
}

void removeConnection(Storage& storage) {
    int id = InputHelper::inputIntegerPositive("Введите ID соединения для удаления: ");
    storage.removeConnection(id);
}

void topologicalSort(Storage& storage) {
    storage.performTopologicalSort();
}

void printNetwork(Storage& storage) {
    storage.printNetwork();
}