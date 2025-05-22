#pragma once

namespace Manager {

class NetworkManager : public Manger {
public:
        NetworkManager() : Manager("NetworkManager") {}
    virtual ~NetworkManager() = default;

    virtual bool init(void);
    virtual bool service(void* args);
    virtual bool cleanup(void);
};

}; //namespace Manager