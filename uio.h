#pragma once
#include <iostream>
#include "utils.h"

#include "Reg.h"

namespace  snowlake { // start namespace snowlake 


class uio
{
public:
    uio();
    ~uio();
    
private:
    Reg *m_reg;
};

}; // end of namespace std;



