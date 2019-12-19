#ifdef ENABLE_INOTIFY
#include "debug.h"

#include <dirent.h>
#include <pthread.h>
#include <SDL.h>
#include <signal.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "inputmanager.h"
#include "monitor.h"
#include "utilities.h"

void Monitor::inject_event(bool is_add, const char *path)
{
	if (is_add)
		inject_user_event(OPEN_PACKAGE, strdup(path));
	else
		inject_user_event(REMOVE_LINKS, strdup(path));
}

bool Monitor::event_accepted(struct inotify_event &event)
{
	/* Don't bother other files than OPKs */
	size_t len = strlen(event.name);
	return len >= 5 && !strncmp(event.name + len - 4, ".opk", 4);
}

int Monitor::run()
{
	int wd, fd;

	DEBUG("Starting inotify thread for path %s...\n", path.c_str());

	fd = inotify_init1(IN_CLOEXEC);
	if (fd < 0) {
		ERROR("Unable to start inotify\n");
		return fd;
	}

	wd = inotify_add_watch(fd, path.c_str(), mask);
	if (wd < 0) {
		ERROR("Unable to add inotify watch\n");
		close(fd);
		return wd;
	}

	DEBUG("Starting watching directory %s\n", path.c_str());

	for (;;) {
		size_t len = sizeof(struct inotify_event) + NAME_MAX + 1;
		struct inotify_event event;
		char buf[256];

		read(fd, &event, len);

		if (event.mask & (IN_DELETE_SELF | IN_MOVE_SELF)) {
			inject_event(false, path.c_str());
			break;
		}

		sprintf(buf, "%s/%s", path.c_str(), event.name);

		if (!event_accepted(event))
			continue;

		inject_event(event.mask & (IN_MOVED_TO | IN_CLOSE_WRITE | IN_CREATE), buf);
	}

	return 0;
}

static void * inotify_thd(void *p)
{
	Monitor *monitor = (Monitor *) p;
	monitor->run();
	return NULL;
}

Monitor::Monitor(std::string path, unsigned int flags) : path(path)
{
	mask = flags;
	pthread_create(&thd, NULL, inotify_thd, (void *) this);
}

Monitor::~Monitor()
{
	pthread_cancel(thd);
	pthread_join(thd, NULL);
	DEBUG("Monitor thread stopped (was watching %s)\n", path.c_str());
}
#endif
