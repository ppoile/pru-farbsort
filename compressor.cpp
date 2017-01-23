#include "compressor.h"
#include "msg_definition.h"

int16_t post_info(char info);

Compressor::Compressor(int mask) : Gpo(mask)
{

}

void Compressor::stop()
{
    clear();
    post_info(INFO_COMPRESSOR_STOP);
}

void Compressor::start()
{
    set();
    post_info(INFO_COMPRESSOR_START);
}
