#ifndef __MEDIAMONITOR_H__
#define __MEDIAMONITOR_H__
#ifdef ENABLE_INOTIFY

#include "monitor.h"

class MediaMonitor: public Monitor {
	public:
		MediaMonitor(std::string dir);
		virtual ~MediaMonitor() { };

	private:
		virtual bool event_accepted(struct inotify_event &event);
		virtual void inject_event(bool is_add, const char *path);
};

#endif /* ENABLE_INOTIFY */
#endif /* __MEDIAMONITOR_H__ */
