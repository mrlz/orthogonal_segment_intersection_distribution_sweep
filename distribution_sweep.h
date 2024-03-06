#include "k_way_mergesort.h"
extern int DISK_READS_SWEEP;
extern int DISK_WRITES_SWEEP;
extern int DISK_READS_INTERSECTIONS;
extern int DISK_WRITES_INTERSECTIONS;
std::string generate_X_file(std::string source, int B_lines, int B_ints);
int choose_xi(int x_i[], std::string source, int B_ints, int indexes[], int start_point, int end_point, int slabs, std::string offset);
unsigned long long orthogonal_segment_intersection(std::string source, double memory, double blocksize, int prints_on, int intersections_on, int progress_on, unsigned long long* measurements, double *estimates, int tentative_slabs);
