#pragma once

namespace Manager {

class SysManager : public Manager {
public:
        SysManager() : Manager("SysManager") {}
    virtual ~SysManager() = default;

    virtual bool init(void);
    virtual bool service(void* args);
    virtual bool cleanup(void);
};
    
}; // namespace Manager