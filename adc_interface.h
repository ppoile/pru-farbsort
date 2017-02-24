#ifndef ADCINTERFACE
#define ADCINTERFACE


class AdcInterface
{
    public:
        virtual uint16_t read() = 0;
};

#endif // ADCINTERFACE

