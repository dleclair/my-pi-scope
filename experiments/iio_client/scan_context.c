#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include "iio.h"

int main() {
	printf("creating scan context.\n");
	struct iio_scan_context *scan_context_p = iio_create_scan_context("local", 0);

	struct iio_context_info **ctx_infos;
	size_t num_infos = iio_scan_context_get_info_list(scan_context_p, &ctx_infos);
	printf("scan info count: %d.\n", num_infos);

	for(int i = 0; i < num_infos; i++) {
		printf("entry: %d\n", i);
		printf("description: %s\n", iio_context_info_get_description(ctx_infos[i]));
		printf("uri: %s\n\n", iio_context_info_get_uri(ctx_infos[i]));
	}

	printf("acquiringe local context...\n");
	struct iio_context *ctx = iio_create_local_context();

	unsigned int devices_count = iio_context_get_devices_count(ctx);
	printf("found: %d devices.\n", devices_count);

	printf("\nDevices:\n");
	char attr_buf[128];
	for (int dev_index = 0; dev_index < devices_count; dev_index++) {
		struct iio_device *iio_dev = iio_context_get_device(ctx, dev_index);
		printf("\n\ndevice ID: %s\n", iio_device_get_id(iio_dev));
		printf("device Name: %s\n", iio_device_get_name(iio_dev));
		printf("device Label: %s\n", iio_device_get_label(iio_dev));
		unsigned int attrs_count = iio_device_get_attrs_count(iio_dev);
		for (int attr_i = 0; attr_i < attrs_count; attr_i++) {
			const char *attr_key = iio_device_get_attr(iio_dev, attr_i);
			iio_device_attr_read(iio_dev, attr_key, attr_buf, 128);
			printf("attr: %s = %s\n", attr_key, attr_buf);
		}

		unsigned int channel_count = iio_device_get_channels_count(iio_dev);
		printf("channel count: %d\n", channel_count);
		printf("Channels:");
		for (int ch_i = 0; ch_i < channel_count; ch_i++) {
			struct iio_channel *iio_ch = iio_device_get_channel(iio_dev, ch_i);
			const char *chid = iio_channel_get_id(iio_ch);
			printf("\nchannel ID: %s\n", chid);
			printf("channel Name: %s\n", iio_channel_get_name(iio_ch));
			unsigned int attrs_count = iio_channel_get_attrs_count(iio_ch);
			for (int attr_i = 0; attr_i < attrs_count; attr_i++) {
				const char *attr_key = iio_channel_get_attr(iio_ch, attr_i);
				iio_channel_attr_read(iio_ch, attr_key, attr_buf, 128);
				printf("attr: %s = %s\n", attr_key, attr_buf);
			}
			if (
					(strcmp(chid, "voltage0") != 0) &&
					(strcmp(chid, "timestamp") != 0) &&
					iio_channel_is_enabled(iio_ch)
			   ) {
				printf("*** disabling unwanted channel. ***\n");
				iio_channel_disable(iio_ch);
			}
		}
	}

	printf("\n\nTesting channel: ads1015,voltage0\n");
	struct iio_device *iiodev = iio_context_find_device(ctx, "ads1015");
	struct iio_channel *iioch = iio_device_find_channel(iiodev, "voltage0", false);
	const struct iio_data_format *fmt = iio_channel_get_data_format(iioch);
	printf("length: %d\n", fmt->length);
	printf("bits: %d\n", fmt->bits);
	printf("shift: %d\n", fmt->shift);
	printf("is_signed: %d\n", fmt->is_signed);
	printf("is_fully_defined: %d\n", fmt->is_fully_defined);
	printf("is_be: %d\n", fmt->is_be);
	printf("with_scale: %d\n", fmt->with_scale);
	printf("scale: %f\n", fmt->scale);
	printf("repeat: %d\n", fmt->repeat);

	printf("Timestamp:\n");
	struct iio_channel *tsch = iio_device_find_channel(iiodev, "timestamp", false);
	const struct iio_data_format *tsfmt = iio_channel_get_data_format(tsch);
	printf("length: %d\n", tsfmt->length);
	printf("bits: %d\n", tsfmt->bits);
	printf("shift: %d\n", tsfmt->shift);
	printf("is_signed: %d\n", tsfmt->is_signed);
	printf("is_fully_defined: %d\n", tsfmt->is_fully_defined);
	printf("is_be: %d\n", tsfmt->is_be);
	printf("with_scale: %d\n", tsfmt->with_scale);
	printf("scale: %f\n", tsfmt->scale);
	printf("repeat: %d\n", tsfmt->repeat);

	printf("enabling voltage0, and timestamp\n");
	printf("before enabling, iioch: %d\n", iio_channel_is_enabled(iioch));
	printf("before enabling, tsch: %d\n", iio_channel_is_enabled(tsch));
	iio_channel_enable(iioch);
	iio_channel_enable(tsch);
	printf("after enabling, iioch: %d\n", iio_channel_is_enabled(iioch));
	printf("after enabling, tsch: %d\n", iio_channel_is_enabled(tsch));
	
	#define SAMPLE_COUNT 15
	printf("\nCreating non-cyclic Rx IIO buffer.\n");
	struct iio_buffer *rxbuf = iio_device_create_buffer(iiodev, SAMPLE_COUNT, false);
	if (!rxbuf) {
		printf("*** ERROR: creating rxbuf, errno=%d\n", errno);
	}

	printf("Starting scan...\n");
	ssize_t nbytes_rx = iio_buffer_refill(rxbuf);
	printf("received %d bytes\n", nbytes_rx);

	char *p_dat, *p_end;
	ptrdiff_t p_inc;
	int64_t now_ts, last_ts;
	last_ts = 0;

	p_inc = iio_buffer_step(rxbuf);
	p_end = iio_buffer_end(rxbuf);

	/* print timetamps */
	printf("          ");
	for (p_dat = iio_buffer_first(rxbuf, tsch); p_dat < p_end; p_dat += p_inc) {
		now_ts = (((int64_t *)p_dat)[0]);
		printf("[%05" PRId64 "] ", last_ts > 0 ? (now_ts - last_ts)/1000 : 0);
		if (!last_ts) {
			last_ts = now_ts;
		}
	}
	printf("\n");
	
	/* print samples */
	size_t sample_size = 2, bytes;
	uint8_t buf[SAMPLE_COUNT * sample_size];
	
	bytes = iio_channel_read(iioch, rxbuf, buf, SAMPLE_COUNT * sample_size);
	printf("%s ", iio_channel_get_id(iioch));
	for (int sample = 0; sample < (bytes / 2); sample++) {
		printf("%7" PRIi16 " ", ((int16_t *)buf)[sample]);
	}
	printf("\n");


	/* clean up */
	printf("\n");
	printf("destroying rx buffer.\n");
	//iio_buffer_destroy(rxbuf);

	printf("destroying local context.\n");
	iio_context_destroy(ctx);

	printf("freeing ctx_infos.\n");
	iio_context_info_list_free(ctx_infos);

	printf("destroying scan context. \n");
       	iio_scan_context_destroy(scan_context_p);
	printf("done.\n");
}
