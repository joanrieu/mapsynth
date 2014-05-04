#include <functional>
#include <iostream>
#include <map>
#include <vector>

int main() {

    // Module outputs
    std::map<std::string, float> outputs = {

        // Master
        { "master-time", 0 },
        { "master-period", 1.f / 48000 },

        // XXX
        { "test-0", 0 },
        { "test-1", 1 },
        { "test-20", 20 },
        { "test-440", 440 },

        // VCO
        { "vco-output", 0 },

        // LFO
        { "lfo-output", 0 },

        // VCF
        { "vcf-output", 0 },

    };

    // Module inputs (an input comes from an output)
    std::map<std::string, std::string> inputs = {

        // Master
        { "master-input", "vco-output" }, // FIXME

        // VCO
        { "vco-trigger", "test-1" },
        { "vco-pitch", "lfo-output" },

        // LFO
        { "lfo-rate", "test-1" },
        { "lfo-base", "test-440" },
        { "lfo-amount", "test-20" },

        // VCF
        { "vcf-input", "" },
        { "vcf-cutoff", "" },
        { "vcf-resonance", "" },

    };

    // Module update procedures
    std::vector<std::function<void()>> updates = {

        // Master
        [&]() {
            // Clock
            outputs.at("master-time") += outputs.at("master-period");
            // Output
            // - Raw float
            const float& output = outputs.at(inputs.at("master-input"));
            const char* buffer = reinterpret_cast<const char*>(&output);
            for (size_t i = 0; i < sizeof(float); ++i)
                std::cout << buffer[i];
            // - Gnuplot
            //std::cout
                //<< outputs.at("master-time")
                //<< '\t'
                //<< outputs.at(inputs.at("master-input"))
                //<< std::endl;
        },

        // VCO
        [&]() {
            // Phase
            static float phase;
            phase +=
                outputs.at("master-period")
                * outputs.at(inputs.at("vco-pitch"));
            if (phase >= 1)
                phase -= 1;
            // Value
            const bool enable = outputs.at(inputs.at("vco-trigger"));
            const float value = 4 * (phase < .5f ? phase : (1 - phase)) - 1;
            outputs.at("vco-output") = enable * value;
        },

        // LFO
        [&]() {
            // Phase
            static float phase;
            phase +=
                outputs.at("master-period")
                * outputs.at(inputs.at("lfo-rate"));
            if (phase >= 1)
                phase -= 1;
            // Value
            const float mod = 4 * (phase < .5f ? phase : (1 - phase)) - 1;
            const float value =
                outputs.at(inputs.at("lfo-base"))
                + mod * outputs.at(inputs.at("lfo-amount"));
            outputs.at("lfo-output") = value;
        },

        // VCF
        [&]() {
            // TODO
        }

    };

    // Global update loop
    while (outputs.at("master-time") < 5)
        for (auto& update: updates)
            update();

}
