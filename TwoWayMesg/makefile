# Copyright 2023 Siegfrij(Justin Siegfried)

cc = g++
#path =..
standard = c++17

flags = -std=$(standard)
flags += -Wall
flags += -pedantic
flags += -g
#flags += -I $(path)

link = $(cc) $(flags) -o
compile = $(cc) $(flags) -c

#Server
TwoWayMesgServer : TwoWayMesgServer.o
	$(link) $@ $^

TwoWayMesgServer.o : TwoWayMesgServer.cc
	$(compile) $< -o $@

#Client
TwoWayMesgClient : TwoWayMesgClient.o
	$(link) $@ $^

TwoWayMesgClient.o : TwoWayMesgClient.cc
	$(compile) $< -o $@

#style
style : TwoWayMesgClient.cc \
				TwoWayMesgServer.cc
	cpplint $^

clean:
	rm -f TwoWayMesgClient TwoWayMesgServer