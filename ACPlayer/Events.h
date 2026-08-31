#pragma once
#include <iostream>
#include <vector>
#include <functional>

using namespace std;

class Events
{
public:
    ~Events();
    void AddListener(std::function<void()> callback);
    void Invoke();

private:
    vector<function<void()>> listeners;
};

