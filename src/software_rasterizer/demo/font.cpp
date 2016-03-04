#include "software_rasterizer/precompiled.h"

#include "font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 4100) /* unreferenced formal parameter */
#pragma warning(disable : 4365) /* signed/unsigned mismatch */
#include <stb/stb_truetype.h>
#pragma warning(pop)

#include <stdio.h>

font::font(const char *file_name, float line_height)
	: buffer(NULL)
	, font_info(NULL)
{
	assert(file_name && "font_create: file_name is NULL");

	FILE *font_file = fopen(file_name, "rb");
	if (!font_file)
		return;

	fseek(font_file, 0, SEEK_END);
	unsigned size = (unsigned)ftell(font_file);
	fseek(font_file, 0, SEEK_SET);

	buffer = malloc(size);
	font_info = new struct stbtt_fontinfo;

	fread(buffer, size, 1, font_file);
	fclose(font_file);

	if (!stbtt_InitFont(font_info, (const unsigned char *)buffer, 0))
	{
		delete font_info;
		font_info = NULL;
		free(buffer);
		buffer = NULL;
		return;
	}

	set_line_height(line_height);
}

font::~font()
{
	if (buffer)
		free(buffer);
	if (font_info)
		delete font_info;
}

void font::set_line_height(float line_height)
{
	scale = stbtt_ScaleForPixelHeight(font_info, line_height);
	stbtt_GetFontVMetrics(font_info, &ascent, &descent, &line_gap);
	ascent = (int)(ascent * scale);
	descent = (int)(descent * scale);
}

void font::render_text(void *render_target, const struct vec2_int &target_size, const char *text, const struct vec2_int &pos, const uint32_t text_color) const
{
	assert(render_target && "font_render_text: render_target is NULL");
	assert(text && "font_render_text: text is NULL");

	int x = 0;
	unsigned int text_len = (unsigned int)strlen(text);
	/* This sorely needs optimization */
	for (unsigned int i = 0; i < text_len; ++i)
	{
		int char_x0, char_x1;
		int char_y0, char_y1;
		stbtt_GetCodepointBitmapBox(font_info, text[i], scale, scale, &char_x0, &char_y0, &char_x1, &char_y1);

		int y = ascent + char_y0;
		int char_width, char_height;
		int x_offset, y_offset;
		/* NOTE: This allocates memory, should do something about that */
		unsigned char *bitmap = stbtt_GetCodepointBitmap(font_info, scale, scale, (int)text[i], &char_width, &char_height, &x_offset, &y_offset);

		for (int char_y = 0; char_y < char_height; ++char_y)
		{
			int line = target_size.y - (pos.y + char_y + y);
			if (line >= target_size.y)
				break;

			for (int char_x = 0; char_x < char_width; ++char_x)
			{
				int pos_x = pos.x + char_x + x;
				if (pos_x >= target_size.x)
					break;

				uint8_t alpha = bitmap[char_y * char_width + char_x];	
				uint32_t *pixel = &((uint32_t*)render_target)[line * target_size.x + pos_x];
				/* This needs to be calculated using a macro or something as this depends on the target format */
				/* Colors of the original pixel */
				uint32_t s_r = (*pixel >> 16) & 0xFF;
				uint32_t s_g = (*pixel >> 8) & 0xFF;
				uint32_t s_b = (*pixel) & 0xFF;
				/* Colors for the text */
				uint32_t t_r = (text_color >> 16) & 0xFF;
				uint32_t t_g = (text_color >> 8) & 0xFF;
				uint32_t t_b = (text_color) & 0xFF;
				/* 255 = opaque, 0 = transparent */
				float alpha_float = (float)(alpha / 255.0f);
				uint32_t n_r = ((uint32_t)(s_r * (1.0f - alpha_float)) + (uint32_t)(t_r * alpha_float)) << 16;
				uint32_t n_g = ((uint32_t)(s_g * (1.0f - alpha_float)) + (uint32_t)(t_g * alpha_float)) << 8;
				uint32_t n_b = ((uint32_t)(s_b * (1.0f - alpha_float)) + (uint32_t)(t_b * alpha_float));
				/* Calculate target pixel */
				uint32_t target_pixel = 0;
				target_pixel |= n_r & 0xFF0000;
				target_pixel |= n_g & 0xFF00;
				target_pixel |= n_b & 0xFF;
				*pixel = target_pixel;
			}
		}

		free(bitmap);

		/* Advance character width */
		int advance_x;
		stbtt_GetCodepointHMetrics(font_info, text[i], &advance_x, 0);
		x = x + (int)(advance_x * scale);

		/* Add kerning */
		int kerning = stbtt_GetCodepointKernAdvance(font_info, text[i], text[i + 1]);
		x = x + (int)(kerning * scale);

		if (x >= target_size.x)
			break;
	}
}
