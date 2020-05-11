#include "../inc/MutextLock.h"

namespace  snowlake {

MutextLock::MutextLock()
    : _isLocking(false)
{
    CHECK(!pthread_mutex_init(&_mutex, nullptr));
}

MutextLock::~MutextLock()
{
    assert(!_isLocking);
    CHECK(!pthread_mutex_destroy(&_mutex));
}

void MutextLock::lock()
{
    CHECK(!pthread_mutex_lock(&_mutex));
    _isLocking = true;
}

void MutextLock::unlock()
{
    _isLocking = false;
    CHECK(!pthread_mutex_unlock(&_mutex));
}

bool MutextLock::is_locking() const
{
    return _isLocking;
}

pthread_mutex_t* MutextLock::get_mutex_ptr()
{
    return &_mutex;
}

void MutextLock::restore_mutex_status()
{
    _isLocking = true;
}

};// end of namespace snowlake
