FILE1=./src/IntMon.cpp
FILE2=./src/NetMon.cpp

interfaceMon: $(FILE2)
	g++ $(FILE1) -o $@

networkMon: $(FILE1)
	g++ $(FILE2) -o $@

all: networkMon interfaceMon

clean:
	rm networkMon interaceMon
