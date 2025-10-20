#include <iostream>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "user.h"

using namespace std;

enum PrimaryPrompt { LOGIN, REGISTER, EXIT, MAIN_PROMPT };
enum subUserPrompt { USER_VERIFICATION, LOGIN_MENU };
enum featurePrompt { WHOAMI, LIST_CONTACT, ADD_CONTACT, SEEK_CONTACT, LOGOUT, LOGIN_MENU_PROMPT };

// --- Serialization helpers ---
void saveUsers(const vector<User> &users, const string &filename) {
    ofstream ofs(filename);
    boost::archive::text_oarchive oa(ofs);
    oa << users;
    cout << "Users saved to file.\n";
}

vector<User> loadUsers(const string &filename) {
    ifstream ifs(filename);
    if (!ifs) {
        cout << "No saved user file found, starting fresh.\n";
        return {};
    }
    boost::archive::text_iarchive ia(ifs);
    vector<User> users;
    ia >> users;
    cout << "Users loaded successfully.\n";
    return users;
}

void editPrompt(User &other) {
    cout << "Do you want to edit this contact? (y/n): ";
    char choice; cin >> choice;
    if (choice == 'n' || choice == 'N') return;
    string name, phone;
    cout << "Enter updated name: "; cin >> name;
    cout << "Enter updated phone: "; cin >> phone;
    other.setName(name);
    other.setPhone(phone);
}

int main() {
    const string serialname = "users_data.txt";
    vector<User> users = loadUsers(serialname);
    User user;

    PrimaryPrompt prompt = MAIN_PROMPT;
    subUserPrompt sub_login_prompt = USER_VERIFICATION;
    featurePrompt login_menu_prompt = LOGIN_MENU_PROMPT;

    while (true) {
        switch (prompt) {
            case MAIN_PROMPT: {
                cout << "\n1. Login\n2. Register\n3. Exit\nChoice: ";
                int choice; cin >> choice;
                prompt = static_cast<PrimaryPrompt>(choice - 1);
                break;
            }

            case LOGIN: {
                if (sub_login_prompt == USER_VERIFICATION) {
                    string name, phone;
                    cout << "Name: "; cin >> name;
                    cout << "Phone: "; cin >> phone;

                    bool found = false;
                    for (auto &u : users) {
                        if (u.getName() == name && u.getPhone() == phone) {
                            user = u;
                            user.setLoginState(1);
                            found = true;
                            cout << "Login successful!\n";
                            sub_login_prompt = LOGIN_MENU;
                            break;
                        }
                    }
                    if (!found) {
                        cout << "User not found.\n";
                        prompt = MAIN_PROMPT;
                    }
                }
                else if (sub_login_prompt == LOGIN_MENU) {
                    cout << "\n1. WHO AM I\n2. List contact\n3. Add contact\n4. Seek contact\n5. Logout\nChoice: ";
                    int choice; cin >> choice;
                    featurePrompt action = static_cast<featurePrompt>(choice - 1);

                    switch (action) {
                        case WHOAMI:
                            cout << "You are: " << user.getName() << " (" << user.getPhone() << ")\n";
                            break;
                        case LIST_CONTACT:
                            user.printContact();
                            break;
                        case ADD_CONTACT:
                            user.addContactPrompt(users);
                            break;
                        case SEEK_CONTACT: {
                            string phone;
                            cout << "Enter phone to search: ";
                            cin >> phone;
                            if (user.checkUserExist(phone)) {
                                User found = user.seekContact(phone);
                                cout << "Found " << found.getName() << " (" << found.getPhone() << ")\n";
                                editPrompt(found);
                            } else cout << "Contact not found.\n";
                            break;
                        }
                        case LOGOUT:
                            user.setLoginState(0);
                            // update the main list
                            for (auto &u : users)
                                if (u.getPhone() == user.getPhone())
                                    u = user;
                            saveUsers(users, serialname);
                            prompt = MAIN_PROMPT;
                            sub_login_prompt = USER_VERIFICATION;
                            cout << "Logged out.\n";
                            break;
                        default: break;
                    }
                }
                break;
            }

            case REGISTER: {
                string name, phone;
                cout << "Enter new name: "; cin >> name;
                cout << "Enter phone: "; cin >> phone;
                users.emplace_back(name, phone);
                saveUsers(users, serialname);
                cout << "Registration successful!\n";
                prompt = MAIN_PROMPT;
                break;
            }

            case EXIT:
                saveUsers(users, serialname);
                cout << "Goodbye!\n";
                return 0;
        }
    }
}
