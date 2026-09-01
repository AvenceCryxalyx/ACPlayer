#include "Events.h"

Events::~Events()
{
    listeners.clear();
}

void Events::AddListener(const std::function<void(int)>& callback) {
    listeners.push_back(callback);
}

void Events::Invoke(int number) {
    for (const auto& callback : listeners) {
        callback(number);
    }
}