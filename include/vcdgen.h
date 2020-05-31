/*
##################################################################################################
VCG Waveform Generation library in c++

Copyright (c) 2019 - 2020, Pradeep Ramani

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

--- Optional exception to the license ---

As an exception, if, as a result of your compiling your source code, portions
of this Software are embedded into a machine-executable object form of such
source code, you may redistribute such embedded portions in such object form
without including the above copyright and permission notices.
##################################################################################################
*/

#pragma once

#include <iostream>
#include <map>
#include <cstdint>
#include <vector>
#include <array>

//////////////////////////////////////////////////////////////////////////////////////////////////
namespace VcdGen
{
//////////////////////////////////////////////////////////////////////////////////////////////////

    class VcdGenerator
    {

        /// Final string output
        std::string final_output;

        /// Software Version
        const float version = 0.1;

        /// Hash table between Signal Names and Symbols used to dump them 
        std::map<std::string, std::string> signal_symbol_map;

        /// Hash table between signal name and their widths
        std::map<std::string, uint16_t> signal_width_map;

        /// character used to form the symbols
        std::array<char, 9> symbol_chars;

        /// index in the symbol table
        short unsigned int st_idx;

        /// Num repetitions needed to create a unique symbol
        size_t st_repeats;

        /// Next symbol to be used
        std::string next_symbol;

        /// current module name (includes the super module names)
        ///
        /// Example :
        /// If the current module hierarchy has the structure :
        /// MAIN module
        /// |-- SUB_COMP_1
        ///     |-- SUB_SUB_COMP_1 
        /// current_module_name = MAIN_SUB_COMP_1_SUB_SUB_COMP_1
        std::string current_module_name;

        // Hierarchy of the current module
        std::vector<std::string> module_hierarchy;

        /// VCD Header information
        void call_header()
        {
            final_output += "$date\n";
            final_output += "   May 25, 2020\n";
            final_output += "$end\n";
            final_output += "$version\n";
            final_output += "   VCD generator tool version " + std::to_string(version) + "\n";
            final_output += "$end\n";
            final_output += "$timescale 1ps $end\n";
        }

        /// Updates what symbol to use for the next Signal
        ///
        /// Ensure that the it will be a unique symbol name
        void update_next_symbol()
        {
            ++st_idx;
            if( st_idx == symbol_chars.size() )
            {
                st_idx = 0;
                ++st_repeats;
            }

            next_symbol = "";

            for( size_t cnt = 0; cnt < st_repeats; cnt++)
            {
                next_symbol += symbol_chars[st_idx];    
            }
        }

        /// Converts the current module name to a string
        ///
        /// This name also includes the parent module names
        /// Example :
        /// If the current module hierarchy has the structure :
        /// MAIN module
        /// |-- SUB_COMP_1
        ///     |-- SUB_SUB_COMP_1 
        /// current_module_name = MAIN_SUB_COMP_1_SUB_SUB_COMP_1
        std::string get_current_module_name()
        {
            current_module_name = "";

            for(const auto &s : module_hierarchy)
            {
                current_module_name += s + "_";
            }

            return current_module_name;
        }

        /// Adds an entry to the signal_symbol_map
        void assign_symbol(std::string signal_name, uint16_t width)
        {
            signal_name = get_current_module_name() + signal_name;

            if ( signal_symbol_map.count(signal_name) == 0 ) {

                signal_symbol_map[signal_name] = next_symbol;
                signal_width_map[signal_name] = width;

                // This makes ready the next symbol to use
                update_next_symbol();
            } else {
                std::cout << "ERROR : Signal already Registerd" << std::endl;
                exit(0);
            }
        }


        public :

        /// Default constructor
        VcdGenerator() 
            : st_idx(0)
            , st_repeats(1)
        {
            call_header();
            symbol_chars = {'!', '@', '#', '$', '%', '^', '&', '*', '\''};
            update_next_symbol();
        }

        /// Adds a module to the current hierarchy (declaration)
        ///
        /// @note : Sub components / wires can be declared only within a module
        /// @param module_name Name of the module
        void start_module(std::string module_name)
        {
            final_output += "$scope module " + module_name + " $end\n";
            module_hierarchy.push_back(module_name);
        }

        /// Signifies end of module declaration
        void end_module()
        {
            final_output += "$upscope $end\n";
            module_hierarchy.pop_back();
        }

        /// Signifies end of all module declarations
        void end_all_module_definitions()
        {
            final_output += "$enddefinitions $end\n";
        }

        /// Adds a signal to the current module
        ///
        /// @param signal_name Name of the signal being added
        /// @param width Width of the signal (#bits) being added
        void add_signal(std::string signal_name, uint16_t width)
        {
            assign_symbol(signal_name, width);
            final_output += "$var wire " + std::to_string(width) + " " \
                            + signal_symbol_map[current_module_name + signal_name]  \
                            +  " " + signal_name + " $end\n";
        }

        /// Dump value of a signal
        ///
        /// @attention : Name of the signal should include names of PARENT MODULES as well.
        /// @param signal_name FULL Name of the signal (including full module path)
        /// @param value Value of the signal in string format. Multi-bit values, should be passed
        /// as a string of binary bits
        /// Example :
        /// If the signal to dump (data_valid) is under the following hierarchy :
        /// MAIN module
        /// |-- SUB_COMP_1
        ///     |-- SUB_SUB_COMP_1 
        ///      |-- data_valid
        /// signal_name = MAIN_SUB_COMP_1_SUB_SUB_COMP_1_data_valid
        /// @warning It is the responsibility of the sender to ensure full path is mentioned
        void dump_signal(std::string signal_name, std::string value)
        {
            if ( signal_symbol_map.count(signal_name) == 0 ) {
                std::cout << "ERROR : Unregisterd Signal name" << std::endl;
                exit(0);
            } else {
                if (signal_width_map[signal_name] == 1 ) {
                    final_output += value + signal_symbol_map[signal_name] + "\n";
                } else {
                    final_output += "b" + value + " " + signal_symbol_map[signal_name] + "\n";
                }
            }
        }

        /// Dumps the time specified in clocks
        void dump_time(size_t clock)
        {
            final_output += "#" + std::to_string(clock) + "\n";
        }

        /// Prints to the console the constructed vcd (until this point)
        void dump_vcd()
        {
            std::cout << final_output;
        }

    };

//////////////////////////////////////////////////////////////////////////////////////////////////
} // End namespace VcdGen
//////////////////////////////////////////////////////////////////////////////////////////////////
