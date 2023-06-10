src=$(wildcard src/*.cpp)
echo_server:
	g++ -std=c++11 -pthread -g \
	$(src) \
	-D OS_LINUX \
	echo_server.cpp \
	-o echo_server
echo_client:
	g++ -std=c++11 -pthread -g \
	$(src) \
	-D OS_LINUX \
	echo_client.cpp \
	-o echo_client
echo_clients:
	g++ -std=c++11 -pthread -g \
	$(src) \
	-D OS_LINUX \
	echo_clients.cpp \
	-o echo_clients
clean:
	rm echo_server && rm echo_client && rm echo_clients