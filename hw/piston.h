#ifndef PISTON_H
#define PISTON_H

#include "gpo.h"
#include "piston_interface.h"

class RpMsgTxInterface;

class Piston : public PistonInterface, public Gpo
{
public:
    Piston(int id, uint32_t mask, RpMsgTxInterface *rpmsg);

    void push();
    void pull();
    bool isPushed();

private:
    int id;
    RpMsgTxInterface *rpmsg;
};

#endif // PISTON_H
