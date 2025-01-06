#include <m_pd.h>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <functional>
#include "sl_spectral_dsp.hpp"

static t_class *sl_spectral_tilde_class;

class sl_spectralTilde
{
  public:
    t_object object;
    t_sample signal_in;
    t_float gain;
    t_float sample_rate;
    t_inlet* inlet_left;
    t_inlet* inlet_right;
    t_outlet* outlet_left;
    t_outlet* outlet_right;
    t_outlet* outlet_info;
    int block_size;
    DspBuffer input;
    DspBuffer output;
    Sl_spectral* sl_spectral;
};

// ─────────────────────────────────────
static void on_bang(sl_spectralTilde *x)
{
    // // post("Hello from sl_spectral~");
    // std::array<std::array<float, 5>, 2> harmonics = x->sl_spectral->getHarmonics();
    // t_atom configurationInfo[11];
    // SETSYMBOL(configurationInfo, gensym("harmonics"));
    // SETFLOAT(configurationInfo+1, harmonics[0][0]);
    // SETFLOAT(configurationInfo+2, harmonics[1][0]);
    // SETFLOAT(configurationInfo+3, harmonics[0][1]);
    // SETFLOAT(configurationInfo+4, harmonics[1][1]);
    // SETFLOAT(configurationInfo+5, harmonics[0][2]);
    // SETFLOAT(configurationInfo+6, harmonics[1][2]);
    // SETFLOAT(configurationInfo+7, harmonics[0][3]);
    // SETFLOAT(configurationInfo+8, harmonics[1][3]);
    // SETFLOAT(configurationInfo+9, harmonics[0][4]);
    // SETFLOAT(configurationInfo+10, harmonics[1][4]);
    // outlet_list(x->outlet_info, gensym("list"),11,configurationInfo);

    t_atom configurationInfo[2];
    SETSYMBOL(configurationInfo, gensym("euclidean"));
    SETFLOAT(configurationInfo+1, x->sl_spectral->get_euclidean());
    outlet_list(x->outlet_info, gensym("list"),2,configurationInfo);
    SETSYMBOL(configurationInfo, gensym("cosineSimilarity"));
    SETFLOAT(configurationInfo+1, x->sl_spectral->get_cosineSimilarity());
    outlet_list(x->outlet_info, gensym("list"),2,configurationInfo);
    SETSYMBOL(configurationInfo, gensym("flatness"));
    SETFLOAT(configurationInfo+1, x->sl_spectral->get_flatness());
    outlet_list(x->outlet_info, gensym("list"),2,configurationInfo);
    SETSYMBOL(configurationInfo, gensym("centroid"));
    SETFLOAT(configurationInfo+1, x->sl_spectral->get_zeroCrossingRate());
    outlet_list(x->outlet_info, gensym("list"),2,configurationInfo);
    // SETSYMBOL(configurationInfo, gensym("frequency"));
    // SETFLOAT(configurationInfo+1, x->sl_spectral->get_autocorr_freq());
    // outlet_list(x->outlet_info, gensym("list"),2,configurationInfo);
}

// ─────────────────────────────────────
static void print_info(sl_spectralTilde *x)
{
    post(" Options: gain <float> (0-1) - Sets gain level ");
}

// ─────────────────────────────────────
static void interpret_control(sl_spectralTilde *x, const std::string& control_messages)
{
    std::istringstream stream(control_messages);
    std::string message_id;
    std::unordered_map<std::string, std::function<void(float)>> control_map;

    //ADD CONTROL MESSAGES HERE
    control_map["gain"] = [&x](float f) { x->sl_spectral->set_gain(f); };

    while(stream >> message_id)
    {
        const auto iterator = control_map.find(message_id);

        if (iterator != control_map.end())
        {
            float value;
            stream >> value;
            iterator->second(value);
        }
    }
}

