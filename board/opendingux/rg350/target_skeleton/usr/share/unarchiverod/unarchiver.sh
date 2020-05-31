#!/bin/sh
clear
readKey() {  # no arguments
    echo;echo
    printf '\e[1;36m%-6s\e[m' "Press START to continue..."
    oldstty=`stty -g`
    stty -icanon -echo min 1 time 0
    dd bs=1 count=1 >/dev/null 2>&1
    stty "$oldstty"
    echo
}
finish() {  # no arguments
    readKey
    exit
}
checkCmd() {  # $1 == command
    which=$(which "$1")
    if [ -z "$which" ]; then
        printf '\e[0;31m%-6s\e[m' "$1: command not found"
        finish
    fi
}
unarchive() {  # $1 == file ; $2 == decompress command (string)
    if eval $2; then
        echo
        printf '\e[0;32m%-6s\e[m' "$(basename "$1"): successfully decompressed"
    else
        echo
        printf '\e[0;31m%-6s\e[m' "$(basename "$1"): decompression failed"
    fi
}
ungzip() {  # $1 == file ; $2 == directory
    regexSize=$(expr "$1" : '^.*.tar.gz$')
    totalSize=$(expr length "$1")
    if [ "$regexSize" = "$totalSize" ]; then
        checkCmd tar
        unarchive "$1" "tar -xzf \"$1\" -C \"$2\" --overwrite"
    else
        file=$(echo $1 | sed 's/\.gz$//')
        checkCmd gunzip        
        unarchive "$1" "gunzip -cf \"$1\" > \"$2/$file\""
    fi
}
byExtension() {  # $1 == file ; $2 == directory
    ext=$(echo $1 | sed 's/^.*\.//')
    case $ext in
        "zip")
            checkCmd unzip
            unarchive "$1" "unzip -od \"$2\" \"$1\""
            ;;
        "gz")
            ungzip "$1" "$2"
            ;;
        "tar")
            checkCmd tar
            unarchive "$1" "tar -xzf \"$1\" -C \"$2\" --overwrite"
            ;;
        "rar")
            checkCmd unrar
            unarchive "$1" "unrar x -o+ \"$1\" \"$2\""
            ;;
        "7z")
            checkCmd 7zr
            unarchive "$1" "7zr x -aoa -o\"$2\" \"$1\""
            ;;
        *)
            printf '\e[0;31m%-6s\e[m' "$(basename "$1"): extension not recognized"
            ;;
    esac
}
# MAIN BLOCK
if [ ! -f "$1" ]; then 
    printf '\e[0;31m%-6s\e[m' "$(basename "$1"): file does not exists"
    finish
fi
dir=$(dirname "$1")
mime=$(file -bi "$1" | cut -d';' -f1)
case $mime in
    "application/zip" | "application/x-zip-compressed")
        checkCmd unzip
        unarchive "$1" "unzip -od \"$dir\" \"$1\""
        ;;
    "application/gzip" | "application/x-gzip")
        ungzip "$1" "$dir"
        ;;
    "application/tar" | "application/x-tar")
        checkCmd tar
        unarchive "$1" "tar -xf \"$1\" -C \"$dir\" --overwrite"
        ;;
    "application/tar+gzip")
        checkCmd tar
        unarchive "$1" "tar -xzf \"$1\" -C \"$dir\" --overwrite"
        ;;
    "application/x-rar" | "application/x-rar-compressed")
        checkCmd unrar
        unarchive "$1" "unrar x -o+ \"$1\" \"$dir\""
        ;;
    "application/x-7z-compressed")
        checkCmd 7zr
        unarchive "$1" "7zr x -aoa -o\"$dir\" \"$1\""
        ;;
    "application/octet-stream")
        byExtension "$1" "$dir"
        ;;
    *)
        printf '\e[0;31m%-6s\e[m' "$mime: MIME not recognized"
        ;;
esac
finish
