FILE1=./src/IntMon.cpp
FILE2=./src/NetMon.cpp

IntMon: $(FILE2)
	g++ $(FILE1) -o $@

NetMon: $(FILE1)
	g++ $(FILE2) -o $@

all: NetMon IntMon

clean:
	rm NetMon IntMon
