server:
	g++ server.cpp -o server && g++ client.cpp -o client
clean:
	rm srever && rm client