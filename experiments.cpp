#include "distribution_sweep.h"
#include <random>
#include <fstream>

int max = INT_MAX;

// std::default_random_engine generator; //this will always produce the same sequence, unless we change the seed

std::random_device rand_dev; //these will produce different sequences everytime
std::mt19937 generator(rand_dev());
const int start = 0;
std::uniform_int_distribution<int> uniform_distribution(start,(const int)max);


void generate_different_pair(int *y1, int *y2){
  int yy = uniform_distribution(generator);
  int xx = uniform_distribution(generator);
  while(yy == xx){
    xx = uniform_distribution(generator);
  }
  *y1 = yy;
  *y2 = xx;
}

void populate_verticals_uniform(array_buffer* line_buffer, int vertical_lines){
  const int maxx = max;

  //uniform distribution
  std::uniform_int_distribution<int> vertical_x_distribution(start,maxx); //params (a,b) lower and upper bound
  //uniform distribution

  int i = 0;
  while(i < vertical_lines){
    int x = vertical_x_distribution(generator);
    int y1;
    int y2;
    generate_different_pair(&y1,&y2);
    line l;
    l.x1 = x;
    l.x2 = x;
    if (y1 < y2){
    l.y1 = y1;
    l.y2 = y2;
    }else{
      l.y1 = y2;
      l.y2 = y1;
    }
    line_buffer->insert_element(&l);
    i++;
  }
}

void populate_verticals_binomial(array_buffer* line_buffer, int vertical_lines){
  //binomial distribution
    double p = 0.5;
    std::binomial_distribution<int> vertical_x_distribution(max,p); //params (n,p) trials,probability of success
  //binomial distribution

  int i = 0;
  while(i < vertical_lines){
    int x = vertical_x_distribution(generator);
    int y1;
    int y2;
    generate_different_pair(&y1,&y2);
    line l;
    l.x1 = x;
    l.x2 = x;
    if (y1 < y2){
    l.y1 = y1;
    l.y2 = y2;
    }else{
      l.y1 = y2;
      l.y2 = y1;
    }
    line_buffer->insert_element(&l);
    i++;
  }
}

void generate_sample_file(std::string filename, int size, double alfa, int distribution_type){
  int generation_reads = 0;
  int generation_writes = 0;
  int B_lines = 512.0/(4*DATA_TYPE_SIZE);
  array_buffer *line_buffer = new array_buffer(1,1,B_lines,filename, &generation_reads, &generation_writes);
  int lines = pow(2,size);
  int horizontal_lines = (int)lines*alfa;
  int vertical_lines = lines-horizontal_lines;

  if (distribution_type == 0){
    populate_verticals_uniform(line_buffer, vertical_lines);
  }else{
    populate_verticals_binomial(line_buffer, vertical_lines);
  }

  int i = 0;
  while(i < horizontal_lines){
    int x1;
    int x2;
    generate_different_pair(&x1,&x2);
    int y = uniform_distribution(generator);
    line l;
    l.y1 = y;
    l.y2 = y;
    if (x1 < x2){
      l.x1 = x1;
      l.x2 = x2;
    }else{
      l.x1 = x2;
      l.x2 = x1;
    }
    line_buffer->insert_element(&l);
    i++;
  }
  line_buffer->dump_remaining_items();
  line_buffer->delete_buffer();
  line_buffer->close_file();
  delete line_buffer;
}

double meaningful_memory(int size){
  if (size < 13){ //Enough memory for 2-branching
    return 32768.0;
  }
  if(size < 16){//Enough memory for 4-branching
    return 57344.0;
  }
  int lines = pow(2,size);
  double memory = lines*16.0;
  return memory/10.0;
}

void reset_counters(unsigned long long* average_measurements, double* average_estimates){
  int j = 0;
  while(j < 8){
    average_measurements[j] = 0;
    average_estimates[j] = 0.0;
    j++;
  }
  while(j < 11){
    average_estimates[j] = 0.0;
    j++;
  }
}

