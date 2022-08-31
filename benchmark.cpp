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


#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <fstream>
#include <algorithm>
#include "benchmark.h"
#include <random>
#include "colors.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
// REQUIRES TO BE BUILT BY C++17
#include <filesystem>

namespace fs = std::filesystem;

#define NANO_SEC_TO_SEC 1e9
#define FILE_SIZE 1000000
#define FILE_NUMBER 100

/**
 * Write files into a directory. Opens file_local and sets memory buffer size to 0
 * to ensure all IO is done directly to the disk instead. This results in more accurate results.
 * 
 * @param
 *      file (std::string) containing name of the file to be written
 *      size (int) containing size (in bytes) of the file ot be written
 *
 * @return 
 *      time required to write N number of files calculated by ret_cpu_time()
*/
double write_files(std::string file,int size){
  double time =0;
  std::string to_write;
  // creates file and marks it to be written directly to disk
  // we set steam buffer to 0 for this
  std::ofstream file_local;
  file_local.rdbuf()->pubsetbuf(0, 0);
  file_local.open(file.c_str());

  if (file_local.is_open()){
    struct timespec begin;
    struct timespec end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&begin); // gets time from High Resolution CPU timer 
    for (int i=0;i < size; i++){
      if (i%1024 == 0){
        // If 1024 bytes have been written add newline character, flush to disk
        // and reset the to_write variable
        to_write+="\n";
        file_local << to_write;
        to_write = "";
      }
      to_write+="0";
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&end);
    time = ret_cpu_time(begin,end);
  }
  return time;
} 
/**
 * Subracts the 2 given time gotten from the HRT in the kernel.
 * Converts seconds to nano seconds to return precise timings.
 *
 * @param 2 structs, the beginning time and the end time.
 *
 * @returns a double value equal to the sumation of time taken in seconds*1e9
 * and the time taken in nano seconds
 *
 *
 */
double ret_cpu_time(struct timespec begin, struct timespec end){
  double seconds = static_cast<double>(end.tv_sec - begin.tv_sec)*NANO_SEC_TO_SEC;
  double nano_seconds = static_cast<double>(end.tv_nsec - begin.tv_nsec);

  return seconds + nano_seconds; // add nano seconds to get very precise time;
}

/**
 * read files from a directory in disk 
 * Write files into a directory. Opens file_local and sets memory buffer size to 0
 * to ensure all IO is done directly to the disk instead. This results in more accurate results.
 *
 * @param a string containing the file name
 *
 * @return time taken to read file from disk
 *
 * Note: We do not use getline for this as this causes higher overhead from userspace
 * to kernel calls. For example lets assume the overhead to be 1ms, just calling it 10 times has
 * gotten us over 10ms time wasted in calling it, resulting in vastlyu inacorrect results
 */

double read_files(std::string filename){
  double time =0;
  std::string storage_var;
  // Reads buffer directly from disk
  std::ifstream file(filename.c_str());
  file.rdbuf()->pubsetbuf(0,0);

  if (file.is_open()){
    struct timespec begin;
    struct timespec end;
    std::string str;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&begin);

    file.seekg(0, std::ios::end);   
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&end);
    // HRT bug on gentoo kernel, returns time negative time, halving and converting to positve
    // to get approx disk speed value;
    time = ret_cpu_time(begin,end);
    
    if (time < 0){
      time = time * -1;
      time = time /2;
    }
    file.close();
  }
  
  return time;
}

/**
 * Read /proc/self/mounts file to get details used to mount a partition
 *
 *
 * @param string containing partition to scan for.
 *
 * @return if nothing is found, call itself to return /dev/root
 *
 * Note: In the current code we do not use this function other than to get details
 * about /dev/root partition as it is the only location supported right now.
 */

std::string read_procmounts(std::string to_search){
  std::ifstream procfile;
  procfile.open("/proc/self/mounts");
  std::string storage_var;
  std::string result;

  while(std::getline(procfile,storage_var)){
    size_t found = storage_var.find(to_search);
    if (found != std::string::npos){
      return storage_var;
    }
  }

  std::cout << "No other partition found at location, assuming /dev/root\n";
  return read_procmounts("/dev/root"); 
}

extern "C" void bench_disk(const char *DIRECTORY){

  std::string DIRNAME_TO_CREATE(DIRECTORY);
  DIRNAME_TO_CREATE.append("/bench_disk/");
  std::cout << "\nChecking speed for partition /dev/root. Mtab mount details given below:\n";
  std::cout << read_procmounts(DIRECTORY) << std::endl;
  int file_arr[FILE_NUMBER]; 
  for (int i =0; i < FILE_NUMBER;i++){
    file_arr[i]=i; // fill up the array;
  }
  
  std::cout << "Creating directory at: " << DIRNAME_TO_CREATE << std::endl;
  fs::create_directory(DIRNAME_TO_CREATE);
  std::cout << "Beginning disk benchmark. Please be patient\n";

  // Shuffle the array, creates random devices using the standard library
  uint64_t seed = 69420;
  std::shuffle(file_arr, file_arr + FILE_NUMBER,std::default_random_engine(seed));

  
  // write the files for testing
  double total_time_write(0);
  double average_time_write;
  for (int i =0;i < FILE_NUMBER; i++){
    std::string file_name_to_write = DIRNAME_TO_CREATE + std::to_string(static_cast<uint_fast64_t>(i)) + ".test";
    total_time_write+= write_files(file_name_to_write,FILE_SIZE);
  }
  average_time_write = total_time_write / FILE_NUMBER / NANO_SEC_TO_SEC;
  
  double total_time_read(0);
  double average_time_read;

  for (int i=0;i < FILE_NUMBER;i++){
    std::string file_name_to_read= DIRNAME_TO_CREATE + std::to_string(static_cast<uint_fast64_t>(i)) + ".test";
    total_time_read+=read_files(file_name_to_read);
  }
  average_time_read = total_time_read/FILE_NUMBER/NANO_SEC_TO_SEC;
  

  std::cout << "Total Write Time (in seconds): ";
  SET_COLOR_YELLOW;
  std::cout << total_time_write / NANO_SEC_TO_SEC << std::endl << std::endl;
  RESET_COLOR;

  std::cout << "Average time to write files of 10000000 bytes: ";
  SET_COLOR_YELLOW;
  std::cout << average_time_write << std::endl << std::endl;
  RESET_COLOR;


  std::cout << "Total Read Time (in seconds): ";
  SET_COLOR_YELLOW;
  std::cout << total_time_read / NANO_SEC_TO_SEC << std::endl << std::endl;
  RESET_COLOR;

  std::cout << "Average time to read files of 10000000 bytes: ";
  SET_COLOR_YELLOW;
  std::cout << average_time_read << std::endl << std::endl;
  RESET_COLOR;
  
  double write_rate=FILE_SIZE/average_time_write/1000000; // convert to mbps
  double read_rate=FILE_SIZE/average_time_read/1000000;
  
  std::cout << "Write/Read rate: " << std::endl;
  SET_COLOR_YELLOW;
  std::cout << write_rate << " / " << read_rate << std::endl;
  RESET_COLOR;
  
  std::cout << "\n Benchmark finished, proceeding to check partitions\n";
}
