/*
 * This file is part of dhelper.
 *
 * Developed for the XINO Event.
 * This product includes software developed by Soham Nandy @ DPSRPK
 * (soham.nandy2006@gmail.com).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// C code written in CPP file to ensure proper linking of files
// without this compilation was near impossible due to C++ / C linking
// incompatibilities. Code is still compiled with C compiler.

#include "dhelper.h"
#include "partition.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void print_help() {
  printf(
      "Usage:\n --type [nvme/hdd] indicates type of block device\n --device shows the device to be used\n \
      Optionally specify --bench to benchmark the disk the disk current directory is mounted on");
}

int main(int argc, char **argv) {
  int nvme = 0x69420;
  int to_bench;
  char *blk_device;
  char *bench_path;
  char *path_to_bench = "/boot/efi";

  if (argc > 1) {

    int opt;
    while ((opt = getopt(argc, argv, "hvbB:d:")) != -1) {
      switch (opt) {

      case 'v':
        continue;
      case 'h':
        print_help();
        continue;
      case 'b':
        to_bench++;
        continue;
      case 'B':
        bench_path=optarg;
        continue;
      case 'd':
        blk_device=optarg;
        continue;
      }
    }
  } else {
    print_help();
    exit(1);
  }

  probe_disk(blk_device, nvme);
  bench_disk(path_to_bench);
}
