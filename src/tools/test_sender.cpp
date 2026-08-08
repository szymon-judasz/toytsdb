#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "../core/protocol.hpp"

alignas(64) char send_buffer[32*1024] = {0};

rtdb::core::header* prepare_message() {
  std::cout << "building header";
  rtdb::core::header* header = reinterpret_cast<rtdb::core::header*>(&send_buffer);
  header->size = 32 * 1024;
  header->type = rtdb::core::message_type::SEND_DATA;
  std::cout << "building senddata";

  rtdb::core::send_data_request* send_data_request = reinterpret_cast<rtdb::core::send_data_request*>(&send_buffer + sizeof(rtdb::core::header));
  
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  
  send_data_request->timestamp = timestamp_ms;
  send_data_request->stream_id = 2;
  return header;
}

int main() {
  // Create socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    std::cerr << "Failed to create socket" << std::endl;
    return 1;
  }
  std::cout << "Socker created\n";

  // Set up server address
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8088);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect to server
  while (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    std::cerr << "Failed to connect to server" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  std::cout << "connected\n";

  // Send data
  rtdb::core::header* header = prepare_message();
  std::cout << "sending...\n";
  send(sock, header, header->size, 0);
  std::cout << "Sent message"; 

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