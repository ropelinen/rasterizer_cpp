#include "software_rasterizer/precompiled.h"

#include "stats.h"

#include <string.h>

stats::stats(const unsigned char stat_count, const unsigned int frames_in_buffer, const bool profiling_run)
	: stats_arr(NULL)
	, stats_sorted(NULL)
{
	stats_arr = new uint32_t[stat_count * frames_in_buffer];
	memset(stats_arr, 0, stat_count * frames_in_buffer * sizeof(uint32_t));
	stats_sorted = new uint32_t[stat_count * frames_in_buffer];
	memset(stats_sorted, 0, stat_count * frames_in_buffer * sizeof(uint32_t));
	sums = new uint64_t[stat_count];
	memset(sums, 0, stat_count * sizeof(uint64_t));

	current_index = 0;
	this->frames_in_buffer = frames_in_buffer;
	this->stat_count = stat_count;
	this->profiling_run = profiling_run;
}

stats::~stats()
{
	delete[] sums;
	delete[] stats_sorted;
	delete[] stats_arr;
}

bool stats::is_profiling_complete() const
{

	return profiling_run && current_index >= frames_in_buffer;
}

void stats::update_sorted(uint32_t *data, const unsigned int data_size, const uint32_t new_val, const uint32_t prev_val)
{
	assert(data && "update_sorted: data is NULL");

	unsigned int index = ~0U;
	for (unsigned i = 0; i < data_size; ++i)
	{
		if (data[i] == prev_val)
		{
			index = i;
			break;
		}
	}

	assert(index != ~0U && "update_sorted: prev_val not found");
	if (index == ~0U)
		return;

	/* Should sort these from smallest to largest instead. (remember to change get percentile func too. */
	if (new_val > prev_val)
	{
		if (index == 0)
		{
			data[index] = new_val;
			return;
		}

		while (index > 0)
		{
			if (data[index - 1] >= new_val)
			{
				data[index] = new_val;
				return;
			}
			else
			{
				data[index] = data[index - 1];
				--index;
				if (index == 0)
				{
					data[0] = new_val;
					return;
				}
			}
		}
	}
	else
	{
		if (index == data_size - 1)
		{
			data[index] = new_val;
			return;
		}

		while (index < data_size - 1)
		{
			if (data[index + 1] <= new_val)
			{
				data[index] = new_val;
				return;
			}
			else
			{
				data[index] = data[index + 1];
				++index;
				if (index == data_size - 1)
				{
					data[index] = new_val;
					return;
				}
			}
		}
	}
}

void stats::update_stat(const unsigned char stat_id, const uint32_t time)
{
	assert(stat_id < stat_count && "stats_update_stat: Too big stat_id");

	if (profiling_run && current_index >= frames_in_buffer)
		return;

	uint32_t prev_value = stats_arr[(stat_id * frames_in_buffer) + current_index];
	/* No change here, don't do anything. */
	if (time == prev_value)
		return;
	
	stats_arr[(stat_id * frames_in_buffer) + current_index] = time;

	sums[stat_id] -= prev_value;
	sums[stat_id] += time;

	update_sorted(&(stats_sorted[stat_id * frames_in_buffer]), frames_in_buffer, time, prev_value);
#if RPLNN_BUILD_TYPE == RPLNN_DEBUG
	uint32_t *data = &(stats_sorted[stat_id * frames_in_buffer]);
	for (unsigned int i = 0; i < frames_in_buffer - 1; ++i)
		assert(data[i] >= data[i + 1] && "sort: stats not properly sorted after running insertion");
#endif
}

void stats::frame_complete()
{
	++current_index;
	if (current_index >= frames_in_buffer)
	{
		if (profiling_run)
			current_index = frames_in_buffer;
		else
			current_index = 0;
	}
}

uint32_t stats::get_stat_prev_frame(const unsigned char stat_id) const
{
	return stats_arr[(stat_id * frames_in_buffer) + (current_index ? current_index : frames_in_buffer) - 1];
}

uint32_t stats::get_stat_percentile(const unsigned char stat_id, const float percentile) const
{
	unsigned int index = (unsigned int)((frames_in_buffer - 1) * (1.0f - (percentile * 0.01f)));
	return stats_sorted[(stat_id * frames_in_buffer) + index];
}

uint32_t stats::get_avarage(const unsigned char stat_id) const
{
	return (uint32_t)(sums[stat_id] / frames_in_buffer);
}
