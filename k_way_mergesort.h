#include "utility.h"
extern int DISK_READS;
extern int DISK_WRITES;
extern int verbose;
extern int print_progress;
double k_way_mergesort(std::string source,  std::string destination, double M, double blocksize, int type, std::string offset, unsigned long long* read_output, unsigned long long* write_output, double* time_output, double * estimate_output, double * c_output);
