#include <iostream>

class VoxelsCanvas {
protected:
    VoxelsCanvas() = default;
};

template <typename T>
class _VoxelsCanvas : public VoxelsCanvas {
protected:
    T* data = nullptr;

    _VoxelsCanvas() { data = new T[16]; }

    ~_VoxelsCanvas() { delete[] data; }

public:

    virtual void set(size_t l, size_t c, size_t s, size_t ch, size_t fr, T val) {
        data[4] = val;
    }
};

struct rgba {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t a;
};

class VoxelsCanvasRGBa : public _VoxelsCanvas<rgba> {
    
};

class VoxelsCanvasU8 : public _VoxelsCanvas<uint8_t> {

};

class MaskCanvas : public _VoxelsCanvas<uint8_t> {
public:

    enum class Partition {
        FOREGROUND,
        BACKGROUND
    };

public:

    void set(size_t l, size_t c, size_t s, size_t ch, size_t fr, Partition p) {
        data[4] = (p == Partition::FOREGROUND) ? MaskCanvas::foreground : MaskCanvas::background;
    }

public:

    static uint8_t foreground;
    static uint8_t background;
};

uint8_t MaskCanvas::foreground = 255;
uint8_t MaskCanvas::background = 0;

class VoxelsCanvasU16 : public _VoxelsCanvas<uint16_t> {
    
};

class VoxelsCanvasFloat : public _VoxelsCanvas<float> {
    
};

class LabelsCanvas : public _VoxelsCanvas<uint64_t> {
    
};

class ThresholdTask {

};

int main(int argc, char* argv[], char* env[]) {
    return 0;
}