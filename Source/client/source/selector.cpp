#include "../headers/selector.h"

Selector::Selector(int _max)
{
    max = _max;
    value = 0;
}
Selector::Selector()
{
    max = 0;
    value = 0;
}

void Selector::operator++()
{
    if (value == max)
    {
        value = 0;
    }
    else
    {
        value++;
    }
}

void Selector::operator--()
{
    if (value == 0)
    {
        value = max;
    }
    else
    {
        value--;
    }
}

int Selector::getValue()
{
    return value;
}

void Selector::setMax(int _max)
{
    max = _max;
}