#!/bin/bash

[[ "$(docker images -q cppsparkplug:0.1.0 2> /dev/null)" == "" ]] || docker rmi --no-prune cppsparkplug:0.1.0
docker build . -t cppsparkplug:0.1.0