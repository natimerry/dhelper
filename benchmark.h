#include <string>
double read_files(std::string file);
double ret_cpu_time(struct timespec begin, struct timespec end);
double write_files(std::string file,int size);
void bench_disk();
