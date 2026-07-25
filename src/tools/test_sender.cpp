#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../core/protocol.hpp"

alignas(64) char send_buffer[64*1024] = {0};

void prepare_message() {
  rtdb::header* header = reinterpret_cast<rtdb::header*>(&send_buffer);
  header->size = 60 * 1024;
  header->type = rtdb::message_type::SEND_DATA;

}

int main() {
  // Create socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    std::cerr << "Failed to create socket" << std::endl;
    return 1;
  }

  // Set up server address
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect to server
  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    std::cerr << "Failed to connect to server" << std::endl;
    close(sock);
    return 1;
  }

  // Send data
  std::string message = "Hello Server";
  send(sock, message.c_str(), message.length(), 0);
  std::cout << "Sent: " << message << std::endl;

  // Receive response
  char buffer[1024] = {0};
  int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
  if (bytes_received > 0) {
    std::cout << "Received: " << buffer << std::endl;
  } else {
    std::cout << "No data received" << std::endl;
  }

  // Close socket
  close(sock);
  return 0;
}