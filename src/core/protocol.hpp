
namespace rtdb {

enum class message_type : uint8_t {
  NA,
  SEND_DATA
};

struct header {
  uint16_t size;
  message_type type;
};


} // namespace rtdb