#include "Gmail.h"

int main () {
    Gmail mail(trojan, "htkx bbnr spzq lqad");
    std::string sender;
    std::string subject;
    std::string content;
    std::string emailNumber;

    // if (mail.sendEmail(admin, "Test C++ not send file", "Test C++ \n test not send file", "file/TranMinh.png"))
    //     std::cout << "Email sent successfully!\n";
    // else
    //     std::cout << "Failed to send email!\n";
    // if (mail.receiveEmail(sender, subject, content, emailNumber)) {
    //     std::cout << "Emails fetched successfully." << std::endl;
    // } else {
    //     std::cout << "Failed to fetch emails." << std::endl;
    // }
    // std:: cout << sender << "\n" << subject << "\n" << content << "\n";

    mail.autoCheckEmail();

    return 0;
}