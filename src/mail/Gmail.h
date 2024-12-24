#pragma once

#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <set>
#include <syncstream>

const std::string client = "mmt23clc007017236@gmail.com";
const std::string CAcert = "CAcertificate/cacert.pem";

struct Email {
    std::string account;
    std::string subject;
    std::string messageID;
        
    Email() {}
};

struct Mail {
    Email sender;
    std::string content;
};

class GmailAccount {
    private:
        std::string username;
        std::string password;
        static std::set<std::string> adminEmails;

        static void readAdminEmail();
        static bool isValidAdminEmail(const std::string &sender);
        static size_t headerCallback(char *buffer, size_t size, size_t nitems, std::string *userdata);
    public:
        // static bool addAdminEmail(const std::string &email);
        // static bool removeAdminEmail(const std::string &email);

        GmailAccount(std::string Username = "", std::string Password = "");
        void initializeInfo(std::string &Username, std::string &Password);

        std::queue<std::string> searchNewEmail();
        std::queue<Mail> getEmailQueue();
        bool repEmail(const Email &email, const std::string &content, const std::string &filePath = "");
        bool sendEmail(const Email &email, const std::string &content, const std::string &filePath = "");
        bool receiveEmail(Email &email, std::string &content, const std::string &emailNumber);
};