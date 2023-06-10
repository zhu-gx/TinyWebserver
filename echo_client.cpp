#include"./src/include/Connection.h"
#include "./src/include/Socket.h"
#include <iostream>

int main() {
  Socket *sock = new Socket();
  sock->Connect("127.0.0.1", 2023);

  Connection *conn = new Connection(nullptr, sock);

  while (true) {
    conn->GetlineSendBuffer();
    conn->Write();
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
  }

  delete conn;
  return 0;
}
