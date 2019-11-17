#!/bin/bash

RG350_HOME="/media/data/local/home"
LAUNCHER="/usr/local/sbin/frontend_start"
RG350_IP="root@10.1.1.2"

# probably don't edit below here

scp=`which scp`
if [ $? -ne 0 ]; then
	echo "This script requires scp"
	exit 1
fi

function makeOpk {
	echo "building the opk"
	rm -f ./output/images/*.opk
	./board/opendingux/gcw0/make_upgrade.sh
	if [ $? -ne 0 ]; then
		echo "making opk failed"
		exit 1
	fi
	return $?
}

function copyOpk {
	echo "doing a quick copy of the opk to the rg-350"
	cmd="scp ./output/images/*.opk ${RG350_IP}:${RG350_HOME}/"
	echo ${cmd}
	${cmd}
	return $?
}

function showHelp {
      echo "Usage:"
      echo "    -h                 Display this help message."
      echo "    -f                 Full clean, build, deploy, enable."
      echo "	-n                 Nuclear. Delete cached downloads as well....."
	  echo "    -q                 Quick, build, deploy binary only."
	  echo "    -c                 deploy last opk"
}

function myMake {
	args="$*"
	cmd="make BR2_EXTERNAL=board/opendingux ${args}"
	echo "make command : ${cmd}"
	${cmd}
	if [ $? -ne 0 ]; then
		echo "make failed"
		exit 1
	fi
}

if [ $# -eq 0 ]; then
	showHelp
	exit 0
fi

while getopts ":hfnqc" opt; do

  case ${opt} in
	h )
		showHelp
		exit 0
      ;;
    f )
		myMake "clean"
		myMake "rg350_defconfig"
		myMake
		makeOpk
		copyOpk
		exit 0
      ;;
	n )
		rm -rf ./dl
		myMake "clean"
		myMake "rg350_defconfig"
		myMake
		makeOpk
		copyOpk
		exit 0
      ;;
    q )
		myMake
		makeOpk
		copyOpk
		exit 0
      ;;
    c )
		makeOpk
		copyOpk
		exit 0
      ;;
    \? )
      showHelp
      ;;
    : )
      echo "Invalid option: $OPTARG requires an argument" 1>&2
	  showHelp
      ;;
  esac
done
shift $((OPTIND -1))
