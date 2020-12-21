ccalendar: ccalendar.cc
	g++ -O3 ccalendar.cc -o ccalendar
clean: ccalendar
	rm ccalendar
install: ccalendar
	sudo cp ccalendar /bin/ccalendar
