project new "My_Project"
project set family "Spartan6"
project set device "xc6slx9"
project set package "csg324"
project set speed "-2"
project set top_level_module_type "HDL"
project set synthesis_tool "XST (VHDL/Verilog)"
project set simulator "ISim (VHDL/Verilog)"
project set "Preferred Language" "VHDL"
project set "Enable Message Filtering" "false"

xfile add "mb_system.xmp"

project save

process run "Generate Top HDL Source" -instance "mb_system"

