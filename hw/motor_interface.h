#ifndef MOTOR_INTERFACE
#define MOTOR_INTERFACE

class MotorInterface
{
    public:
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual bool isRunning() = 0;
};

#endif // MOTOR_INTERFACE

