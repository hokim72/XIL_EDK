setMode -bscan
setCable -p auto
identify -inferir
identifyMPM
attachflash -position 1 -spi "@FLASH_DEVICE@"
assignfiletoattachedflash -position 1 -file @MCS_FILE@
Program -p 1 -dataWidth 1 -spionly -e -v -loadfpga
quit

