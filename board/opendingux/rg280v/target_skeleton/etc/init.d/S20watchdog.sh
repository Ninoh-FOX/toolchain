#!/bin/sh

case "$1" in

    start)
        echo "Starting watchdog..."

        # Defaults.
        . /etc/watchdog.conf

        # User overrides.
        if [ -f /usr/local/etc/watchdog.conf ]; then
            . /usr/local/etc/watchdog.conf
        fi

        watchdog -T $WATCHDOG_TIMEOUT -t $WATCHDOG_KICK_RATE /dev/watchdog
        ;;

    stop)
        echo "Stopping watchdog..."

        if ! killall watchdog 2> /dev/null ; then
            echo "Watchdog was not running."
        fi
        ;;

    restart)
        $0 stop
        $0 start
        ;;

    status)
        if pidof watchdog -o $$ > /dev/null ; then
            echo "running"
        else
            echo "stopped"
        fi
        ;;
esac

