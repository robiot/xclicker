FROM ubuntu:18.04

RUN apt-get update -y
RUN apt-get install -y meson pkg-config gtk+-3.0 libx11-dev libxi-dev libxtst-dev
