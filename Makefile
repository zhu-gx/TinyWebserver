server:
	g++ src/errif.cpp server.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Channel.cpp src/EventLoop.cpp src/Server.cpp -o server && \
	 g++ client.cpp src/errif.cpp -o client
clean:
	rm srever && rm client