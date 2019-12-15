#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#include "cpu.h"
#include "debug.h"

#define SYSFS_CPUFREQ_DIR "/sys/devices/system/cpu/cpu0/cpufreq"
#define SYSFS_CPUFREQ_MAX SYSFS_CPUFREQ_DIR "/scaling_max_freq"
#define SYSFS_CPUFREQ_SET SYSFS_CPUFREQ_DIR "/scaling_setspeed"

void writeStringToFile(const char *path, const char *content)
{
	int fd = open(path, O_RDWR);
	if (fd == -1) {
		WARNING("Failed to open '%s': %s\n", path, strerror(errno));
	} else {
		ssize_t written = write(fd, content, strlen(content));
		if (written == -1) {
			WARNING("Error writing '%s': %s\n", path, strerror(errno));
		}
		close(fd);
	}
}

void jz_cpuspeed(unsigned clockspeed)
{
	char freq[10];
	sprintf(freq, "%d", clockspeed * 1000);
	writeStringToFile(SYSFS_CPUFREQ_MAX, freq);
	writeStringToFile(SYSFS_CPUFREQ_SET, freq);
}
