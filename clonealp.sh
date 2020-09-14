#!/bin/bash
mkdir -p alpine
wget http://dl-cdn.alpinelinux.org/alpine/v3.11/releases/x86_64/alpine-minirootfs-3.11.3-x86_64.tar.gz -O alpine/alp.tar.gz
tar -xf alpine/alp.tar.gz -C ./alpine/