// ─────────────────────────────────────
static t_int *process(t_int *w)
{
    sl_spectralTilde *x = (sl_spectralTilde *)(w[1]);
    t_sample *in_left = (t_sample *)(w[2]);
    t_sample *in_right = (t_sample *)(w[3]);
    t_sample *out_left = (t_sample *)(w[4]);
    t_sample *out_right = (t_sample *)(w[5]);
    int block_size = (int)(w[6]);

    x->input[0].resize(block_size);
    x->input[1].resize(block_size);
    x->output[0].resize(block_size);
    x->output[1].resize(block_size);

    // x->sl_spectral->plot[2048];

    for(int i = 0; i < block_size; i++)
    {
        x->input[0][i] = in_left[i];
        x->input[1][i] = in_right[i];
    }

    

    //DO YOUR PROCESSING HERE
    x->sl_spectral->process(x->input, x->output);

    // static int plotFrame = 0;
    for(int i = 0; i < block_size; i++)
    {
        out_left[i] = x->output[0][i];
        // x->sl_spectral->plot[plotFrame] = x->output[0][i];
        // plotFrame++;
        out_right[i] = x->output[1][i];
    }

    // if(plotFrame >= 2048)
    // {
    //     int checl = plotFrame;
    //     plotFrame = 0;
    //     //something else
    // }

    // for(int i = 0; i < 2048; i++)
    // {
    //     x->sl_spectral->plot[i] = x->sl_spectral->get_ringbuf_L()[i];
    // }

    on_bang(x);

    return (w + 7);
}

// ─────────────────────────────────────
static void sl_spectral_tilde_dsp(sl_spectralTilde *x, t_signal **sp)
{
    x->sample_rate = sp[0]->s_sr;
    x->sl_spectral->set_sample_rate(x->sample_rate);
    dsp_add(process, 6, x,
        sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);
}

// ─────────────────────────────────────
static void *sl_spectral_tilde_new(void)
{
    sl_spectralTilde *x = (sl_spectralTilde *)pd_new(sl_spectral_tilde_class);
    x->sl_spectral = new Sl_spectral();

    x->inlet_left = inlet_new(&x->object, &x->object.ob_pd, &s_signal, &s_signal);
    x->inlet_right = inlet_new(&x->object, &x->object.ob_pd, &s_signal, &s_signal);
    x->outlet_left = outlet_new(&x->object, &s_signal);
    x->outlet_right = outlet_new(&x->object, &s_signal);
    x->outlet_info = outlet_new(&x->object, gensym("list"));

    return x;
}

// ─────────────────────────────────────
static void sl_spectral_tilde_free(sl_spectralTilde *x)
{
    inlet_free(x->inlet_left);
    inlet_free(x->inlet_right);
    outlet_free(x->outlet_left);
    outlet_free(x->outlet_right);
    outlet_free(x->outlet_info);
}

// ─────────────────────────────────────
static void control_input(sl_spectralTilde *x, t_symbol *s, int argc, t_atom *argv)
{
    std::string control_messages;

    for(int i = 0; i < argc; i++)
    {
        if(argv[i].a_type == A_FLOAT)
            control_messages += std::to_string(atom_getfloat(&argv[i]));
        else if(argv[i].a_type == A_SYMBOL)
            control_messages += std::string(atom_getsymbol(&argv[i])->s_name);
        if(i < argc - 1)
            control_messages += " ";
    }
    interpret_control(x, control_messages);
}

// ─────────────────────────────────────
extern "C" void sl_spectral_tilde_setup(void)
{
    sl_spectral_tilde_class = class_new(gensym("sl_spectral~"),
                                    (t_newmethod)sl_spectral_tilde_new,
                                    (t_method)sl_spectral_tilde_free,
                                    sizeof(sl_spectralTilde),
                                    CLASS_DEFAULT,
                                    A_NULL);

    class_addmethod(sl_spectral_tilde_class,
                        (t_method)sl_spectral_tilde_dsp, gensym("dsp"), A_CANT, 0);

    class_addmethod(sl_spectral_tilde_class,
                        (t_method)control_input, gensym("control"), A_GIMME, 0);

    class_addmethod(sl_spectral_tilde_class,
                        (t_method)print_info, gensym("info"), A_NULL);

    class_addbang(sl_spectral_tilde_class,
                        (t_method)on_bang);
}
