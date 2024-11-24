#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <conio.h>
#include <queue>
#include <set>

const std::string trojan = "mmt23clc007017236@gmail.com";
const std::string CAcert = "CAcertificate/cacert.pem";

class Gmail {
    private:
        std::string username;
        std::string password;
        static std::set<std::string> adminEmail;

        static void readAdminEmail();
        static bool isValidAdminEmail(const std::string &sender);
        static size_t headerCallback(char *buffer, size_t size, size_t nitems, std::string *userdata);
    public:
        Gmail(): username(""), password("") {if (adminEmail.empty()) readAdminEmail();}
        Gmail(const std::string &Username, const std::string &Password): username(Username), password(Password) {if (adminEmail.empty()) readAdminEmail();}

        std::queue<std::string> searchNewEmail();
        bool sendEmail(const std::string &receiver, const std::string &subject, const std::string &content);
        bool sendEmail(const std::string &receiver, const std::string &subject, const std::string &content, const std::string &filePath);
        bool receiveEmail(std::string &sender, std::string &subject, std::string &content, const std::string &emailNumber);
        void autoCheckEmail();
};