#ifndef SWITCH_EXPORT_H
#define SWITCH_EXPORT_H

#include "thirdparty/libnx/nro.h"
#include "thirdparty/libnx/nacp.h"
#include <cstring>
#include <string>

unsigned char *read_file(const char *fn, size_t *len_out);
unsigned char *read_bytes(const char *fn, size_t off, size_t len);
size_t write_bytes(const char *fn, size_t off, size_t len, const unsigned char *data);

void register_switch_exporter();
void register_switch_exporter_types();

#endif // SWITCH_EXPORT_H