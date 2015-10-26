connect mb mdm
set executable "[lindex $argv 0]"
dow -data image.mfs 0xA5000000
dow $executable
debugconfig -reset_on_run system enable
run
disconnect 0
exit
