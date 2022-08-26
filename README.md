# DHelper
A simple disk utility helper written in C.

## Description
This project was written for the XINO event prompt 3. This uses liblkid library to retrieve details
about a block device.

## Features
* Retrieve details from the block device specified.
* Scan a disk for healthy / unhealthy sectors
* Benchmark the disk the program is running on.

## Prerequisites
This program has minimal dependencies. It depends on `liblkid` which 
is already present in all linux distros. It also depedns on `badblocks` which is a part of `e2fsprogs`
which should also be on your linux distro, if not it can be easily installed using the package
manager of your choice.

**Ubuntu/Debian**  
`apt install e2fsprogs`

**Arch Linux**  
`pacman install e2fsprogs`

**Fedora**  
`dnf install e2fsprogs`

**Gentoo**  
`emerge -aqv e2fsprogs`

## Building from source
As simple as running `make`. Can be installed using `make install`.  
The binary is copied to `/usr/bin/dhelper`  

## Usage
The usage is very simple too, the format is as follows:
* `--type` followed by an argument, this can either be `nvme/hdd`. Indicates type of block device you want to probe.  
* `--device` followed by the disk you want to scan. Example usage `--device /dev/nvme0n1`  

Note: Requires root access.

## Limitations and problems faced
The benchmark heavily relies on STL libraries, this causes a bottleneck scanning disks. It is impossible to give a 1:1 representation of how fast your disk is, instead this reports a speed of how fast an average program can Read/Write files onto your disk.  

## Video Demo  

https://raw.githubusercontent.com/jiro-too/dhelper/master/video_demo/output.mp4
