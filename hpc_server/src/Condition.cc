#include "../inc/Condition.h"
#include "../inc/MutextLock.h"

#include <stdio.h>
#include <errno.h>

namespace snowlake {

Condition::Condition(MutextLock & mutex)
    : _mutex(mutex)
{
    CHECK(!pthread_cond_init(&_cond, nullptr));
}
Condition::~Condition()
{
    CHECK(!pthread_cond_destroy(&_cond));
}
void Condition::wait()
{
    CHECK(!pthread_cond_wait(&_cond,_mutex.get_mutex_ptr()));
}

void Condition::notify()
{
    CHECK(!pthread_cond_signal(&_cond));
}

void Condition::notify_all()
{
    CHECK(!pthread_cond_broadcast(&_cond));
}

};// end of namespace snowlake
