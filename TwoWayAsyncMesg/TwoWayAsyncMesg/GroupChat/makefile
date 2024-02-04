# Copyright 2023 Siegfrij(Justin Siegfried)

cc = g++
#path =..
standard = c++17

flags = -std=$(standard)
flags += -Wall
flags += -pedantic
flags += -g

link = $(cc) $(flags) -o
compile = $(cc) $(flags) -c

#Server
GroupChatServer : GroupChatServer.o
	$(link) $@ $^

GroupChatServer.o : GroupChatServer.cc
	$(compile) $< -o $@

#Client
GroupChatClient : GroupChatClient.o
	$(link) $@ $^

GroupChatClient.o : GroupChatClient.cc
	$(compile) $< -o $@

#style
style : GroupChatClient.cc \
				GroupChatServer.cc
	cpplint $^

clean:
	rm -f GroupChatClient GroupChatServer