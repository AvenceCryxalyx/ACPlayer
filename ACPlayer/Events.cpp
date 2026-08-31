#include "Events.h"

Events::~Events()
{
    listeners.clear();
}

void Events::AddListener(std::function<void()> callback) {
    listeners.push_back(callback);
}

void Events::Invoke() {
    for (const auto& callback : listeners) {
        callback();
    }
}