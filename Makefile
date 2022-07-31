FILE1=./src/IntMon.cpp
FILE2=./src/NetMon.cpp

networkMon: $(FILE2)
	g++ $(FILE2) -o $@

intfMon: $(FILE1)
	g++ $(FILE1) -o $@

all: NetMon IntMon

clean:
	rm NetMon IntMon
