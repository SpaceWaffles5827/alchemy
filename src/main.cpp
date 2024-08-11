#include <iostream>
#include <string>
#include <alchemy/server.h>
#include <alchemy/global.h>

void displayMenu() {
    std::cout << "Welcome to the Game!\n";
    std::cout << "Please select an option:\n";
    std::cout << "1. Start Game\n";
    std::cout << "2. Start Server\n";
    std::cout << "3. Start Level Editor\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    while (true) {
        displayMenu();

        std::string choice;
        std::cin >> choice;

        if (choice == "1") {
            game.init();
            game.run();
            break;
        }
        else if (choice == "2") {
            std::cout << "Starting Server...\n";
            Server server;
            server.run();
            break;
        }
        else if (choice == "3") {
            std::cout << "Starting Level Editor...\n";
            game.init(); 
            game.run();
            break;
        }
        else if (choice == "4") {
            std::cout << "Exiting...\n";
            break;
        }
        else {
            std::cout << "Invalid choice, please try again.\n";
        }
    }

    return 0;
}
