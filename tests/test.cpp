#include "vcdgen.h"
#include "utils.h"

bool test_001()
{
  VcdGen::VcdGenerator v;

  v.start_module("logic");
    v.add_signal("data", 8);
    v.add_signal("data_valid", 1);
    v.add_signal("en", 1);
  v.end_module();

  v.end_all_module_definitions();

  v.dump_time(0);
    v.dump_signal("logic_data", "00000000");
    v.dump_signal("logic_data_valid", "0");
    v.dump_signal("logic_en", "0");
  v.dump_time(100);
    v.dump_signal("logic_data", "01010101");
    v.dump_signal("logic_data_valid", "1");
    v.dump_signal("logic_en", "1");
  v.dump_time(110);
    v.dump_signal("logic_data", "00000000");
    v.dump_signal("logic_data_valid", "0");
    v.dump_signal("logic_en", "0");

  v.set_filename("test_001.vcd");
  v.dump_vcd();
  return compare_files("test_001.vcd", "tests/golden/test_001_golden.vcd");
}

bool test_002()
{
  VcdGen::VcdGenerator v;

  v.start_module("MAIN");
    v.add_signal("data", 8);
    v.add_signal("data_valid", 1);
    v.add_signal("en", 1);

  	v.start_module("SUB_COMP");
    	v.add_signal("data", 8);
    	v.add_signal("data_valid", 1);
    	v.add_signal("en", 1);
  	v.end_module();

  v.end_module();

  v.end_all_module_definitions();

  v.dump_time(0);
    v.dump_signal("MAIN_data", "00000000");
    v.dump_signal("MAIN_data_valid", "0");
    v.dump_signal("MAIN_en", "0");

    v.dump_signal("MAIN_SUB_COMP_data", "00000000");
    v.dump_signal("MAIN_SUB_COMP_data_valid", "0");
    v.dump_signal("MAIN_SUB_COMP_en", "0");
  v.dump_time(100);
    v.dump_signal("MAIN_data", "01010101");
    v.dump_signal("MAIN_data_valid", "1");
    v.dump_signal("MAIN_en", "1");

    v.dump_signal("MAIN_SUB_COMP_data", "11111111");
    v.dump_signal("MAIN_SUB_COMP_data_valid", "1");
    v.dump_signal("MAIN_SUB_COMP_en", "1");
  v.dump_time(110);
    v.dump_signal("MAIN_data", "00000000");
    v.dump_signal("MAIN_data_valid", "0");
    v.dump_signal("MAIN_en", "0");

  v.dump_time(150);
    v.dump_signal("MAIN_SUB_COMP_data", "00000000");
    v.dump_signal("MAIN_SUB_COMP_data_valid", "0");
    v.dump_signal("MAIN_SUB_COMP_en", "0");

  v.set_filename("test_002.vcd");
  v.dump_vcd();
  return compare_files("test_002.vcd", "tests/golden/test_002_golden.vcd");
}

void print_result(std::string test_name, bool result)
{
    if( result) {
        std::cout << "SUCCESS : " << test_name << std::endl;
    } else {
        std::cout << "FAILED : " << test_name << std::endl;
    }
}

int main()
{
    print_result("test_001", test_001());
    print_result("test_002", test_002());
	return 0;
}
