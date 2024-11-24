#include "Gmail.h"

size_t Gmail::headerCallback(char* buffer, size_t size, size_t nitems, std::string* userdata) {
    userdata -> append(std::string(buffer, size * nitems));
    userdata -> append("#");
    return size * nitems;
}

void Gmail::readAdminEmail() {
    std::ifstream fin ("AdminAccount");
    if (!fin.is_open()) {
        std::cerr << "Fail to read Admin's Email!\n";
        return;
    }

    std::string s;
    while (getline(fin, s)) {
        adminEmail.insert(s);
    }

    fin.close();
}

bool Gmail::isValidAdminEmail(const std::string &sender) {
    return (adminEmail.find(sender) != adminEmail.end());
}

std::queue<std::string> Gmail::searchNewEmail()
{
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
            std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
            return qEmailNums;
        }
        // get email numbers
        size_t pos1 = 0, pos2 = 0;
        pos1 = searchResult.find("EXISTS", pos1);
        pos1 = searchResult.find("SEARCH", pos1);
        pos1 = searchResult.find(" ", pos1) + 1;
        pos2 = searchResult.find("#", pos1);
        searchResult = searchResult.substr(pos1, pos2 - pos1);

        pos1 = pos2 = 0;
        while (pos2 = searchResult.find(' ', pos2)) {
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

bool Gmail::sendEmail(const std::string &receiver, const std::string &subject, const std::string &content) {
    CURL* handle;
    CURLcode res = CURLE_OK;
    struct curl_slist* listRecipients = nullptr;
    struct curl_slist* headers = nullptr;
    curl_mime* mime;
    curl_mimepart* part;

    std::string header[] = {
        "To: " + receiver,
        "from: " + username,
        "Subject: " + subject,
        ""
    };

    handle = curl_easy_init();
    if (handle) {
        // add information of email account (sender)
        curl_easy_setopt(handle, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(handle, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(handle, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(handle, CURLOPT_MAIL_FROM, ("<" + username + ">").c_str());

        // add information of email (receivers)
        listRecipients = curl_slist_append(listRecipients, ("<" + receiver + ">").c_str());
        curl_easy_setopt(handle, CURLOPT_MAIL_RCPT, listRecipients);

        // add headers
        for (std::string* str = header; *str != ""; ++str)
            headers = curl_slist_append(headers, (*str).c_str());
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

        mime = curl_mime_init(handle);

        // add contents
        part = curl_mime_addpart(mime);
        curl_mime_data(part, content.c_str(), CURL_ZERO_TERMINATED);

        // send data
        curl_easy_setopt(handle, CURLOPT_MIMEPOST, mime);

        curl_easy_setopt(handle, CURLOPT_CAINFO, CAcert.c_str());
        // curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(handle);
        if (res != CURLE_OK)
            std::cerr << "ERROR!!!\n" << curl_easy_strerror(res) << "\n";

        // clean memory
        curl_mime_free(mime);
        curl_slist_free_all(headers);
        curl_slist_free_all(listRecipients);
        curl_easy_cleanup(handle); 
    }
    return (res == CURLE_OK);
}

bool Gmail::sendEmail(const std::string &receiver, const std::string &subject, const std::string &content, const std::string &filePath) {
    CURL* handle;
    CURLcode res = CURLE_OK;
    struct curl_slist* listRecipients = nullptr;
    struct curl_slist* headers = nullptr;
    curl_mime* mime;
    curl_mimepart* part;

    std::string header[] = {
        "To: " + receiver,
        "From: " + username,
        "Subject: " + subject,
        ""
    };

    handle = curl_easy_init();
    if (handle) {
        // add information of email account (sender)
        curl_easy_setopt(handle, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(handle, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(handle, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(handle, CURLOPT_MAIL_FROM, ("<" + username + ">").c_str());

        // add information of email (receivers)
        listRecipients = curl_slist_append(listRecipients, ("<" + receiver + ">").c_str());
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
        part = curl_mime_addpart(mime);
        curl_mime_filedata(part, filePath.c_str());
        curl_mime_encoder(part, "base64");
        // curl_mime_filename(part, (filePath.substr(filePath.rfind("/"))).c_str());

        // send data
        curl_easy_setopt(handle, CURLOPT_MIMEPOST, mime);

        // CA certificate
        curl_easy_setopt(handle, CURLOPT_CAINFO, CAcert.c_str());
        // curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(handle);
        if (res != CURLE_OK)
            std::cerr << "ERROR!!!\n" << curl_easy_strerror(res) << "\n";

        // clean memory
        curl_slist_free_all(listRecipients);
        curl_slist_free_all(headers);
        curl_mime_free(mime);
        curl_easy_cleanup(handle); 
    }
    return (res == CURLE_OK);
}

bool Gmail::receiveEmail(std::string &sender, std::string &subject, std::string &content, const std::string &emailNumber) {
    CURL* handle;
    CURLcode res = CURLE_OK;
    std::string command;

    handle = curl_easy_init();
    if(handle) {
        curl_easy_setopt(handle, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(handle, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, "imaps://imap.gmail.com:993/INBOX");
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(handle, CURLOPT_CAINFO, CAcert.c_str());
        // curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

        size_t pos1, pos2;
        // get Sender
        command = "FETCH " + emailNumber + " BODY[HEADER.FIELDS (FROM)]";
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, command.c_str());
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &sender);
        res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }
        pos1 = sender.find("<", 0);
        pos2 = sender.find(">", pos1);
        sender = sender.substr(pos1 + 1, pos2 - pos1 - 1);
        
        // get subject
        command = "FETCH " + emailNumber + " BODY[HEADER.FIELDS (SUBJECT)]";
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, command.c_str());
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &subject);
        res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }
        pos1 = subject.find(":", 0);
        pos2 = subject.find(")", pos1);
        subject = subject.substr(pos1 + 2, pos2 - pos1 - 6);

        // get content
        command = "FETCH " + emailNumber + " BODY[TEXT]";
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, command.c_str());
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &content);
        res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }
        pos1 = content.find(">", 0);
        pos2 = content.find("<", pos1);
        content = content.substr(pos1 + 1, pos2 - pos1 - 1);

        curl_easy_cleanup(handle);
    }
    return (res == CURLE_OK);
}

void Gmail::autoCheckEmail() {
    std::string sender;
    std::string subject;
    std::string content;
    std::queue<std::string> emailNumbers;

    while (true) {
        if (_kbhit()) {
            std::cout << "Key pressed. Stop check email!\n";
            break;
        }
        emailNumbers = this->searchNewEmail();
        while (!emailNumbers.empty()) {
            std::string eNum = emailNumbers.front();
            emailNumbers.pop();
            this->receiveEmail(sender, subject, content, eNum);
            if (!isValidAdminEmail(sender)) {
                std::cerr << "Not Admin!\n";
                continue;
            }
            std::cout << "New email: \n";
            std::cout << "\tSender: \t" << sender << "\n"
                      << "\tSubject: \t" << subject << "\n"
                      << "\tContent: \t" << content << "\n";
            std::cout << "----------------------------\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}