<h1>How to set up and run modbus_tcp</h1>

<h2>Preparation</h2>

<h3>if not present, install:</h3>

_**sudo apt install build-essential python3-venv**_

<h3>Prepare python environment:</h3>

_**python3 -m venv venv**_
_**venv/bin/pip install "pymodbus==3.6.9"**_

In a separate terminal, run: _**venv/bin/python modbus_sim.py**_
Leave it running.

<h3>Compile source code:</h3>
in project's directory, run: _**make**_


<h2>Running the program</h2>

Usage:

_**./mbread {mode} {ip} {port} {address} {count}**_

modes: read, write

read: Will print and read {count} registers to the console, starting with register {address}.
write: {count} will be used as a value to write in register {address}. Will write the value and print it out the changed register to the console.

ip and port as defined in modbus_sim.py will be 127.0.0.1 and 1502.


<h2>Additional information</h2>

There are 10 registers. Register addressess start at index 0.

When using read, the sum of address and count, cannot be more than 10. To print all 10 registers, use 0 as address, and 10 for count, this will print all 10 existing registers.

For writing to a register, address cannot be less than 0 or more than 9.
Value is a 16 bit unsigned integer. The biggest value you can write to a register is 65535.

<h3>Usage examples:</h3>

_**./mbread read 127.0.0.1 1502 0 10**_

_**./mbread read 127.0.0.1 1502 9 1**_

_**./mbread write 127.0.0.1 1502 9 65535**_



