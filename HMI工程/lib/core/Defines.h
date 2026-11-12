/*
 * Defines.h
 *
 *  Created on: 2022年12月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include <chrono>

#define __UNUSE(X) (void(X))

#define __NO_CPOY_ASSIGN(CLASS) \
    CLASS(const CLASS&); \
    CLASS& operator=(const CLASS&) \

#define HAS_MEMBER(member)\
    template<typename T, typename... Args>struct has_member_##member\
{\
    private:\
    template<typename U> static auto Check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type()); \
    template<typename U> static auto Check(...) -> decltype(std::false_type()); \
    public:\
    static const bool value = std::is_same<decltype(Check<T>(0)), std::true_type>::value; \
    } \

