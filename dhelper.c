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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dhelper.h"

void print_help(){
  printf("Usage:\n --type [nvme/hdd] indicates type of block device\n --device shows the device to be used\n \
      Optionally specify --bench to benchmark the disk the disk current directory is mounted on");
}

int main(int argc, char **argv){
  int nvme = 0x69420;
  int to_bench;
  char *blk_device;
  char *bench_path;
  char *path_to_bench="/boot/efi";
  //loops thru argument list to get it.
  for (int i =0;i < argc; i++){
    int check_type_var = strcmp(argv[i],"--type");
    if (check_type_var == 0){
      if (strcmp(argv[i+1],"hdd") == 0){
        nvme=0;
      }
      else nvme=1;
    }
    else if (strcmp("--device",argv[i]) == 0){
      blk_device=argv[i+1];
    }
    else if (strcmp("--help",argv[i]) ==0){
      print_help();
    }
    
    else if (strcmp("--bench-path",argv[i])==0){
      path_to_bench=argv[i+1];
    }
  }



  probe_disk(blk_device,nvme);
  bench_disk(path_to_bench);

  
}

