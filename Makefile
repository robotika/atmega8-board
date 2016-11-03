PRG            = monopost
OPTIMIZE       = -O2
WARNINGS       = -Wall --pedantic

DEFS           =
LIBS           =

# You should not have to change anything below here.

CC             = g++

CFLAGS         = -g $(WARNINGS) $(OPTIMIZE) $(DEFS) -I. -I..
CPPFLAGS       = $(CFLAGS)
LDFLAGS        = 
OBJS           = main.o RealHardware.o Logger.o LogCommLine.o SerialCommLinux.o

all: $(PRG)

clean:
	rm -rf $(PRG)
	rm -rf *.o

$(PRG): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

dep :
	makedepend -Y -- $(CPPFLAGS) -- $(OBJS:.o=.cpp) 2>/dev/null

# DO NOT DELETE

main.o: Logger.h AHardware.h robobios/hwlayer.h Exception.h RealHardware.h
main.o: ACommLine.h
RealHardware.o: RealHardware.h AHardware.h robobios/hwlayer.h Exception.h
RealHardware.o: ACommLine.h SerialCommLinux.h LogCommLine.h
Logger.o: Logger.h AHardware.h robobios/hwlayer.h Exception.h LogCommLine.h
Logger.o: ACommLine.h
LogCommLine.o: LogCommLine.h ACommLine.h Exception.h
SerialCommLinux.o: SerialCommLinux.h ACommLine.h
