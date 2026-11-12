/*
 * SttLooper.h
 *
 *  Created on: 2022年12月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include "core/Defines.h"
#include <map>
#include <thread>
#include <atomic>
#include <algorithm>
#include "core/SttQueue.h"

namespace stt {

//队列环
class SttLooper
{
public:
	static SttLooper* currentLooper();
	static SttLooper* getLooper(const std::thread::id& id_);
	static void clean();
	static void notifyAll();
	void enqueue(SttTaskBase* t_);
	bool dequeue(SttTaskBase*& t_, int ms_ = -1);
	void exec_once(int ms_);
	int exec(int ms_= -1);
	void exit();
	//当前任务队列
	SttQueue m_queue;
private:
	SttLooper();

private:
	//取队列中最低时间进行等待
	void waitdelay();

	//当前任务是否运行
	std::atomic_bool m_isRunning;

	//任务队列集合
	static std::map<std::thread::id, SttLooper*> m_loopers;
	//任务队列集合操作锁
	static std::mutex m_mutex;
};

}
