/*
 * cmp_utils.h
 *
 *  Created on: Jun 3, 2019
 *      Author: mengguang
 */

#ifndef CMP_UTILS_H_
#define CMP_UTILS_H_

#include "misc_utils.h"
#include "cmp.h"

#define CMP_BUFFER_SIZE 255
typedef struct {
	uint8_t data[CMP_BUFFER_SIZE];
	uint32_t position;
} cmp_buffer_t;

bool cmp_buffer_init(cmp_buffer_t *buffer);
bool cmp_simple_init(cmp_ctx_t *ctx, cmp_buffer_t *buffer);
bool cmp_buffer_fill(cmp_buffer_t *buffer, const uint8_t *data, uint32_t length);
bool cmp_buffer_rewind(cmp_buffer_t *buffer);

bool cmp_process_request(cmp_ctx_t *cmp);
#endif /* CMP_UTILS_H_ */
