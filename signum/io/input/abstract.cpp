#include "abstract.h"

namespace signum
{
    AudioIn_Abstract *shared_in = NULL;
    
    AudioIn_Abstract::AudioIn_Abstract()
    {
        if (shared_in)
            throw std::runtime_error("Multiple AudioIn units are not yet supported.");
        
        shared_in = this;
        
        this->name = "audioin";
        this->channels_out = 2;
    }
        
}