int determine_tentative_slabs(int blocksize, int memory){
int i = 2;
while( (3*i + 2)*blocksize < memory ){ //We need 1 block for the intersection buffer, 1 block for the sweep line buffer and 3 blocks per slab (active list, replacement active list and Y_i list)
i++;
}
return i;
}

int main(){

  struct timespec time_start, time_end;
  struct timespec elapsed_time_start, elapsed_time_end;
  double alfa = 0.25;

  int min_size = 9;
  int size = 0;
  int max_size = 10;
  int branching = 2;

  int distribution_type = 0;

  double blocksize = 4096.0;
  double memory = 25165824.0;

  int iteration = 0;
  int number_of_iterations_per_setting = 5;

  unsigned long long *average_measurements = new unsigned long long[8];
  double *average_estimates = new double[11];
  double average_time = 0.0;
  unsigned long long average_intersections = 0;

  std::ofstream out("RAW_DATA.csv");
  std::ofstream avg_data("AVERAGE_DATA.csv");
  out << "Branching , Blocksize , Memory, Iteration , Alfa , Size , Dist_type , X_read , X_write , X_time, X_nlogn , X_c , Y_read , Y_write , Y_time, Y_nlogn , Y_c , DS_read , DS_write , DS_IO , DS_time , DS_nlogn , DS_c , I_read, I_write , I_TB , I_c , MERGESORT_READ , MERGESORT_WRITE , MERGESORT_TIME , TOTAL_TIME , Intersections" << std::endl;
  avg_data << "Branching , Blocksize , Memory , Alfa , Size , Dist_type , X_read , X_write , X_time, X_nlogn , X_c , Y_read , Y_write , Y_time, Y_nlogn , Y_c , DS_read , DS_write , DS_IO, DS_time , DS_nlogn , DS_c , I_read, I_write , I_TB , I_c , MERGESORT_READ , MERGESORT_WRITE , MERGESORT_TIME , TOTAL_TIME , Intersections" << std::endl;
  clock_gettime(CLOCK_MONOTONIC, &elapsed_time_start);
  while(alfa < 0.8){
    size = min_size;
    while(size < max_size+1){
      distribution_type = 0;
      memory = meaningful_memory(size);
      branching = determine_tentative_slabs(blocksize,memory);
      while(distribution_type < 2){
        iteration = 0;
        std::cout << "SIZE:" << size << " ALFA:" << alfa << " DIST:" << distribution_type << std::endl;
        reset_counters(average_measurements, average_estimates);
        average_time = 0.0;
        average_intersections = 0;
        while(iteration < number_of_iterations_per_setting){
	std::cout << "	GENERATING DATA" << std::endl;
        std::string file = "SOURCEFILE";
        unsigned long long *measurements = new unsigned long long[8];
        double *estimates = new double[11];

	clock_gettime(CLOCK_MONOTONIC, &time_start);
        generate_sample_file(file, size, alfa, distribution_type);
	clock_gettime(CLOCK_MONOTONIC, &time_end);
	double generation_time = operation_time_in_seconds(operation_time_in_nanos(time_start,time_end));
	std::cout << "	DONE GENERATING, took: " << generation_time << std::endl;
	std::cout << "	STARTING OSI-DS" << std::endl;
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        //arguments are source name, memory size in kb, blocksize in kb, general prints enabled (1) or disabled (0),
        //intersection reporting enabled (1) or disabled (0) and progress prints enabled (1) or disabled (0)
        unsigned long long intersections = orthogonal_segment_intersection(file,memory,blocksize, 0, 0, 0, measurements, estimates, branching);
        clock_gettime(CLOCK_MONOTONIC, &time_end);

        double total_time = operation_time_in_seconds(operation_time_in_nanos(time_start,time_end));
	std::cout << "	Iteration "<< iteration << " done, took: " << total_time << std::endl;
        out  << branching << "," << blocksize << "," << memory << "," << iteration << "," << alfa << "," << size << "," << distribution_type << "," << measurements[0] << "," << measurements[1] << "," << estimates[0] << "," << estimates[1] << "," << estimates[2] << "," << measurements[2] << "," << measurements[3] << "," << estimates[3] << "," << estimates[4] << "," << estimates[5] << "," << measurements[4] << "," << measurements[5] << "," << measurements[4] + measurements[5] << "," << estimates[6] << "," << estimates[7] << "," << estimates[8] << "," << measurements[6] << "," << measurements[7] << "," << estimates[9] << "," << estimates[10] << "," << measurements[0] + measurements[2] << "," << measurements[1] + measurements[3] << "," << estimates[0] + estimates[3] << "," << total_time << "," << intersections << std::endl;
        iteration++;
        int j = 0;
        while (j < 8){
          average_measurements[j] = average_measurements[j] + measurements[j];
          average_estimates[j] = average_estimates[j] + estimates[j];
          j++;
        }
        while(j < 11){
          average_estimates[j] = average_estimates[j] + estimates[j];
          j++;
        }
        average_time = average_time + total_time;
        average_intersections = average_intersections + intersections;
        }
        avg_data  << branching << "," << blocksize << "," << memory  << "," << alfa << "," << size << "," << distribution_type << "," << average_measurements[0]/number_of_iterations_per_setting << "," << average_measurements[1]/number_of_iterations_per_setting << "," << ((double)average_estimates[0])/number_of_iterations_per_setting << "," << ((double)average_estimates[1])/number_of_iterations_per_setting << "," << ((double)average_estimates[2])/number_of_iterations_per_setting << "," << average_measurements[2]/number_of_iterations_per_setting << "," << average_measurements[3]/number_of_iterations_per_setting << "," << ((double)average_estimates[3])/number_of_iterations_per_setting << "," << ((double)average_estimates[4])/number_of_iterations_per_setting << "," << ((double)average_estimates[5])/number_of_iterations_per_setting << "," << average_measurements[4]/number_of_iterations_per_setting << "," << average_measurements[5]/number_of_iterations_per_setting << "," << (average_measurements[4]+average_measurements[5])/number_of_iterations_per_setting << "," << ((double)average_estimates[6])/number_of_iterations_per_setting << "," << ((double)average_estimates[7])/number_of_iterations_per_setting << "," << ((double)average_estimates[8])/number_of_iterations_per_setting << "," << average_measurements[6]/number_of_iterations_per_setting << "," << average_measurements[7]/number_of_iterations_per_setting << "," << ((double)average_estimates[9])/number_of_iterations_per_setting << "," << ((double)average_estimates[10])/number_of_iterations_per_setting << "," << (average_measurements[0]+average_measurements[2])/number_of_iterations_per_setting  << "," << ((double)average_measurements[1])/number_of_iterations_per_setting + ((double)average_measurements[3])/number_of_iterations_per_setting << "," << ((double)average_estimates[0])/number_of_iterations_per_setting + ((double)average_estimates[3])/number_of_iterations_per_setting << "," << ((double)average_time)/number_of_iterations_per_setting << "," << average_intersections/number_of_iterations_per_setting << std::endl;
        distribution_type++;
      }
      size++;
    }
    alfa = alfa + 0.25;
  }
  out.close();
  avg_data.close();
  clock_gettime(CLOCK_MONOTONIC, &elapsed_time_end);
  double elapsed_time = operation_time_in_seconds(operation_time_in_nanos(elapsed_time_start,elapsed_time_end));
  std::cout << "elapsed time: " << elapsed_time << "s = " << elapsed_time/60.0 << "m" << std::endl;

  //IF WE WANNA PRINT THE INTERSECTIONS
  // int print_intersections = 0;
  // if (print_intersections==1){
  //     std::cout << "PRINTING INTERSECTIONS:" << std::endl;
  //     std::string file = "INTERSECTIONS";
  //     int a = 0;
  //     array_buffer* intersection_buffer = new array_buffer(0,0,B_ints,file,&a,&a);
  //     intersection_buffer->count_elements_in_disk();
  //     while(intersection_buffer->number_of_stored_elements() > 0){
  //       int *x =  (int *)intersection_buffer->retrieve_element();
  //       int *y =  (int *)intersection_buffer->retrieve_element();
  //       std::cout << "(" << *x << ","<<*y<<")" << std::endl;
  //     }
  //   }

  return 0;
}
