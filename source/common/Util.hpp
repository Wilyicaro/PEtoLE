/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <iomanip>

class Util
{
public:
	static const std::string EMPTY_STRING;

public:
	static std::string stringTrim(const std::string &, const std::string &, bool, bool);
	static std::string stringTrim(const std::string &);

	template<typename... Args>
	static std::string format(const char* fmt, Args... args) {
		int size = std::snprintf(nullptr, 0, fmt, args...) + 1;
		if (size <= 0) {
			return EMPTY_STRING;
		}

		std::vector<char> buf(size);
		std::snprintf(buf.data(), size, fmt, args...);
		return std::string(buf.data());
	}

	template<typename T>
	static std::string toString(const T& value) {
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}

    template<typename... Args>
    static std::string format(const std::string& fmt, Args&&... args) {
        std::vector<std::string> argStrings = { toString(std::forward<Args>(args))... };

        std::string result;
        size_t pos = 0;
        size_t argIndex = 0;

        while (pos < fmt.size()) {
            if (fmt[pos] == '%') {
                size_t start = pos;
                ++pos;

                if (pos >= fmt.size()) {
                    result += '%';
                    break;
                }

                int argNum = -1;
                if (std::isdigit(fmt[pos])) {
                    argNum = 0;
                    while (pos < fmt.size() && std::isdigit(fmt[pos])) {
                        argNum = argNum * 10 + (fmt[pos] - '0');
                        ++pos;
                    }
                    if (pos < fmt.size() && fmt[pos] == '$') ++pos;
                    else argNum = -1;
                }

                bool leftAlign = false;
                bool zeroPad = false;
                int width = 0;
                int precision = -1;

                while (pos < fmt.size()) {
                    char c = fmt[pos];
                    if (c == '-') { leftAlign = true; ++pos; }
                    else if (c == '0') { zeroPad = true; ++pos; }
                    else break;
                }

                if (pos < fmt.size() && std::isdigit(fmt[pos])) {
                    width = 0;
                    while (pos < fmt.size() && std::isdigit(fmt[pos])) {
                        width = width * 10 + (fmt[pos] - '0');
                        ++pos;
                    }
                }

                if (pos < fmt.size() && fmt[pos] == '.') {
                    ++pos;
                    precision = 0;
                    while (pos < fmt.size() && std::isdigit(fmt[pos])) {
                        precision = precision * 10 + (fmt[pos] - '0');
                        ++pos;
                    }
                }

                if (pos >= fmt.size()) {
                    result += fmt.substr(start);
                    break;
                }

                char type = fmt[pos];
                ++pos;

                std::string replacement;

                size_t useIndex = (argNum >= 0) ? static_cast<size_t>(argNum - 1) : argIndex++;

                if (useIndex >= argStrings.size()) {
                    replacement = "%" + std::string(1, type);
                }
                else {
                    const std::string& value = argStrings[useIndex];

                    std::ostringstream oss;

                    switch (type)
                    {
                    case 's':
                        oss << value;
                        break;
                    case 'd':
                        oss << std::stoi(value);
                        break;
                    case 'f':
                        if (precision >= 0) oss << std::fixed << std::setprecision(precision);
                        oss << std::stod(value);
                        break;
                    default:
                        oss << "%" << type;
                        break;
                    }

                    replacement = oss.str();

                    if (width > 0) {
                        if (leftAlign) {
                            replacement += std::string(width - replacement.length(), ' ');
                        }
                        else if (zeroPad && type != 's') {
                            replacement = std::string(width - replacement.length(), '0') + replacement;
                        }
                        else {
                            replacement = std::string(width - replacement.length(), ' ') + replacement;
                        }
                    }
                }

                result += replacement;
            }
            else {
                result += fmt[pos++];
            }
        }

        return result;
    }

	template<typename T>
	static bool remove(std::vector<T>& vec, const T& t)
	{
		typename std::vector<T>::iterator iter = std::find(vec.begin(), vec.end(), t);
		if (iter == vec.end())
			return false;

		vec.erase(iter);
		return true;
	}

	template<typename T>
	static int removeAll(std::vector<T>& vec, const std::vector<T>& toRemove)
	{
		int removed = 0;

		for (typename std::vector<T>::const_iterator it = toRemove.begin(); it != toRemove.end(); it++)
		{
			T rem = *it;

			if (remove(vec, rem))
				removed++;
		}

		return removed;
	}

	// @TODO: reverse the actual thing? This is something different, but I'm lazy. It uses std::string::replace
	static void stringReplace(std::string& in, const std::string& what, const std::string& with)
	{
		//snippet from Zahlman's post on gamedev:  http://www.gamedev.net/community/forums/topic.asp?topic_id=372125
		size_t pos = 0;
		size_t whatLen = what.length();
		size_t withLen = with.length();
		while ((pos = in.find(what, pos)) != std::string::npos)
		{
			in.replace(pos, whatLen, with);
			pos += withLen;
		}
	}

	static int32_t hashCode(const std::string& str)
	{
		int32_t result = 0;

		const size_t size = str.size();
		for (int i = 0; i < size; i++)
		{
			result = result * 31 + str.at(i);
		}


		return result;
	}
};
