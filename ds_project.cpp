#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

class Task {
    public:
    string name;
    string category;
    string dueDate;
    int priority;
    string notes;
    bool completed;
    Task* next;
};

class TaskManager {
private:
    Task* head;
    string password;

    void saveToFile() {
        ofstream file("tasks.txt");
        Task* current = head;
        while (current) {
            file << current->name << "\n"
                 << current->category << "\n"
                 << current->dueDate << "\n"
                 << current->priority << "\n"
                 << current->notes << "\n"
                 << current->completed << "\n";
            current = current->next;
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file("tasks.txt");
        string line;
        while (getline(file, line)) {
            Task* newTask = new Task;
            newTask->name = line;
            getline(file, newTask->category);
            getline(file, newTask->dueDate);
            file >> newTask->priority;
            file.ignore();
            getline(file, newTask->notes);
            file >> newTask->completed;
            file.ignore();
            newTask->next = head;
            head = newTask;
        }
        file.close();
    }

    bool isOverdue(const string& dueDate) {
        time_t t = time(nullptr);
        tm* currentDate = localtime(&t);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", currentDate);
        string today(buffer);
        return dueDate < today;
    }
  void savePassword() {
        ofstream file("password.txt");
        file << password;
        file.close();
    }

    void loadPassword() {
        ifstream file("password.txt");
        if (file.is_open()) {
            getline(file, password);
            file.close();
        } else {
            password = "1234"; // default password
            savePassword();
        }
    }

    void backupTasks() {
        ofstream backup("tasks_backup.txt");
        Task* current = head;
        while (current) {
            backup << current->name << "\n"
                   << current->category << "\n"
                   << current->dueDate << "\n"
                   << current->priority << "\n"
                   << current->notes << "\n"
                   << current->completed << "\n";
            current = current->next;
        }
        backup.close();
    }

    int countTasks() {
        int count = 0;
        Task* current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }

    int countTasksByPriority(int priority) {
        int count = 0;
        Task* current = head;
        while (current) {
            if (current->priority == priority) {
                count++;
            }
            current = current->next;
        }
        return count;
    }

    int countTasksByCategory(const string& category) {
        int count = 0;
        Task* current = head;
        while (current) {
            if (current->category == category) {
                count++;
            }
            current = current->next;
        }
        return count;
    }

public:
     TaskManager() : head(nullptr) {
        loadFromFile();
        loadPassword();
    }

    ~TaskManager() {
        saveToFile();
        backupTasks();  
        while (head) {
            Task* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void clearScreen() {
#ifdef _WIN32
        system("CLS");
#else
        system("clear");
#endif
    }

    bool verifyPassword() {
        string input;
        int attempts = 3;
        while (attempts > 0) {
            cout << "\nTo access, enter your password: ";
            cin >> input;
            if (input == password) {
                return true;
            }
            attempts--;
            cout << "\n\nIncorrect password.\nAttempts left: " << attempts << endl;
            if (attempts == 0) {
                return false;  
            }
        }    
        return false;
    }

    void setPassword() {
        clearScreen();
        cout << "\t\t__________________________\n\n";
        cout << "\t\t|    SET NEW PASSWORD    |\n";
        cout << "\t\t__________________________\n\n";
        string oldPassword;
        cout << "Enter old password: ";
        cin >> oldPassword;
        if (oldPassword != password) {
            cout << "Incorrect old password.\n";
            cout << "\n\nPress Enter to return to the main menu.";
            cin.ignore();
            cin.get();
            return;
        }
        cout << "Enter new password: ";
        cin >> password;
        savePassword();
        cout << "Password updated successfully!\n";
        cout << "\n\nPress Enter to return to the main menu.";
        cin.ignore();
        cin.get();
    }

    void addTask() {
        clearScreen();
        cout << "\t\t_____________________\n\n";
        cout << "\t\t|    ADD NEW TASK    |\n";
        cout << "\t\t_____________________\n\n";
        Task* newTask = new Task;
        cout << "\nEnter task name: ";
        cin.ignore();
        getline(cin, newTask->name);
        Task* current = head;
        while (current) {
            if (current->name == newTask->name) {
                cout << "Task already exists.\n";
                delete newTask;
                cout << "\n\nPress Enter to return to the main menu.";
                cin.ignore();
                cin.get();
                return;
            }
            current = current->next;
        }
        cout << "Enter category: ";
        getline(cin, newTask->category);
        cout << "Enter due date (YYYY-MM-DD): ";
        getline(cin, newTask->dueDate);
        cout << "Enter priority (1-5): ";
        cin >> newTask->priority;
        cin.ignore();
        cout << "Enter notes: ";
        getline(cin, newTask->notes);
        newTask->completed = false;
        newTask->next = head;
        head = newTask;
        cout << "\nTask added successfully!\n";
        cin.get();
    }

    void displayTasks() {
        clearScreen();
        cout << "\t\t_______________________\n\n";
        cout << "\t\t|    DISPLAY TASKS    |\n";
        cout << "\t\t_______________________\n\n";
        if (!head) {
            cout << "\nNo tasks available.\n";
        } else {
            Task* current = head;
            while (current) {
                cout << "\nName: " << current->name << endl;
                cout << "Category: " << current->category << endl;
                cout << "Due Date: " << current->dueDate << endl;
                cout << "Priority: " << current->priority << endl;
                cout << "Notes: " << current->notes << endl;
                cout << "Status: " << (current->completed ? "Completed" : "Pending") << endl;
                cout << "------------------------------------------------\n";
                current = current->next;
            }
        }
        cout << "\n\nPress Enter to return to the main menu.";
        cin.ignore();
        cin.get();
    }

    void markComplete() {
        clearScreen();
        cout << "\t\t________________________________\n\n";
        cout << "\t\t|    MARK TASK AS COMPLETED    |\n";
        cout << "\t\t________________________________\n\n";
        string name;
        cout << "\nEnter task name to mark as complete: ";
        cin.ignore();
        getline(cin, name);
        Task* current = head;
        while (current) {
            if (current->name == name) {
                current->completed = true;
                cout << "\nTask marked as complete.\n";
                cin.get();
                return;
            }
            current = current->next;
        }
        cout << "\nTask not found.\n";
        cin.get();
    }

    void deleteTask() {
        clearScreen();
        cout << "\t\t_____________________\n\n";
        cout << "\t\t|    DELETE TASK    |\n";
        cout << "\t\t_____________________\n\n";
        string name;
        cout << "\nEnter task name to delete: ";
        cin.ignore();
        getline(cin, name);
        if (head && head->name == name) {
            Task* temp = head;  
            head = head->next;   
            delete temp;         
            cout << "\nTask deleted successfully!\n";
            cout << "\n\nPress Enter to return to the main menu.";
            cin.ignore();
            cin.get();
            return;
        }
        Task* current = head;
        Task* prev = nullptr;
        while (current) {
            if (current->name == name) {
                prev->next = current->next; 
                delete current;             
                cout << "\nTask deleted successfully!\n";
                cout << "\n\nPress Enter to return to the main menu.";
                cin.ignore();
                cin.get();
                return;
            }
            prev = current;
            current = current->next;
        }
        cout << "\nTask not found.\n";
        cout << "\n\nPress Enter to return to the main menu.";
        cin.ignore();
        cin.get();
    }

    void displayOverdueTasks() {
        clearScreen();
        cout << "\t\t_______________________________\n\n";
        cout << "\t\t|    DISPLAY OVERDUE TASKS    |\n";
        cout << "\t\t_______________________________\n\n";
        Task* current = head;
        bool found = false;
        while (current) {
            if (isOverdue(current->dueDate)) {
                cout << "\nName: " << current->name << endl;
                cout << "Category: " << current->category << endl;
                cout << "Due Date: " << current->dueDate << endl;
                cout << "Priority: " << current->priority << endl;
                cout << "Notes: " << current->notes << endl;
                cout << "-----------------------------------------\n";
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "\nNo overdue tasks found.\n";
        }
        cout << "\n\nPress Enter to return to the main menu.";
        cin.ignore();
        cin.get();
    }

    void displayTasksByPriority() {
        clearScreen();
        cout << "\t\t___________________________________\n\n";
        cout << "\t\t|    DISPLAY TASKS BY PRIORITY    |\n";
        cout << "\t\t___________________________________\n\n";
        int priority;
        cout << "\nEnter priority level (1-5): ";
        cin >> priority;
        Task* current = head;
        bool found = false;
        while (current) {
            if (current->priority == priority) {
                cout << "\nName: " << current->name << endl;
                cout << "Category: " << current->category << endl;
                cout << "Due Date: " << current->dueDate << endl;
                cout << "Priority: " << current->priority << endl;
                cout << "Notes: " << current->notes << endl;
                cout << "-----------------------------------------\n";
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "\nNo tasks found for this priority level.\n";
        }
        cout << "\n\nPress Enter to return to the main menu.";
        cin.ignore();
        cin.get();
    }

    void displayTaskAnalysis() {
        clearScreen();
        cout << "\t\t_______________________________\n\n";
        cout << "\t\t|    DISPLAY TASK ANALYSIS    |\n";
        cout << "\t\t_______________________________\n\n";
        int taskCount = countTasks();
        cout << "\nTotal tasks: " << taskCount << endl << endl;
        for (int i = 1; i <= 5; ++i) {
            cout << "Tasks with priority " << i << ": " << countTasksByPriority(i) << endl;
        }
        cout << "\n\nPress Enter to return to the main menu.";
        cin.ignore();
        cin.get();
    }

    void clearCompletedTasks() {
        clearScreen();
        cout << "\t\t_______________________________\n\n";
        cout << "\t\t|    CLEAR COMPLETED TASKS    |\n";
        cout << "\t\t_______________________________\n\n";
        while (head && head->completed) {
            Task* temp = head;
            head = head->next;
            delete temp;
        }
        Task* current = head;
        Task* prev = nullptr;
        while (current) {
            if (current->completed) {
                prev->next = current->next;
                delete current;
                current = prev->next;
            } else {
                prev = current;
                current = current->next;
            }
        }

        cout << "\nCompleted tasks cleared.\n";
        cout << "\n\nPress Enter to return to the main menu.";
        cin.ignore();
        cin.get();
    }

};

int main() {
    cout << "\n\n\t\t=======================================================\n\n";
    cout << "\t\t|\tWELCOME TO TASK SCHEDULER AND MANAGER\t\t|\n\n";
    cout << "\t\t=======================================================\n\n";
    cout << "\t\t*  your everyday helper for better task organization  *\n\n";
    
    TaskManager manager;
    if (!manager.verifyPassword()) {
        return 0;
    }

    int choice;
    do {
        manager.clearScreen();
        cout << "\t\t==============================================\n\n";
        cout << "\t\t|    TASK SCHEDULER AND MANAGER DASHBOARD    |\n\n";
        cout << "\t\t==============================================\n\n";
        cout << "\t\t| 1. Add Task                                |\n";
        cout << "\t\t| 2. Display Tasks                           |\n";
        cout << "\t\t| 3. Mark Task as Complete                   |\n";             
        cout << "\t\t| 4. Delete Task                             |\n";
        cout << "\t\t| 5. Display Overdue Tasks                   |\n";
        cout << "\t\t| 6. Display Tasks by Priority               |\n";
        cout << "\t\t| 7. Display Task Analysis                   |\n";
        cout << "\t\t| 8. Clear Completed Tasks                   |\n";
        cout << "\t\t| 9. Change Password                         |\n";
        cout << "\t\t| 10. Exit                                   |\n\n";
        cout << "\t\t==============================================\n\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.addTask();
                break;
            case 2:
                manager.displayTasks();
                break;
            case 3:
                manager.markComplete();
                break;
            case 4:
                manager.deleteTask();
                break;
            case 5:
                manager.displayOverdueTasks();
                break;
            case 6:
                manager.displayTasksByPriority();
                break;
            case 7:
                manager.displayTaskAnalysis();
                break;
            case 8:
                manager.clearCompletedTasks();
                break;
            case 9:
                manager.setPassword();
                break;
            case 10:
                cout << "\nExiting... Press Enter to close the application.";
                cin.ignore();
                cin.get();
                break;
            default:
                cout << "\nInvalid choice. Try again. Press Enter to continue.";
                cin.ignore();
                cin.get();
        }
    } while (choice != 10);

    return 0;
}