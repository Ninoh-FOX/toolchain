#!/bin/sh
case "$1" in
  start)
    /usr/bin/governor.sh
    ;;
  stop)
    exit 1
    ;;
  *)
    exit 1
    ;;
esac

exit 0
