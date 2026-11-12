/*
 * SttObject.h
 *
 *  Created on: 2022年12月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include "core/Defines.h"
#include "core/SttLooper.h"
#include "core/SttThread.h"

#include <algorithm>

namespace stt {

class SttObject
{
public:
	SttObject(SttObject* parent = nullptr);
	virtual ~SttObject();
	SttLooper* eventLooper();
	void moveToThread(SttThread* th_);
	int exec();

protected:
	void addChild(SttObject* aChild);
	void removeAchild(SttObject* aChild);
	void clearChildren();

	SttLooper* m_looper;
	SttObject* m_parent;
	std::list<SttObject*> m_children;

};

}
