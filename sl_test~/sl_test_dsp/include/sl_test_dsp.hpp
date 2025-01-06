#ifndef sl_test_DSP_HPP
#define sl_test_DSP_HPP

#include "idsp/buffer_types.hpp"

using DspBuffer = idsp::PolySampleBufferDynamic<2>;

class Sl_test
{
    public:
        Sl_test() : gain(1.0f)
        {}

        void process(const DspBuffer& input, DspBuffer& output);
        void set_gain(const float f) {gain = f;}

    private:
        float gain;
};

#endif // sl_test_DSP_HPP
