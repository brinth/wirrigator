namespace Manager {

class Manager {
public:
            Manager(const std::string& name = "Manager") : _name(name) {};
    virtual ~Manager() = default;

    virtual bool init(void);
    virtual bool service(void* args);
    virtual bool cleanup(void);

private:
    std::string _name;
};

};