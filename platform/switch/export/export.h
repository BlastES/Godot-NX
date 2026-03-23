#include "../switch_wrapper.h"
#include <cstring>
#include <string>

unsigned char *read_file(const char *fn, size_t *len_out);
unsigned char *read_bytes(const char *fn, size_t off, size_t len);
size_t write_bytes(const char *fn, size_t off, size_t len, const unsigned char *data);

void register_switch_exporter();