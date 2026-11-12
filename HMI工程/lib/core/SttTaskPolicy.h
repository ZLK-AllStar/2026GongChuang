/*
 * SttTaskPolicy.h
 *
 *  Created on: 2022年12月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include "core/Defines.h"

namespace stt {

typedef std::chrono::milliseconds ms;

enum TASK_TYPE
{
    TASK_COMMON = 1 << 0,
    TASK_DELAY = 1 << 1,
};

struct SttTaskPolicy
{
	virtual ~SttTaskPolicy() {}
	virtual ms ttl() const
	{
		return ms(0);
	}

	virtual int type() const = 0;

	virtual void setTtl(ms ms_)
	{
		__UNUSE(ms_);
	}
};

class SttCommonTaskPolicy : public SttTaskPolicy
{
public:
	int type() const
	{
		return TASK_COMMON;
	}
};

class SttDelayTaskPolicy : public SttTaskPolicy
{
public:

	SttDelayTaskPolicy();
	int type() const;
	ms ttl() const;
	void setTtl(ms ms_);

private:
	std::chrono::system_clock::time_point m_delay;
};

}

