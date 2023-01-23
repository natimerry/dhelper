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
// incompatibilities


#include <stdint.h>
#include <stdio.h>
#include "colors.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <blkid/blkid.h>
#include <linux/fs.h>
#include "partition.h"

/** Calls badblocks command provided by e2fsprogs
 * 
 * @param block device to provice to badblocks
 *
 * @return void
 */
void call_badblocks(char *block_device){
  char blk_dv[20];
  char command[50]= "/sbin/badblocks -sb 2048 ";
  sprintf(blk_dv,"%s",block_device);
  strcat(command, blk_dv);
  system(command);
}

/**
 * Uses libblkid to get details about partition.
 * Due to lack of documentation about libblkid 
 * https://github.com/util-linux/util-linux/blob/master/libblkid/src/partitions/partitions.c 
 * was used to guess and use for our code below.
 *
 * @param 
 *    block_device which has the disk we are to probe
 *    is_nvme which is for checking if the disk is a NVME one or not
 */
void probe_disk(char* block_device,int is_nvme){ 
  // Probes a block device given its path / filename
  blkid_probe pr = blkid_new_probe_from_filename(block_device);
  if (!pr){
    SET_COLOR_RED;
    err(69,"Block device %s could not be opened",block_device);
    RESET_COLOR;
  }

  blkid_partlist part_list; // Creates a struct for partition list
  part_list = blkid_probe_get_partitions(pr); // Probe block device for partitions

  int number_of_part = blkid_partlist_numof_partitions(part_list); // Lists number of partition available on disk
  printf("Number of partitions: ");
  SET_COLOR_YELLOW;
  printf("%d",number_of_part);
  RESET_COLOR;
 
  // Calls kernel to get file_descriptor, or the block device in this case
  int file_descriptor = open(block_device,O_RDONLY);
  uint_least64_t size_uint;
  ioctl(file_descriptor,BLKGETSIZE64,&size_uint);
  double size = size_uint / 1024 / 1024 / 1024;

  printf("\nSize of block device is %f GiB",size); // Converts bytes to GiB


  const char *uuid;
  const char *label;
  const char *type;

  for(int i =0;i < number_of_part;i++){
      printf("\n-------------------------\n");
      char dev_name[20];
      /*
       * Append block_device followed by a name. Adds a extra 'p' 
       * in case of a NVME drive to make sure formatting is correct.
       */

      if (!is_nvme) sprintf(dev_name, "%s%d", block_device, (i+1)); 
      else sprintf(dev_name,"%s%s%d",block_device,"p",i+1);
    
      pr = blkid_new_probe_from_filename(dev_name);
      blkid_do_probe(pr); 

      blkid_probe_lookup_value(pr, "UUID", &uuid, NULL); // probe UUID
      blkid_probe_lookup_value(pr, "TYPE", &type, NULL); // probe disk type i.e NTFS, ext4, btrfs

      printf("\nParition Name=");
      SET_COLOR_GREEN;
      printf("%s",dev_name);
      RESET_COLOR;
      
      printf("\nUUID=");
      SET_COLOR_YELLOW;
      printf("%s",uuid);
      RESET_COLOR;
      
      printf("\nType=");
      SET_COLOR_RED;
      printf("%s",type);
      RESET_COLOR;

      sleep(1);
  
      call_badblocks(dev_name); // Call badblocks on a per partition basis.

  }
  blkid_free_probe(pr); // Free the block device.

}


