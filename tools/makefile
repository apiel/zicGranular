all: list_pulse

list_pulse:
	gcc -o list_pulse list_pulse.c $(pkg-config --cflags --libs libpulse-simple)
	./list_pulse
