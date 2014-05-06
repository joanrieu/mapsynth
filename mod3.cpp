#include <cmath>
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
        { "test-vco-trigger", 1 },
        { "test-vco-shape", 2 },
        { "test-lfo-rate", 1 },
        { "test-lfo-base", 440 },
        { "test-lfo-amount", 20 },
        { "test-vcf-cutoff", 0.1 },

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
        { "master-input", "vcf-output" },

        // VCO
        { "vco-trigger", "test-vco-trigger" },
        { "vco-pitch", "lfo-output" },
        { "vco-shape", "test-vco-shape" },

        // LFO
        { "lfo-rate", "test-lfo-rate" },
        { "lfo-base", "test-lfo-base" },
        { "lfo-amount", "test-lfo-amount" },

        // VCF
        { "vcf-input", "vco-output" },
        { "vcf-cutoff", "test-vcf-cutoff" },
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
            // Shape
            const bool enable = outputs.at(inputs.at("vco-trigger"));
            const float sine = std::sin(phase * 2 * M_PI);
            const float triangle = std::abs(4 * phase - 2) - 1;
            const float saw = 1 - 2 * phase;
            const float square = 1 - 2 * (phase < .5f);
            const float shape = outputs.at(inputs.at("vco-shape"));
            float output;
            if (shape >= 0 and shape <= 3) {
                if (shape < 1)
                    output = (1 - shape) * sine + shape * triangle;
                else if (shape < 2)
                    output = (2 - shape) * triangle + (shape - 1) * saw;
                else
                    output = (3 - shape) * saw + (shape - 2) * square;
            }
            outputs.at("vco-output") = enable * output;
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
            // Modulation
            const float mod = 4 * (phase < .5f ? phase : (1 - phase)) - 1;
            const float output =
                outputs.at(inputs.at("lfo-base"))
                + mod * outputs.at(inputs.at("lfo-amount"));
            outputs.at("lfo-output") = output;
        },

        // VCF
        [&]() {
            static float output;
            const float alpha = outputs.at(inputs.at("vcf-cutoff"));
            output = alpha * outputs.at(inputs.at("vcf-input")) + (1 - alpha) * output;
            outputs.at("vcf-output") = output;
        }

    };

    // Global update loop
    while (outputs.at("master-time") < 5)
        for (auto& update: updates)
            update();

}
