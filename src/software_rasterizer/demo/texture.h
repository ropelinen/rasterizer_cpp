#ifndef RPLNN_TEXTURE_H
#define RPLNN_TEXTURE_H

class texture
{
public:
	texture(const char *file_name);
	~texture();

	bool is_valid() const;
	void get_info(uint32_t **buf, struct vec2_int **texture_size);

private:
	uint32_t *buffer;
	struct vec2_int size;
};

#endif /* RPLNN_TEXTURE_H */
