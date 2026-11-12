/*
 * SttTask.h
 *
 *  Created on: 2022年12月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include "core/Defines.h"
#include "core/SttTaskPolicy.h"
#include <memory>
#include <functional>

namespace stt {

struct SttTaskBase
{
	virtual ~SttTaskBase() {}
	virtual void run() = 0;
	virtual int64_t ttl() const = 0;
	virtual int type() const = 0;
	virtual void setTtl(int ms_) = 0;
	virtual void * getObj()=0;
};

template <typename _Policy>
class SttTask : public SttTaskBase
{
	struct __Impl_base;
	typedef std::shared_ptr<__Impl_base> __share_base_type;

	struct __Impl_base
	{
		virtual ~__Impl_base() {}
		virtual void _run() = 0;
	};

	template <typename _Callable>
	struct __Impl : public __Impl_base
	{
		_Callable m_fun;
		__Impl(_Callable&& f_)
			: m_fun(std::forward<_Callable>(f_))
		{

		}

		void _run()
		{
			m_fun();
		}
	};

public:
	template <typename _Callable, typename ... Args>
	SttTask(void* obj,_Callable&& f_, Args&& ... args)
	{
		m_object = obj;
		m_Mfun = _make_routine(std::bind(std::forward<_Callable>(f_), std::forward<Args>(args)...));
	}
	void * getObj(){
		return m_object;
	}
	void run()
	{
		m_Mfun->_run();
	}

	int64_t ttl() const
	{
		return m_taskPolicy.ttl().count();
	}

	int type() const
	{
		return m_taskPolicy.type();
	}

	void setTtl(int ms_)
	{
		m_taskPolicy.setTtl(ms(ms_));
	}
	void * m_object;
private:

	template <typename _Callable>
	std::shared_ptr<__Impl<_Callable>> _make_routine(_Callable&& f_) {
		return std::make_shared<__Impl<_Callable>>(std::forward<_Callable>(f_));
	}

	__share_base_type m_Mfun;
	_Policy m_taskPolicy;
};

typedef SttTask<SttCommonTaskPolicy> SttCommonTask;
typedef SttTask<SttDelayTaskPolicy> SttDelayTask;

}
