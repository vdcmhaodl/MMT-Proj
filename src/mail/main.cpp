#include "Gmail.h"

int main() {
    std::cout << "START\n";
    std::string username;
    std::string password;
    std::getline(std::cin, username);
    std::getline(std::cin, password);
    GmailAccount G(username, password);

    // std::queue<Mail> listMail = G.getEmailQueue();
    // while(!listMail.empty()) {
    //     Mail mail = listMail.front();
    //     listMail.pop();

    //     Email sender = mail.sender;
    //     std::string content = mail.content;

    //     std::cout << "Mail: " << sender.account << '\t' << sender.messageID << '\t' << sender.subject << '\n';
    //     std::cout << content << '\n';
    // }

    std::string emailNum;
    getline(std::cin, emailNum);

    Mail mail;
    G.receiveEmail(mail.sender, mail.content, emailNum);

    std::cout << mail.sender.account << "\n" << mail.sender.subject << mail.content;
}