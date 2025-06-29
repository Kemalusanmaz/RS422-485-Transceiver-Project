Linux Drive Installation

1)	Find driver with USB_to_IO/FTDI/Linux x86_64 path file.
2)	tar -xvf /USB_to_IO/FTDI/Linux x86_64 /ftdi_sio.tar.tar => Extract ftdi_sio.tar.tar file.
3)	cd ftdi_sio=> enter the file that is extracted. 
4)	lsusb => find output 
Bus 003 Device 002: ID 0403:6011 Future Technology Devices International, Ltd FT4232H Quad HS USB-UART/FIFO IC
•	VID (Vendor ID): 0403 → FTDI
•	PID (Product ID): 6011 → FT4232H
•	FT4232H: FTDU USB Device with Quad High Speed UART Port
5)	sudo modprobe ftdi_sio => load ftdi_sio driver which is the name of the standard kernel module for FTDI USB-to-Serial converters.
6)	echo 0403 6011 | sudo tee /sys/bus/usb-serial/drivers/ftdi_sio/new_id => This command manually tells the ftdi_sio driver to "take ownership of this new VID and PID combination." This is often necessary for new or custom FTDI devices that the driver doesn't support by default.
•	echo 0403 6011: Writes the text "0403 6011" to standard output.
•	| (pipe): Redirects the output of the echo command to the input of the next command.
•	sudo tee .../new_id: The tee command reads from standard input and writes it both to the screen and to a specified file.
7)	sudo dmesg | tail -n 20 => Check the load of the driver is successful with kernel message that contains last 20 line.
8)	ls /dev/ttyUSB* => Lists all files in the /dev directory that start with ttyUSB.
/dev/ttyUSB0  /dev/ttyUSB1  /dev/ttyUSB2  /dev/ttyUSB3
9)	echo "options ftdi_sio vendor=0x0403 product=0x6011" | sudo tee /etc/modprobe.d/ftdi.conf => This command makes the manual binding process from step 3 permanent.
•	This command creates a file named ftdi.conf and writes the line options ftdi_sio vendor=0x0403 product=0x6011 into it.
•	When the computer is restarted, the modprobe system will read this file and automatically instruct the ftdi_sio driver to support the 0403:6011 VID/PID combination when it loads. This eliminates the need to repeat step 3 every time.


