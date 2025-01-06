#include "sl_test_dsp.hpp"
#include "idsp/functions.hpp"

void Sl_test::process(const DspBuffer& input, DspBuffer& output)
{
    // DSP processing code here
    for(size_t i = 0; i < input[0].size(); i++)
    {
        output[0][i] = input[0][i] * gain;
        output[1][i] = input[1][i] * gain;
    }
}
