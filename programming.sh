~/programs/cgdb_build/cgdb/cgdb -d arm-none-eabi-gdb src/blackmagic.elf -ex "target remote localhost:3333" -ex "mon reset halt" -ex "load" -ex "mon reset halt"
