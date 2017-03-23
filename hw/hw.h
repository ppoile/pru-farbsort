#ifndef HW
#define HW


class LightBarrierInterface;
class PistonInterface;
class AdcInterface;
class MotorInterface;

struct Hw
{
    MotorInterface *motor;
    PistonInterface *piston0;
    PistonInterface *piston1;
    PistonInterface *piston2;
    LightBarrierInterface *lightBarrier0;
    LightBarrierInterface *lightBarrier1;
    LightBarrierInterface *lightBarrierEmergencyStop;
    AdcInterface *adc;

    Hw(MotorInterface *motor,
        PistonInterface *piston0,
        PistonInterface *piston1,
        PistonInterface *piston2,
        LightBarrierInterface *lightBarrier0,
        LightBarrierInterface *lightBarrier1,
        LightBarrierInterface *lightBarrierEmergencyStop,
        AdcInterface *adc):
            motor(motor),
            piston0(piston0),
            piston1(piston1),
            piston2(piston2),
            lightBarrier0(lightBarrier0),
            lightBarrier1(lightBarrier1),
            lightBarrierEmergencyStop(lightBarrierEmergencyStop),
            adc(adc){}

};

#endif // HW

