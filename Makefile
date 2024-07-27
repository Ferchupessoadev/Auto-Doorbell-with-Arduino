# Configuration 
BOARD_TAG = arduino:avr:uno
PORT = /dev/ttyACM0
CORE = arduino:avr
FQBN = $(BOARD_TAG)
TARGET = ./main/main

# Actions
compile:
	arduino-cli compile --fqbn $(FQBN) $(TARGET).ino

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(TARGET).ino

clean:
	rm -rf ./main/*.hex && rm -rf ./main/*.elf
