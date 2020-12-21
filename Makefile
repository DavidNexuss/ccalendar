calendar: calendar.cc
	g++ -O3 calendar.cc -o calendar
clean: calendar
	rm calendar
install: calendar
	sudo cp calendar /bin/calendar
