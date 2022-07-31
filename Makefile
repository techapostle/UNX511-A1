FILE1=./src/IntMon.cpp
FILE2=./src/NetMon.cpp

NetMon: $(FILE2)
	g++ $(FILE2) -o $@

NetMon: $(FILE1)
	g++ $(FILE1) -o $@

all: NetMon IntMon

clean:
	rm NetMon IntMon
