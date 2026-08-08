#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

char recv_buffer[32*1024] = {0};

unsigned uring_entries = 1024;
unsigned uring_flags = IORING_SETUP_SINGLE_ISSUER;

uint16_t server_port = 8088;

struct io_uring_user_data {
  int fd;
  enum operation_type { NA, ACCEPT, SEND, RECV } operation_type;
};

void io_accept(io_uring *ring, int server_fd) {
  io_uring_user_data *userdata = new io_uring_user_data(); // todo leak
  userdata->operation_type = io_uring_user_data::ACCEPT;
  io_uring_sqe *sqe = io_uring_get_sqe(ring);
  io_uring_prep_accept(sqe, server_fd, NULL, NULL, 0);
  io_uring_sqe_set_data(sqe, userdata);
  io_uring_submit(ring);
}

void recv(io_uring *ring, int client_fd, char *buffer, size_t buffer_size) {
  io_uring_user_data *userdata = new io_uring_user_data();  // todo leak
  userdata->operation_type = io_uring_user_data::RECV;
  io_uring_sqe *sqe = io_uring_get_sqe(ring);
  io_uring_prep_recv(sqe, client_fd, buffer, buffer_size, 0);
  io_uring_sqe_set_data(sqe, userdata);
  io_uring_submit(ring);
}

void send(io_uring *ring, int client_fd, const char *buffer,
          size_t buffer_size) {
  io_uring_sqe *sqe = io_uring_get_sqe(ring);
  io_uring_prep_send(sqe, client_fd, buffer, buffer_size, 0);
  io_uring_submit(ring);
}

int main() {

  struct io_uring ring;
  io_uring_queue_init(uring_entries, &ring, uring_flags);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  // Set socket to non-blocking
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(server_port);

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return 1;
  }

  // Listen for connections
  if (listen(sock, 10) < 0) {
    perror("listen");
    return 1;
  }

  std::cout << "Server listening on port 8080" << std::endl;

  // Accept connections
  struct io_uring_sqe *sqe;
  struct io_uring_cqe *cqe;
  int client_fd = 0;

  io_accept(&ring, sock);

  while (true) {
    io_uring_wait_cqe(&ring, &cqe);
    io_uring_user_data *cqe_data = (io_uring_user_data *)io_uring_cqe_get_data(cqe);

    switch (cqe_data->operation_type) {
    case io_uring_user_data::ACCEPT:
      std::cout << "io_uring_user_data::ACCEPT\n";
      client_fd = cqe->res;
      if (client_fd < 0) {
        continue;
      }
      recv(&ring, client_fd, &recv_buffer[0], 32*1024);
      break;

    case io_uring_user_data::RECV:
      // case recv, get data, stg, send
      std::cout << "io_uring_user_data::RECV\n";
      break;

    case io_uring_user_data::SEND:
      std::cout << "io_uring_user_data::SEND\n";
      // case send, get send size from cqe res, advance pointers and try again
      // sending, if all sent -> spawn receive sqe
      break;
    default:
      std::cout << "default\n";
      break;
    }

    io_uring_cqe_seen(&ring, cqe);

    /*
    // Send response
    const char* response = "main.cpp";
    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_send(sqe, client_fd, response, strlen(response), 0);
    io_uring_submit(&ring);

    io_uring_wait_cqe(&ring, &cqe);
    io_uring_cqe_seen(&ring, cqe);

    close(client_fd);*/
  }

  close(sock);
  io_uring_queue_exit(&ring);
  return 0;
}
