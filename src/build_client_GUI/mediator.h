#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <syncstream>
#include <any>
#include "../share_queue.h"

class COMPONENT {
public:
    static constexpr char UI_COMPONENT[] = "UI";
    static constexpr char MAIL_COMPONENT[] = "MAIL";
    static constexpr char CLIENT_COMPONENT[] = "CLIENT";
    static constexpr char BROADCAST_COMPONENT[] = "BROADCAST";
    static constexpr char MEDIATOR_COMPONENT[] = "MEDIATOR";
};

class Mediator;

class Participant {
protected:
    Mediator* mediator;
    std::string name;
    concurrent_queue<std::string> queueMessage;
    concurrent_queue<std::any*> queueAny;
    std::mutex mtx;
public:
    Participant();
    Participant(Mediator* mediator, std::string name);// {}
    void init(Mediator* mediator, std::string name);

    virtual void Send(std::string msg);
    virtual void Send(std::string msg, std::string receiver);
    virtual void Receive(std::string msg);
    virtual void Receive(std::string msg, std::string sender);

    virtual void Send(std::any *ptr);
    virtual void Send(std::any *ptr, std::string dest);
    virtual void Send(std::any *ptr, std::string type, std::string dest);
    virtual void Receive(std::any *ptr);
    virtual void Receive(std::any *ptr, std::string type);
    std::string getName() { return name; }
};

class Mediator {
protected:
    std::vector<Participant*> listParticipants;
    std::mutex mtx;
public:
    void Register(Participant* participant);
    void Forward(std::string receiver, std::string msg);
    void Forward(std::string sender, std::string receiver, std::string msg);
    void Forward(std::any *ptr, std::string dest);
    void Forward(std::any *ptr, std::string type, std::string dest);
    void Broadcast(Participant* sender, std::string msg);
    void Broadcast(Participant* sender, std::any *ptr);
    void Broadcast(Participant* sender, std::any *ptr, std::string type);
};