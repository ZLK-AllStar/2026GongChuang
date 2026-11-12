/*
 * SttThread.h
 *
 *  Created on: 2022年12月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include "core/Defines.h"
#include <thread>
#include <memory>
#include <atomic>

namespace stt {

class SttThread {
public:
	SttThread(uint16_t period=30);
	virtual ~SttThread();
	/**启动线程*/
	bool start();
	bool stop();
	const std::thread::id& getId() const;
	virtual bool threadRun();

	void setPeriod(uint16_t p);

private:
	void _run();
	bool _threadRun();
	void _SttThread();

	//线程对象实例
	std::shared_ptr<std::thread> m_threadHandler;
	//运行标识
	std::atomic_bool m_running;
	//当前线程ID
	std::thread::id m_id;
	uint16_t period;
};

}
