#!/bin/bash
glances --export zeromq --time 10 -q &
sleep 1
./stat_server.py
kill $(jobs -p)
