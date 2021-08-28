/*
##################################################################################################
VCG Waveform Generation library

Copyright (c) 2021 Pradeep Ramani

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

##################################################################################################
*/

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <fstream>
#include <algorithm>
#include <bitset>
#include <cassert>

#pragma once
//////////////////////////////////////////////////////////////////////////////////////////////////
namespace VcdGen
{
//////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

// Forward Declaration
// Denotes the signal we intend to plot
class Signal;

// Stores the Tuple (Time, value, Signal*)
class TickData{

    using Type = vector<tuple<uint64_t, uint32_t, const Signal*>>;
 
    Type tick_data_;

public:
    TickData() = default;

    void append(tuple<uint64_t, uint32_t, const Signal*> data){
        tick_data_.push_back(data);
    }

    void append(tuple<uint64_t, uint32_t> data, const Signal* signal){
        auto tmp = make_tuple(get<0>(data), get<1>(data), signal);
        this->append(tmp);
    }

    void append(vector<tuple<uint64_t, uint32_t>> data, const Signal* signal){
        for( auto& d : data ){
            this->append(d, signal);
        }
    }

    Type& get_data(){
        return tick_data_;
    }
};

// Singleton to get the tick data
static TickData& get_tick_data(){
    static TickData tick_data{};
    return tick_data;
}

// Generates a unique encoded string using a integer
// Every digit in the integer represents one of these characters
string encode_signal_name(uint32_t index){
    vector<char> symbols_ = {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    string ret_val;
    if( index == 0 ){
        ret_val = symbols_[0];
    } else {
        while( index > 0 ){
            int digit = index % 10;
            ret_val += symbols_[digit];
            index /= 10;
        }
    }
    return ret_val;
}

// Represents the entity we wish to plot / dump as VCD
class Signal{

    // Name of the signal
    string name_;

    // Bit width
    uint32_t width_;

    // Encoded name (needed during VCD generation)
    string encoded_name_;

    // Static count of all signals
    inline static int signal_count_ = 0;

public:
    
    // Ctors
    Signal() = delete;

    Signal(string name, uint32_t width) 
    : name_(name), width_(width) {  

        encoded_name_ =  encode_signal_name(signal_count_++);
        #ifdef VCDGEN_DEBUG
            cout << "Init Signal " << name_ << " Encoded Name " << encoded_name_ << endl;
        #endif
    }

    // Inserting into tick-data
    void value_at( int64_t time, int32_t value){
       this->value_at(make_tuple(time, value));
    }

    // Inserting into tick-data
    void value_at( tuple<uint64_t, uint32_t> tick_data ){
        TickData& data = get_tick_data();
        data.append(tick_data, this);
    }

    // Inserting into tick-data
    void value_at( vector<tuple<uint64_t, uint32_t>> tick_data ){
        TickData& data = get_tick_data();
        data.append(tick_data, this);
    }

    // Getters and Setters
    uint32_t get_width() const{ return width_; }

    string get_name() const{ return name_; }

    string get_encoded_name() const{ return encoded_name_; }

    void set_encoded_name(string name){
        encoded_name_ = name;
    }
};

// Modules help in organizing signals into groups
// Modules can hold other modules as sub-modules
class Module {

    // Name of the module
    string name_;

    // Signal pointers it holds
    std::vector<Signal*> signals_;

    // Submodules under this module
    std::vector<Module*> sub_modules_;

public:

    // Ctors.
    Module() = default;

    Module(string name) : name_(name) { }

    // Methods
    void add_submodule(Module* m){
        sub_modules_.push_back(m);
    }

    void add_signal(Signal* s){
        signals_.push_back(s);
    }

    void add_signal(vector<Signal*> vec_s){
        signals_.insert(signals_.end(), vec_s.begin(), vec_s.end());
    }

    // Getters and setters
    uint32_t get_num_submodules(){ return sub_modules_.size(); }

    string get_name() const{ return name_; }

    const vector<Signal*>& get_signals() { return signals_; }

    const vector<Module*>& get_submodules() { return sub_modules_; }
};

// Method to convert integer to binary value
string convert_to_binary(uint32_t value, uint32_t width){

    string sig_val;
    if( width < 64 ){
        sig_val = "b" + std::bitset<64>(value).to_string();
    } else {
        #ifdef VCDGEN_DEBUG
            cout << "Max Supported width = 32b";
        #endif
        assert(false);
    }

    return sig_val;
}

/// Converts from Data Structures to VCD File
class VcdGenerator{

    string filename_;
    Module *top_level_module_;
    string vcd_data_;

    /// VCD Header information
    void header()
    {
        vcd_data_ += R"""($date
    Aug 27, 2021
$end
$version
   VCD generator tool version V0.1
$end
$timescale 1ps $end
)""";
    }

    void generate_module_info(Module* module){
        vcd_data_ += "$scope module " + module->get_name() + " $end\n";
        for( auto signal : module->get_signals() ){
            vcd_data_ += "$var wire " + to_string(signal->get_width()) + " " + 
                         signal->get_encoded_name() + " " + signal->get_name() + 
                         " $end\n";
        }

        //if( module->get_num_submodules() > 0){
        //    vector<Module*> &sub_mods = module->get_submodules();
        //    Module* m = sub_mods[0];
        //    sub_mods.erase(sub_mods.begin());
        //    generate_module_info(m);
        //}

        for( auto m : module->get_submodules() ){
            generate_module_info(m);
        } 
        
        vcd_data_ += "$upscope $end\n";
    }

    void generate_hierarchy_info(){
        generate_module_info(top_level_module_);
    }

    void generate_tick_data(){
        TickData &tick_data = get_tick_data();
        
        auto data = tick_data.get_data();

        sort(data.begin(), data.end());

        uint64_t tick_prev = -1;
        for( auto& d : data ) {

            #ifdef VCDGEN_DEBUG
                cout << "Time : " << get<0>(d) << "\t| Value : " << get<1>(d) << "\t| Name : " 
                     << get<2>(d)->get_name() << "\t| Symbol : " << get<2>(d)->get_encoded_name() 
                     << endl;
            #endif

            uint64_t tick  = get<0>(d); 
            uint32_t value = get<1>(d);
            const Signal* signal  = get<2>(d); 
            uint32_t width = signal->get_width();

            if( tick_prev != tick ){
                vcd_data_ += "#" + to_string(tick) + "\n";
            }
            if( width > 1 ){ 
                vcd_data_ += convert_to_binary(value, width) + " " ;
            } else {
                vcd_data_ += to_string(value);
            }
            vcd_data_ += signal->get_encoded_name() + "\n";
            tick_prev = tick;
        }
    }

public:

    VcdGenerator() = default;
    VcdGenerator(string name) : filename_(name) { }
    
    void set_top_level_module(Module *m){
        top_level_module_ = m;
    }

    void operator()(){
        if( filename_.length() > 0 ) {


            // Time for all the magic :
            // Step 1 : Generate Header
            // Step 3 : Generate Module Hierarchies
            // Step 4 : Generate Tick Data Info for each signal

            header();
            generate_hierarchy_info();
            generate_tick_data();

            std::ofstream file;
            file.open(filename_);
            cout << "Writing To file : " << filename_ << "\n";
            file << vcd_data_;
            file.close();
        } else {
            cout << "ERROR : ILLEGAL VCD FILENAME" << "\n";
        }
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////
} // End namespace VcdGen
//////////////////////////////////////////////////////////////////////////////////////////////////
