/*
 * SttQueue.h
 *
 *  Created on: 2022年12月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include "Defines.h"
#include "SttTask.h"
#include <list>
#include <condition_variable>
#include <mutex>

namespace stt {

class SttQueue {
	friend class SttLooper;
public:
	void enqueue(SttTaskBase* &t_);
	bool dequeue(SttTaskBase* &t_, int ms_ = -1);
	void remove(void *object_);
	std::list<SttTaskBase*> m_queue;
private:
	std::condition_variable m_cv;
	std::mutex m_mutex;

};

}
