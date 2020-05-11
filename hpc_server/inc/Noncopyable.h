#pragma once

#ifndef __SNOWLAKE_NONCOPYABLE_H__
#define __SNOWLAKE_NONCOPYABLE_H__

namespace  snowlake {// namespace snowlake

class Noncopyable
{
protected:
    Noncopyable() {}
    ~Noncopyable() {}
    
    Noncopyable(const Noncopyable&)=delete;
    Noncopyable& operator=(const Noncopyable&)=delete ;
    
};
};// end of namespace snowlake
#endif
