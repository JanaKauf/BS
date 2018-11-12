
if [ $(lsmod | grep scull -c) -eq 1]; then
	sudo rmmod scull
	echo Old Module Removed
fi

sudo insmod scull.ko scull_nr_devs=2
echo Module inserted with 2 devices
MAJOR_NR=$(grep $*scull /proc/devices | cut -d ' ' -f 1)
echo Major_Nr: $MAJOR_NR
sudo mknod /dev/scull c $MAJOR_NR 0
chmod a+rw /dev/scull
echo Done
