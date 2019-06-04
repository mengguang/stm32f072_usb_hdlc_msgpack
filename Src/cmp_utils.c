/*
 * cmp_utils.c
 *
 *  Created on: Jun 3, 2019
 *      Author: mengguang
 */

#include "cmp_utils.h"

bool cmp_sum(cmp_ctx_t *cmp, uint32_t msg_id, int32_t a, int32_t b);

bool cmp_buffer_init(cmp_buffer_t *buffer) {
	memset(buffer->data, 0, CMP_BUFFER_SIZE);
	buffer->position = 0;
	return true;
}

bool cmp_buffer_fill(cmp_buffer_t *buffer, const uint8_t *data, uint32_t length) {
	if (length > sizeof(buffer->data)) {
		return false;
	}
	cmp_buffer_init(buffer);
	memcpy(buffer->data, data, length);
	buffer->position = length;
	return true;
}

bool cmp_buffer_rewind(cmp_buffer_t *buffer) {
	buffer->position = 0;
	return true;
}

bool cmp_buffer_reader(cmp_ctx_t *ctx, void *data, size_t limit) {
	cmp_buffer_t *buffer = ctx->buf;
	if (buffer->position + limit > CMP_BUFFER_SIZE) {
		return false;
	}
	memcpy(data, buffer->data + buffer->position, limit);
	buffer->position += limit;
	return true;
}

size_t cmp_buffer_writer(cmp_ctx_t *ctx, const void *data, size_t count) {
	cmp_buffer_t *buffer = ctx->buf;
	if (buffer->position + count > CMP_BUFFER_SIZE) {
		return false;
	}
	memcpy(buffer->data + buffer->position, data, count);
	buffer->position += count;
	return true;
}

bool cmp_simple_init(cmp_ctx_t *ctx, cmp_buffer_t *buffer) {
	if (ctx == NULL || buffer == NULL) {
		return false;
	}
	cmp_init(ctx, buffer, cmp_buffer_reader, NULL, cmp_buffer_writer);
	return true;
}

bool cmp_build_error(cmp_ctx_t *cmp, uint32_t msg_id, const char *message) {
	//[type, msg_id, error, result]
	if (!cmp_write_array(cmp, 4)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	//type == 1, response.
	if (!cmp_write_uinteger(cmp, 1)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}

	if (!cmp_write_uinteger(cmp, msg_id)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}

	if (!cmp_write_str(cmp, message, strlen(message) + 1)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}

	if (!cmp_write_nil(cmp)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	return true;
}

bool cmp_process_request(cmp_ctx_t *cmp) {
	//[type, msg_id, method, params]
	uint32_t array_size = 0;
	if (!cmp_read_array(cmp, &array_size)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	if (array_size != 4) {
		printf("cmp error: %s\n", "Array size was not 4");
		return false;
	}
	uint32_t type = 0;
	if (!cmp_read_uint(cmp, &type)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	if (type != 0) {
		printf("cmp error: %s\n", "type was not 0");
		return false;
	}
	uint32_t msg_id = 0;
	if (!cmp_read_uint(cmp, &msg_id)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	printf("msg_id: %lu\n", msg_id);
	char method[32];
	memset(method, 0, sizeof(method));
	uint32_t length = sizeof(method);
	if (!cmp_read_str(cmp, method, &length)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	printf("method: %s\n", method);

	if (strcmp(method, "sum") == 0) {
		if (!cmp_read_array(cmp, &array_size)) {
			printf("cmp error: %s\n", cmp_strerror(cmp));
			return false;
		}
		if (array_size != 2) {
			printf("cmp error: %s\n", "Array size was not 1");
			return false;
		}
		uint32_t a = 0;
		if (!cmp_read_uint(cmp, &a)) {
			printf("cmp error: %s\n", cmp_strerror(cmp));
			return false;
		}
		uint32_t b = 0;
		if (!cmp_read_uint(cmp, &b)) {
			printf("cmp error: %s\n", cmp_strerror(cmp));
			return false;
		}
		cmp_buffer_init(cmp->buf);
		return cmp_sum(cmp, msg_id, a, b);
	} else {
		printf("method not implemented.\n");
		cmp_buffer_init(cmp->buf);
		cmp_build_error(cmp, msg_id, "method not implemented.");
		return true;
	}
}

int32_t sum(int32_t a, int32_t b) {
	return a + b;
}

bool cmp_sum(cmp_ctx_t *cmp, uint32_t msg_id, int32_t a, int32_t b) {
	int32_t result = sum(a, b);
	//http://rpclib.net/spec/
	//[type, msg_id, error, result]
	if (!cmp_write_array(cmp, 4)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	//type == 0
	if (!cmp_write_uinteger(cmp, 1)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}

	if (!cmp_write_uinteger(cmp, msg_id)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}

	if (!cmp_write_nil(cmp)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}

	if (!cmp_write_array(cmp, 1)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	if (!cmp_write_integer(cmp, result)) {
		printf("cmp error: %s\n", cmp_strerror(cmp));
		return false;
	}
	return true;
}

