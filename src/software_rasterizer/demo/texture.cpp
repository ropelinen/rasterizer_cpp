#include "software_rasterizer/precompiled.h"

#include "texture.h"

#include "software_rasterizer/vector.h"

#pragma warning(push)
#pragma warning(disable : 4255) /* no function prototype given : converting '()' to '(void)' */
#pragma warning(disable : 4365) /* signed/unsigned mismatch */
#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>
#pragma warning(pop)

texture::texture(const char *file_name)
	: buffer(NULL)
{
	assert(file_name && "texture_create: file_name is NULL");
	
	int n;
	unsigned char *data = stbi_load(file_name, &size.x, &size.y, &n, 3);
	if (!data)
	{
		return;
	}

	buffer = new uint32_t[(size_t)(size.x * size.y)];
	for (int i = 0, j = 0; i < (size.x * size.y); ++i, j += 3)
		buffer[i] = (uint32_t)((data[j] << 16) | (data[j + 1] << 8) | data[j + 2]);

	stbi_image_free(data);
}

texture::~texture()
{
	delete[] buffer;
}

bool texture::is_valid() const
{
	return buffer != NULL;
}

void texture::get_info(uint32_t **buf, struct vec2_int **texture_size)
{
	assert(buf && "texture_get_info: buf is NULL");
	assert(texture_size && "texture_get_info: texture_size is NULL");

	*buf = buffer;
	*texture_size = &(size);
}
