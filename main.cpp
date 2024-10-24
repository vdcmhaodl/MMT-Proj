#include "Gmail.h"

int main () {
    std::string password;
    getline(std::cin, password);
    Gmail mail(trojan, password);
    
    mail.autoCheckEmail();

    return 0;
}