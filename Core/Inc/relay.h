#include <stdint.h>

class Relay{
    private:
        char* chr;
    public:
        Relay(char* c);
        bool State(uint16_t pin);
        void Process();
        void TurnOn(uint16_t pin);
        void TurnOff(uint16_t pin);
};