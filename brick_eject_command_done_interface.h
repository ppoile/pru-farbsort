#ifndef BRICKEJECTCOMMANDDONEINTERFACE
#define BRICKEJECTCOMMANDDONEINTERFACE

class BrickEjectCommand;

class BrickEjectCommandDoneInterface
{
public:
    virtual void brickEjectCommandDone(BrickEjectCommand *command)=0;
};

#endif // BRICKEJECTCOMMANDDONEINTERFACE

