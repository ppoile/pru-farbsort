#ifndef PISTON_INTERFACE
#define PISTON_INTERFACE

class PistonInterface{
public:
    virtual void push() = 0;
    virtual void pull() = 0;
    virtual bool isPushed() = 0;
};

#endif // PISTON_INTERFACE

