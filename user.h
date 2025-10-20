#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace std;

class User {
private:
    friend class boost::serialization::access;

    string name;
    string phone;
    int loginState;
    int id;
    static inline int sequenceCounter = 20;
    vector<User> contact;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & name;
        ar & phone;
        ar & loginState;
        ar & id;
        ar & contact;
    }

    bool checkExistingContact(const string &phone) const {
        for (auto &user : contact) {
            if (user.phone == phone) {
                cout << "Phone number already exists in your contact list!" << endl;
                return true;
            }
        }
        return false;
    }

public:
    User() : name(""), phone(""), loginState(0), id(sequenceCounter++) {}

    User(string name, string phone)
        : name(name), phone(phone), loginState(0), id(sequenceCounter++) {}

    // --- Contact management ---
    bool addContact(const User &user) {
        if (checkExistingContact(user.phone)) return false;
        contact.push_back(user);
        cout << "Contact " << user.name << " added successfully!" << endl;
        return true;
    }

    void addContactPrompt(const vector<User> &registeredUsers) {
        string phone;
        cout << "Enter the phone number of registered user to add as contact: ";
        cin >> phone;

        // Verify contact exists among registered users
        for (const auto &regUser : registeredUsers) {
            if (regUser.phone == phone) {
                if (checkExistingContact(phone)) return;
                contact.push_back(regUser);
                cout << "Added registered user " << regUser.name << " to your contacts." << endl;
                return;
            }
        }
        cout << "This phone number is not a registered user!" << endl;
    }

    void printContact() const {
        cout << "\nContact List for " << name << ":\n";
        if (contact.empty()) {
            cout << "No contacts yet.\n";
            return;
        }
        for (size_t i = 0; i < contact.size(); ++i) {
            cout << i + 1 << ". " << contact[i].name << " (" << contact[i].phone << ")\n";
        }
    }

    bool checkUserExist(const string &phone) const {
        for (auto &user : contact)
            if (user.phone == phone) return true;
        return false;
    }

    User seekContact(const string &phone) const {
        for (auto &user : contact)
            if (user.phone == phone) return user;
        return User();
    }

    void updateContact(int id, const User &other) {
        for (auto &user : contact) {
            if (user.id == id) {
                user = other;
                cout << "Contact " << id << " updated successfully." << endl;
                return;
            }
        }
        cout << "Contact not found." << endl;
    }

    // --- Getters & setters ---
    string getName() const { return name; }
    string getPhone() const { return phone; }
    void setName(string n) { name = n; }
    void setPhone(string p) { phone = p; }
    int getLoginState() const { return loginState; }
    void setLoginState(int s) { loginState = s; }
    int getId() const { return id; }
    static void resetId() { sequenceCounter = 20; }
};
