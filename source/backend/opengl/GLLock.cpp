//
//  GLLock.cpp
//  MNN
//
//  Created by MNN on 2019/01/31.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#include "backend/opengl/GLLock.hpp"
#include <assert.h>
#if defined(__ANDROID__)
#include <pthread.h>
namespace MNN {
namespace OpenGL {

GLLock::GLLock() {
    pthread_mutex_t* m = new pthread_mutex_t;
    pthread_mutex_init(m, NULL);
    mData = (void*)m;
}

GLLock::~GLLock() {
    assert(NULL != mData);
    pthread_mutex_t* m = (pthread_mutex_t*)mData;
    pthread_mutex_destroy(m);
    delete m;
}

void GLLock::lock() {
    assert(NULL != mData);
    pthread_mutex_lock((pthread_mutex_t*)mData);
}

void GLLock::unlock() {
    assert(NULL != mData);
    pthread_mutex_unlock((pthread_mutex_t*)mData);
}
} // namespace OpenGL
} // namespace MNN

#elif defined(_WIN32)
#include <Windows.h>

namespace MNN {
namespace OpenGL {

GLLock::GLLock() {
    CRITICAL_SECTION* m = new CRITICAL_SECTION;
    InitializeCriticalSectionEx(m, 0x0400, 0);
    mData = m;
}

GLLock::~GLLock() {
    assert(NULL != mData);
    CRITICAL_SECTION* m = reinterpret_cast<CRITICAL_SECTION*>(mData);
    DeleteCriticalSection(m);
    delete m;
}

void GLLock::lock() {
    assert(NULL != mData);
    CRITICAL_SECTION* m = reinterpret_cast<CRITICAL_SECTION*>(mData);
    EnterCriticalSection(m);
}

void GLLock::unlock() {
    assert(NULL != mData);
    CRITICAL_SECTION* m = reinterpret_cast<CRITICAL_SECTION*>(mData);
    LeaveCriticalSection(m);
}
} // namespace OpenGL
} // namespace MNN
#endif
