#pragma once
class Selector
{
    int value;
    int max;

public:
    void operator++();
    void operator--();
    Selector(int);
    Selector();
    void setMax(int);
    int getValue();
};