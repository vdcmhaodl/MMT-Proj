#pragma once

#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <conio.h>
#include <queue>
#include <set>

const std::string trojan = "mmt23clc007017236@gmail.com";
const std::string CAcert = "CAcertificate/cacert.pem";

struct Email {
    std::string account;
    std::string subject;
    std::string content;
    std::string messageID;
        
    Email(): account(""), subject(""), content(""), messageID("") {}
};

class GmailAccount {
    private:
        std::string username;
        std::string password;
        static std::set<std::string> adminEmail;

        static void readAdminEmail();
        static bool isValidAdminEmail(const std::string &sender);
        static size_t headerCallback(char *buffer, size_t size, size_t nitems, std::string *userdata);
    public:
        GmailAccount(): username(""), password("") {if (adminEmail.empty()) readAdminEmail();}
        GmailAccount(const std::string &Username, const std::string &Password): username(Username), password(Password) {if (adminEmail.empty()) readAdminEmail();}

        std::queue<std::string> searchNewEmail();
        bool repEmail(const Email &email, const std::string &filePath = "");
        bool sendEmail(const Email &email, const std::string &filePath = "");
        bool receiveEmail(Email &email, const std::string &emailNumber);
        // void autoCheckEmail();
};