#include "Events.h"

Events::~Events()
{
    listenersint.clear();
    listeners.clear();
}

void Events::AddListener(const std::function<void(string)>& callback) {
    listeners.push_back(callback);
}

void Events::AddListenerInt(const std::function<void(int)>& callback) {
    listenersint.push_back(callback);
}

void Events::Invoke(string name) {
    for (const auto& callback : listeners) {
        callback(name);
    }
}

void Events::Invoke(int number) {
    for (const auto& callback : listenersint) {
        callback(number);
    }
}