
namespace rtdb {
namespace core {

enum class message_type : uint8_t {
  NA,
  SEND_DATA
};

enum class response_status : uint8_t {
  NA,
  OK,
  ERROR
};

struct header {
  uint16_t size;
  message_type type;
};

struct send_data_request {
  uint64_t timestamp;
  uint16_t stream_id;
};

struct send_data_response {
  response_status status;  
};

}
} // namespace rtdb