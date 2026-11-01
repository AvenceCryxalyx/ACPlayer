#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <string>
using namespace std;

class Events
{
public:
    ~Events();
    void AddListener(const std::function<void(string)>& callback);
    void AddListenerInt(const std::function<void(int)>& callback);
    void Invoke(string name);
    void Invoke(int number);

private:
    vector<function<void(string)>> listeners;
    vector<function<void(int)>> listenersint;
};

