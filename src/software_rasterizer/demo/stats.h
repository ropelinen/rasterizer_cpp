#ifndef RPLNN_STATS_H
#define RPLNN_STATS_H

enum stat_types
{
	STAT_FRAME = 0,
	STAT_BLIT,
	STAT_RASTER,
	STAT_COUNT
};

class stats
{
public:
	stats(const unsigned char stat_count, const unsigned int frames_in_buffer, const bool profiling_run);
	~stats();

	bool is_profiling_complete() const;
	/* Call this only once per stat between stat_frame_complete calls.
	* Calling this multiple times for a single stat overrides the previous value. */
	void update_stat(const unsigned char stat_id, const uint32_t time);
	void frame_complete();

	uint32_t get_stat_prev_frame(const unsigned char stat_id) const;
	/* Returns stats for the previous frame */
	uint32_t get_stat_percentile(const unsigned char stat_id, const float percentile) const;
	uint32_t get_avarage(const unsigned char stat_id) const;
private:
	/* Should have a vector container etc. and have this there */
	void update_sorted(uint32_t *data, const unsigned int data_size, const uint32_t new_val, const uint32_t prev_val);

	/* When using microseconds uint32_t can store 71 minutes,
	* uint16_would be enough for 65 milliseconds.
	* Could store the time in 1/10 milliseconds or something like,
	* it should be ok for any normal frame time but not worth the trouble right now. */
	uint32_t *stats_arr;
	/* When ever a stat is updated the value is also added to this sorted array and the old value is removed.
	* This allows efficient checking of percentile values. However this doubles memory consumption. */
	uint32_t *stats_sorted;
	/* The sum of all the stats, used for fast calculation of avarages. */
	uint64_t *sums;
	/* We use the stats array like a circular buffer
	* this is the index we are currently using. */
	unsigned int current_index;
	unsigned int frames_in_buffer;
	unsigned char stat_count;
	/* If set to true the stats are collected only until the buffer gets full.
	* Ie the index will not loop (nor is the last stat overwritten). */
	bool profiling_run;
};


#endif /* RPLNN_STATS_H */
