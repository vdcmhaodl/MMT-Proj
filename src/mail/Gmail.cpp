#include "Gmail.h"

const std::string seperateLine = "#*^";

std::set<std::string> GmailAccount::adminEmails;

size_t GmailAccount::headerCallback(char* buffer, size_t size, size_t nitems, std::string* userdata) {
    userdata -> append(std::string(buffer, size * nitems));
    userdata -> append(seperateLine);
    return size * nitems;
}

void GmailAccount::readAdminEmail() {
    std::ifstream fin ("AdminAccount.txt");
    if (!fin.is_open()) {
        std::osyncstream(std::cerr) << "Fail to read Admin's Email!\n";
        return;
    }

    std::string s;
    while (getline(fin, s)) {
        adminEmails.insert(s);
    }

    fin.close();
}

bool GmailAccount::isValidAdminEmail(const std::string &sender) {
    return (adminEmails.find(sender) != adminEmails.end());
}

GmailAccount::GmailAccount(std::string Username, std::string Password) {
    initializeInfo(Username, Password);
}

void GmailAccount::initializeInfo(std::string &Username, std::string &Password) {
    this->username = Username;
    this->password = Password;
    if (adminEmails.empty()) 
        readAdminEmail();
}

std::queue<std::string> GmailAccount::searchNewEmail() {
    CURL* handle;
    CURLcode res = CURLE_OK;
    std::queue<std::string> qEmailNums;
    std::string searchResult;

    handle = curl_easy_init();
    if (handle) {
        curl_easy_setopt(handle, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(handle, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, "imaps://imap.gmail.com:993/INBOX");
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(handle, CURLOPT_CAINFO, CAcert.c_str());
        // curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "SEARCH UNSEEN");
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &searchResult);
        
        res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::osyncstream(std::cerr) << "Error: " << curl_easy_strerror(res) << "\n";
            return qEmailNums;
        }
        // get email numbers
        size_t pos1 = 0, pos2 = 0;
        pos1 = searchResult.find("EXISTS", pos1);
        pos1 = searchResult.find("SEARCH", pos1);
        pos1 = searchResult.find(" ", pos1) + 1;
        pos2 = searchResult.find(seperateLine, pos1);
        searchResult = searchResult.substr(pos1, pos2 - pos1);
        pos1 = searchResult.find_last_not_of(" \t\f\v\n\r");
        if (pos1 != std::string::npos)
            searchResult.erase(pos1 + 1);
        // if no new email
        if (searchResult == "OK SEARCH completed (Success)") {
            std::osyncstream(std::cerr) << "No new Email!\n";
            return qEmailNums;
        }

        pos1 = pos2 = 0;
        while (true) {
            pos2 = searchResult.find(' ', pos2);
            if (pos2 == std::string::npos) {
                pos2 = searchResult.length();
                qEmailNums.push(searchResult.substr(pos1, pos2 - pos1));
                break;
            }
            qEmailNums.push(searchResult.substr(pos1, pos2 - pos1));
            pos1 = ++pos2;
        }

        curl_easy_cleanup(handle);
    }
    return qEmailNums;
}

std::queue<Mail> GmailAccount::getEmailQueue() {
    std::queue<std::string> listEmailNumber = searchNewEmail();
    std::queue<Mail> listMail;

    while(!listEmailNumber.empty()) {
        std::string emailNumber = listEmailNumber.front();
        listEmailNumber.pop();

        Email email;
        std::string content;
        if(receiveEmail(email, content, emailNumber)) {
            listMail.push(Mail{email, content});
        }
    }
    return listMail;
}

