#ifndef RPLNN_FONT_H
#define RPLNN_FONT_H

class font
{
public:
	font(const char *file_name, float line_height);
	~font();

	void set_line_height(float line_height);
	/* Would make more sense to have this somewhere in rasterizing etc */
	void render_text(void *render_target, const struct vec2_int &target_size, const char *text, const struct vec2_int &pos, const uint32_t text_color) const ;
	inline bool is_valid() { return buffer != NULL; };

private:
	float scale;
	int ascent;
	int descent;
	int line_gap;
	void *buffer;
	struct stbtt_fontinfo *font_info;
};

#endif /* RPLNN_FONT_H */
