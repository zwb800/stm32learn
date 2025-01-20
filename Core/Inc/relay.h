#include <stdint.h>

class Relay{
    private:
        char* chr;
    public:
        Relay(char* c);
        bool State(uint8_t pin);
        void Process();
};