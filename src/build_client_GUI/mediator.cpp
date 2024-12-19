#include "mediator.h"

void Mediator::Register(Participant *participant) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    listParticipants.push_back(participant);
}

void Mediator::Forward(std::string receiver, std::string msg) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    for (auto participant: listParticipants) if (participant->getName() == receiver) {
        participant->Receive(msg);
        break;
    }
}

void Mediator::Forward(std::string sender, std::string receiver, std::string msg) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    for (auto participant: listParticipants) if (participant->getName() == receiver) {
        participant->Receive(msg, sender);
        break;
    }
}

void Mediator::Forward(std::any *ptr, std::string dest) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::cerr << "Forwarding...\n";
    for (Participant *participant : listParticipants) {
        if (participant->getName() == dest) {
            participant->Receive(ptr);
        }
    }
}

void Mediator::Forward(std::any *ptr, std::string type, std::string dest) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    for (Participant *participant : listParticipants) {
        if (participant->getName() == dest) {
            participant->Receive(ptr, type);
        }
    }
}

void Mediator::Broadcast(Participant *sender, std::string msg) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::osyncstream(std::cout) << "Broadcast message: " << msg << '\n';
    for (auto participant: listParticipants) if (participant != sender) {
        participant->Receive(msg);
    }
}

void Mediator::Broadcast(Participant *sender, std::any *ptr) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    for (auto participant: listParticipants) if (participant != sender) {
        participant->Receive(ptr);
    }
}

void Mediator::Broadcast(Participant *sender, std::any *ptr, std::string type) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    for (auto participant: listParticipants) if (participant != sender) {
        participant->Receive(ptr, type);
    }
}

Participant::Participant() {}

Participant::Participant(Mediator *mediator, std::string name) {
    init(mediator, name);
}

void Participant::init(Mediator *mediator, std::string name) {
    this->mediator = mediator;
    this->name = name;
}

void Participant::Send(std::string msg) {
    // std::osyncstream(std::cout) << "Participant " << name << " send message: " << msg << '\n';
    mediator->Broadcast(this, msg);
}
void Participant::Send(std::string msg, std::string receiver) {
    // std::osyncstream(std::cout) << "Participant " << name << " send message: " << msg << " and to " << receiver << '\n';
    mediator->Forward(receiver, msg);
}

void Participant::Receive(std::string msg) {
    queueMessage.push(msg);
    std::osyncstream(std::cout) << "Participant " << name << " receive message: " << msg << '\n';
}

void Participant::Receive(std::string msg, std::string sender) {
    queueMessage.push(msg);
}

void Participant::Send(std::any *ptr) {
    mediator->Broadcast(this, ptr);
}

void Participant::Send(std::any *ptr, std::string dest) {
    mediator->Forward(ptr, dest);
}

void Participant::Send(std::any *ptr, std::string type, std::string dest) {
    mediator->Forward(ptr, type, dest);
}

void Participant::Receive(std::any *ptr) {
    queueAny.push(ptr);
}

void Participant::Receive(std::any *ptr, std::string type) {
    
}
