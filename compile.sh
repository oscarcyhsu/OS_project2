cd ./ksocket
make CONFIG_STACK_VALIDATION=
insmod ksocket.ko
cd ../master_device
make CONFIG_STACK_VALIDATION=
insmod master_device.ko
cd ../slave_device
make CONFIG_STACK_VALIDATION=
insmod slave_device.ko
cd ../user_program
make


