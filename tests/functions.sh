#!/bin/bash

set -Ee

builddir="$(realpath ../cbuild)"
spp="$(realpath ../cbuild/spp)"
testdir="$(realpath ./)"

show_notice(){
    echo "\033[34m$1\033[0m"
}

show_error(){
  >&2 echo -e "\e[1;31m ERROR: $1 \e[0m"
  exit 1
}

show_warning(){
  echo -e "\e[1;33m⚠️  WARNING: $1 ⚠️\e[0m"
  sleep 2
}

getFileHash(){
    printf "$(md5sum $1 | cut -f1 -d' ')"
}

verifyEqual(){
    if [ "$(getFileHash $1)" = "$(getFileHash $2)" ];then
        return 0
    fi
    return 1
}


getTestArg(){
    printf "$sampleargs" | grep "$1>" | cut -d' ' -f2-
}

test_begin(){
    local test_case="$1"
    local description="$2"
    local test_params="$3"
    if [ -n "$DO_VERBOSE" ];then
        echo
        echo "============================="
        show_notice "Test $test_case"
        echo "============================="
        echo "summary: $description"
        echo
        echo "$spp $test_params $test_case"
        echo
    fi
}


test_ok(){
    if [ -n "$DO_VERBOSE" ]; then
        local foo="$1"
        [ -z "$foo" ] && foo="[OK]"
        echo "\033[1;32m$foo\033[0m"
    fi
}

test_fail()
{
    local foo="$1"
    [ -z "$foo" ] && foo="[FAIL]"
    echo "\033[0;31m$foo\033[0m"
    false
}

test_end(){
    echo "============================"
    echo
}

base(){
    basename $1
}

do_sample_test(){
    local processed="$2"
    local orig="$1"
    if verifyEqual $orig $processed ; then
        test_ok
    else
        echo
        diff -y $orig $processed || true
        echo _________________________________________________
        cat $processed
        test_fail
    fi
}

run_local_sample_test()
{
    local test_file="$builddir/$2"
    $spp $1 $test_file
    do_sample_test "$test_file.spp" "$test_file.ok"
    # rm ${test_file:?} "${test_file:?}.spp"
}