src=$(wildcard src/*.cpp)

server:
	g++ -std=c++11 -pthread -g \
	$(src) \
	server.cpp \
	-o server
client:
	g++ src/errif.cpp src/Buffer.cpp src/Socket.cpp client.cpp -o client
single_client:
	g++ -std=c++11 -pthread -g \
	$(src) \
	single_client.cpp \
	-o single_client
th:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest
test:
	g++ src/errif.cpp src/Buffer.cpp src/Socket.cpp src/ThreadPool.cpp \
	-pthread \
	test.cpp -o test
clean:
	rm server && rm single_client && rm client && rm test 