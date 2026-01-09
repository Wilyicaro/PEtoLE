#pragma once

#include <string>

class StatFormatter
{
public:
    virtual std::string format(int) const = 0;
};

class DistanceFormatter : public StatFormatter
{
public:
    std::string format(int) const override;

public:
    static const DistanceFormatter INSTANCE;
};

class DateFormatter : public StatFormatter
{
public:
    std::string format(int) const override;

public:
    static const DateFormatter INSTANCE;
};

class NumberFormatter : public StatFormatter
{
public:
    std::string format(int) const override;

public:
    static const NumberFormatter INSTANCE;
};

class DecimalFormatter : public StatFormatter
{
public:   
    DecimalFormatter(int precision = 2);        
    std::string format(int) const override;

public:
    int m_precision;

    static const DecimalFormatter INSTANCE;
};