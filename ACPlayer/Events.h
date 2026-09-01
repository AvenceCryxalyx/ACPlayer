#pragma once
#include <iostream>
#include <vector>
#include <functional>

using namespace std;

class Events
{
public:
    ~Events();
    void AddListener(const std::function<void(int)>& callback);
    void Invoke(int number);

private:
    vector<function<void(int)>> listeners;
};

