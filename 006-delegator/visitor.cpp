#include <iostream>
#include <exception>

struct Data;
struct Image8;
struct Image16;

struct Task {
    virtual int runner(Image8&) {
        throw std::runtime_error("Not available for type Image8");
    }

    virtual int runner(Image16&) {
        throw std::runtime_error("Not available for type Image16");
    }
};

struct Data {
    virtual int run(Task& v) = 0;
};

struct Image8 : Data {
    
    Image8() = default;

    int run(Task& v) override {
        return v.runner(*this);
    }
};

struct Image16 : Data {

    Image16() = default;

    int run(Task& v) override {
        return v.runner(*this);
    }
};


struct ThresholdTask : Task {

    float t_value;

    ThresholdTask(float t) : t_value(t) {

    }

    static Image8 threshold(Image8& c, uint8_t t) {
        std::cout << 'b' << std::endl;
        return Image8();
    }

    static Image8 threshold(Image16& c, uint16_t t) {
        std::cout << 'c' << std::endl;
        return Image8();
    }

    int runner(Image8& c) override {
        
        if (t_value > 255)
            throw std::runtime_error("Threshold value out of range");
        if (t_value < 0)
            throw std::runtime_error("Threshold value out of range");
        
        uint8_t t = static_cast<uint8_t>(t_value);
        threshold(c, t);

        return 1;
    }

    int runner(Image16& c) override {
        
        if (t_value > 65535)
            throw std::runtime_error("Threshold value out of range");
        if (t_value < 0)
            throw std::runtime_error("Threshold value out of range");
        
        u_int16_t t = static_cast<uint16_t>(t_value);
        threshold(c, t);

        return 1;
    }

    void run(Data* r) {
        r->run(*this);
    }
};

struct MaximumTask : Task {

    MaximumTask() = default;

    static uint8_t maximum(Image8& c) {
        std::cout << 'm_b' << std::endl;
        return 125;
    }

    int runner(Image8& c) override {
        uint8_t m = maximum(c);
        return 1;
    }

    void run(Data* r) {
        r->run(*this);
    }
};

int main(int argc, char* argv[], char* env[]) {

    Image8 c1;
    Image16 c2;

    ThresholdTask v(31.0f);

    Data* r2 = &c1;
    Data* r3 = &c2;

    c1.run(v);
    c2.run(v);

    std::cout << "==============" << std::endl;

    r2->run(v);
    r3->run(v);

    std::cout << "==============" << std::endl;

    v.run(r2);
    v.run(r3);

    return 0;
}