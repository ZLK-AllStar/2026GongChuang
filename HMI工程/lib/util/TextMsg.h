/*
 * TextMsg.h
 *
 *  Created on: 2022年12月29日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>

namespace stt {

/**
 * 字符串消息
 */
class TextMsg {
public:
	TextMsg(const char* text);
	TextMsg(const TextMsg& e);
	TextMsg(std::string s);
	~TextMsg() throw();
	const char* content() throw ();

private:
	char* text_;
};

}

