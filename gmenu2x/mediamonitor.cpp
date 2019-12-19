#ifdef ENABLE_INOTIFY
#include <sys/inotify.h>
#include <SDL/SDL.h>
#include <unistd.h>

#include "debug.h"
#include "inputmanager.h"
#include "mediamonitor.h"
#include "utilities.h"

MediaMonitor::MediaMonitor(std::string dir) :
	Monitor(dir, IN_MOVE | IN_DELETE | IN_CREATE | IN_ONLYDIR)
{
}

bool MediaMonitor::event_accepted(
			struct inotify_event &event __attribute__((unused)))
{
	return true;
}

void MediaMonitor::inject_event(bool is_add, const char *path)
{
	/* Sleep for a bit, to ensure that the media will be mounted
	 * on the mountpoint before we start looking for OPKs */
	sleep(1);

	if (is_add)
		inject_user_event(OPEN_PACKAGES_FROM_DIR, strdup(path));
	else
		inject_user_event(REMOVE_LINKS, strdup(path));
}

#endif /* ENABLE_INOTIFY */
