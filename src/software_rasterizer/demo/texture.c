#include "software_rasterizer/precompiled.h"

#include "font.h"

#include "software_rasterizer/vector.h"

#pragma warning(push)
#pragma warning(disable : 4255) /* no function prototype given : converting '()' to '(void)' */
#pragma warning(disable : 4365) /* signed/unsigned mismatch */
#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>
#pragma warning(pop)

struct texture
{
	uint32_t *buf;
	struct vec2_int size;
};

struct texture *texture_create(const char *file_name)
{
	assert(file_name && "texture_create: file_name is NULL");

	struct texture *texture = new struct texture;

	int n;
	unsigned char *data = stbi_load(file_name, &texture->size.x, &texture->size.y, &n, 3);
	if (!data)
	{
		delete texture;
		return NULL;
	}

	texture->buf = new uint32_t[(size_t)(texture->size.x * texture->size.y)];
	for (int i = 0, j = 0; i < (texture->size.x * texture->size.y); ++i, j += 3)
		texture->buf[i] = (uint32_t)((data[j] << 16) | (data[j + 1] << 8) | data[j + 2]);

	stbi_image_free(data);

	return texture;
}

void texture_destroy(struct texture **texture)
{
	assert(texture && "texture_destroy: texture is NULL");
	assert(*texture && "texture_destroy: *texture is NULL");

	delete[] (*texture)->buf;
	delete *texture;
	*texture = NULL;
}

void texture_get_info(struct texture *texture, uint32_t **buf, struct vec2_int **size)
{
	assert(texture && "texture_get_info: texture is NULL");
	assert(buf && "texture_get_info: buf is NULL");
	assert(size && "texture_get_info: size is NULL");

	*buf = texture->buf;
	*size = &(texture->size);
}
