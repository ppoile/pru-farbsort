#ifndef LIGHTBARRIER_H
#define LIGHTBARRIER_H

#include "gpi.h"
#include "light_barrier_interface.h"

class RpMsgTxInterface;

class LightBarrier : public Gpi, public LightBarrierInterface
{
private:


public:
    LightBarrier(int id, int mask, RpMsgTxInterface *rpmsg);
    bool isInterrupted(void);
    void poll();

private:
    bool _isValidId() const;
    uint8_t _getMsgFromIdAndInterrupted(bool isInterrupted);

    const int id;
    bool oldIsInterrupted;
    RpMsgTxInterface *rpmsg;
};

#endif // LIGHTBARRIER_H