bool GmailAccount::repEmail(const Email &email, const std::string &content, const std::string &filePath) {
    CURL* handle;
    CURLcode res = CURLE_OK;
    struct curl_slist* listRecipients = nullptr;
    struct curl_slist* headers = nullptr;
    curl_mime* mime;
    curl_mimepart* part;

    std::string header[] = {
        "To: " + email.account,
        "from: " + username,
        "Subject: Re: " + email.subject,
        "In-Reply-To: " + email.messageID,
        "References: " + email.messageID,
        ""
    };

    handle = curl_easy_init();
    if (handle) {
        // add information of email account (sender)
        curl_easy_setopt(handle, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(handle, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(handle, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

        curl_easy_setopt(handle, CURLOPT_MAIL_FROM, ("<" + username + ">").c_str());
        curl_easy_setopt(handle, CURLOPT_CAINFO, CAcert.c_str());
        // curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

        // add information of email (receivers)
        listRecipients = curl_slist_append(listRecipients, ("<" + email.account + ">").c_str());
        curl_easy_setopt(handle, CURLOPT_MAIL_RCPT, listRecipients);

        // add headers
        for (std::string* str = header; *str != ""; ++str)
            headers = curl_slist_append(headers, (*str).c_str());
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

        mime = curl_mime_init(handle);

        // add contents
        part = curl_mime_addpart(mime);
        curl_mime_data(part, content.c_str(), CURL_ZERO_TERMINATED);

        // send file
        if (filePath != "") {
            std::ifstream file(filePath.c_str(), std::ios::binary);
            if (!file.is_open()) {
                std::osyncstream(std::cerr) << "File Error!\n";
                curl_mime_free(mime);
                curl_slist_free_all(headers);
                curl_slist_free_all(listRecipients);
                curl_easy_cleanup(handle); 
                return false;
            }
            file.close();
            part = curl_mime_addpart(mime);
            curl_mime_filedata(part, filePath.c_str());
            curl_mime_encoder(part, "base64");
        }

        // send data
        curl_easy_setopt(handle, CURLOPT_MIMEPOST, mime);

        res = curl_easy_perform(handle);
        if (res != CURLE_OK)
            std::osyncstream(std::cerr) << "ERROR!!!\n" << curl_easy_strerror(res) << "\n";

        // clean memory
        curl_mime_free(mime);
        curl_slist_free_all(headers);
        curl_slist_free_all(listRecipients);
        curl_easy_cleanup(handle); 
    }
    return (res == CURLE_OK);
}

bool GmailAccount::sendEmail(const Email& email, const std::string &content, const std::string &filePath) {
    CURL* handle;
    CURLcode res = CURLE_OK;
    struct curl_slist* listRecipients = nullptr;
    struct curl_slist* headers = nullptr;
    curl_mime* mime;
    curl_mimepart* part;

    std::string header[] = {
        "To: " + email.account,
        "From: " + username,
        "Subject: " + email.subject,
        ""
    };

    handle = curl_easy_init();
    if (handle) {
        // add information of email account (sender)
        curl_easy_setopt(handle, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(handle, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(handle, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

        curl_easy_setopt(handle, CURLOPT_MAIL_FROM, ("<" + username + ">").c_str());
        curl_easy_setopt(handle, CURLOPT_CAINFO, CAcert.c_str());
        // curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

        // add information of email (receivers)
        listRecipients = curl_slist_append(listRecipients, ("<" + email.account + ">").c_str());
        curl_easy_setopt(handle, CURLOPT_MAIL_RCPT, listRecipients);

        // add headers
        for (std::string* str = header; *str != ""; ++str)
            headers = curl_slist_append(headers, (*str).c_str());
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

        mime = curl_mime_init(handle);

        // add contents
        part = curl_mime_addpart(mime);
        curl_mime_data(part, content.c_str(), CURL_ZERO_TERMINATED);

        // upload file
        if (filePath != "") {
            std::ifstream file(filePath.c_str(), std::ios::binary);
            if (!file.is_open()) {
                std::osyncstream(std::cerr) << "File Error!\n";
                curl_mime_free(mime);
                curl_slist_free_all(headers);
                curl_slist_free_all(listRecipients);
                curl_easy_cleanup(handle); 
                return false;
            }
            file.close();
            part = curl_mime_addpart(mime);
            curl_mime_filedata(part, filePath.c_str());
            curl_mime_encoder(part, "base64");
        }
        // send data
        curl_easy_setopt(handle, CURLOPT_MIMEPOST, mime);

        res = curl_easy_perform(handle);
        if (res != CURLE_OK)
            std::osyncstream(std::cerr) << "ERROR!!!\n" << curl_easy_strerror(res) << "\n";

        // clean memory
        curl_slist_free_all(listRecipients);
        curl_slist_free_all(headers);
        curl_mime_free(mime);
        curl_easy_cleanup(handle); 
    }
    return (res == CURLE_OK);
}

bool GmailAccount::receiveEmail(Email &email, std::string &content, const std::string &emailNumber) {
    CURL* handle;
    CURLcode res = CURLE_OK;
    std::string command;

    email.account = email.subject = content = email.messageID = "";

    handle = curl_easy_init();
    if(handle) {
        curl_easy_setopt(handle, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(handle, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, "imaps://imap.gmail.com:993/INBOX");
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
        curl_easy_setopt(handle, CURLOPT_CAINFO, CAcert.c_str());
        // curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

        size_t pos1, pos2;
        // get Sender
        command = "FETCH " + emailNumber + " BODY[HEADER.FIELDS (FROM)]";
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, command.c_str());
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &email.account);
        res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::osyncstream(std::cerr) << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }
        pos1 = email.account.find("From", 0);
        pos1 = email.account.find("<", pos1);
        pos2 = email.account.find(">", pos1);
        email.account = email.account.substr(pos1 + 1, pos2 - pos1 - 1);
        if (!isValidAdminEmail(email.account)) {
            std::osyncstream(std::cerr) << "Not email's admin!\n";
            curl_easy_cleanup(handle);
            return false;
        }
        
        // get subject
        command = "FETCH " + emailNumber + " BODY[HEADER.FIELDS (SUBJECT)]";
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, command.c_str());
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &email.subject);
        res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::osyncstream(std::cerr) << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }
        pos1 = email.subject.find(":", 0);
        pos2 = email.subject.find(seperateLine, pos1);
        email.subject = email.subject.substr(pos1 + 2, pos2 - pos1 - seperateLine.length());
        pos1 = email.subject.find_last_not_of(" \t\f\v\n\r");
        if (pos1 != std::string::npos)
            email.subject.erase(pos1 + 1);

        // get content
        std::string temp = "";
        command = "FETCH " + emailNumber + " BODY[TEXT]";
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, command.c_str());
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &temp);
        res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::osyncstream(std::cerr) << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }
        pos1 = temp.find("Content-Type", 0);
        if (temp.find("Content-Transfer-Encoding", pos1) != std::string::npos)
            pos1 = temp.find("Content-Transfer-Encoding", pos1);
        pos1 = temp.find(seperateLine, pos1);
        pos1 = temp.find(seperateLine, pos1 + 1);

        pos2 = temp.find("@gmail.com>", pos1 + 1);
        if (pos2 == std::string::npos || pos2 > (size_t)temp.find("--000", pos1 + 1))
            pos2 = temp.find("--000", pos1 + 1);

        temp = temp.substr(pos1 + seperateLine.length(), pos2 - pos1);
        pos2 = temp.find_last_of(seperateLine); temp.erase(pos2 - 2);
        pos2 = temp.find_last_of(seperateLine); temp.erase(pos2 - 2);
        pos1 = temp.find_last_not_of(" \t\f\v\n\r");
        if (pos1 != std::string::npos)
            temp.erase(pos1 + 1);

        pos1 = 0; pos2 = temp.find(seperateLine, 0);
        while (pos2 != std::string::npos) {
            content += temp.substr(pos1, pos2 - pos1);
            pos1 = pos2 + seperateLine.length();
            pos2 = temp.find(seperateLine, pos1);
        }
        content += temp.substr(pos1);

        // get messageID
        command = "FETCH " + emailNumber + " BODY.PEEK[HEADER.FIELDS (MESSAGE-ID)]";
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, command.c_str());
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &email.messageID);
        res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::osyncstream(std::cerr) << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }
        pos1 = email.messageID.find("Message-ID", 0);
        pos1 = email.messageID.find(" ", pos1) + 1;
        pos2 = email.messageID.find(seperateLine, pos1);
        email.messageID = email.messageID.substr(pos1, pos2 - pos1);
        pos1 = email.messageID.find_last_not_of(" \t\f\v\n\r");
        if (pos1 != std::string::npos) 
            email.messageID.erase(pos1 + 1);

        curl_easy_cleanup(handle);
    }
    return (res == CURLE_OK);
}

// bool GmailAccount::addAdminEmail(const std::string &email) {
//     if (adminEmails.find(email) != adminEmails.end()) {
//         std::cerr << email << " has already been admin email!\n";
//         return false;
//     }
//     adminEmails.insert(email);
//     std::ofstream fout("AdminAccount.txt", std::ios::app);
//     fout << email << "\n";
//     fout.close();
//     return true;
// }

// bool GmailAccount::removeAdminEmail(const std::string &email) {
//     if (adminEmails.find(email) == adminEmails.end()) {
//         std::cerr << email << " isn't an admin email!\n";
//         return false;
//     }
//     adminEmails.erase(email);
//     std::ofstream fout("AdminAccount.txt");
//     for (auto &e : adminEmails)
//         fout << e << "\n";
//     fout.close();
//     return true;
// }