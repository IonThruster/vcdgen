#include "vcdgen.h"
#include <iostream>

using namespace std;

int main()
{

    // Single instance which generates the VCD
    VcdGen::VcdGenerator gen("test_001.vcd");

    // Hierachy contains modules
    VcdGen::Module Logic {"Logic"};
    VcdGen::Module SubMod {"SubMod"};

    // Modules can be heirarchical
    Logic.add_submodule( &SubMod ) ;

    // Connect things up
    gen.set_top_level_module( &Logic );
    
    // Create Signals : format Signal var{Name, Width};
    VcdGen::Signal data  { "data"  , 8  };
    VcdGen::Signal valid { "valid" , 1  };
    VcdGen::Signal wr_en { "wr_en" , 1  };
    VcdGen::Signal dummy { "dummy" , 16 };

    // Add Signals to the module / submodule
    SubMod.add_signal( {&data, &valid, &wr_en} );
    Logic.add_signal( &dummy );

    // Set values for the signals
    data.value_at(  {{0, 0}, {100, 85}, {110, 0}, {150, 0}} );
    valid.value_at( {{0, 0}, {100, 1} , {110, 0}, {150, 0}} );
    wr_en.value_at( {{0, 0}, {100, 1} , {110, 0}, {150, 0}} );
    dummy.value_at( {{0, 0}, {100, 11}, {110, 0}, {150, 0}} );
    
    gen();
}
