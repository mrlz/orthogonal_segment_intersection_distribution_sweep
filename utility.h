#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <limits.h>
#define BILLION 1000000000L
#define DATA_TYPE_SIZE sizeof(int)

struct line {
  int x1;
  int y1;
  int x2;
  int y2;
} ;

struct min_heap_node{
    void *element;
    int i; //In the k-way merge phase we'll use the min heap to find the smallest element among the k buffers.
          //this int indicates the source file of the element.
};

uint64_t operation_time_in_nanos(struct timespec start, struct timespec end);
double operation_time_in_seconds(uint64_t nanos);
void print_operation_time(struct timespec start, struct timespec end);
void swap(min_heap_node* x, min_heap_node* y);
int file_bytesize(std::string source);
void print_a_line(line l);
void print_all_lines(line lines[], int number_of_lines, int print);
int total_lines_in_file(std::string source);
int read_n_lines(FILE *pFile, void *buffer, int n_lines, int index_buffer, int* count);
int write_n_lines(FILE *pFile, void *buffer, int n_lines, int index_buffer, int* count);
int retrieve_lines(std::string source, line *lines, int* count);
bool less_x(void* vl1, int i,void* vl2, int j);
bool less_y(void* vl1, int i,void* vl2, int j);
void file_stats(std::string file);
bool is_horizontal(line l);
int total_elements_in_file(std::string source);
int read_n_ints(FILE *pFile, void *buffer, int n, int index_buffer, int* count);
int write_n_ints(FILE *pFile, void *buffer, int n, int index_buffer, int* count);
void assign_int(void *vl1, int i, void *value);
void assign_line(void *vl1, int i, void *value);
void assign_ints(void * vl1, int i, void* vl2, int j);
void assign_lines(void * vl1, int i, void* vl2, int j);
void *retrieve_int(void *vl1, int i);
void *retrieve_line(void *vl1, int i);
int retrieve_elements(std::string source, int *nums, int *count);
void print_all_elements(int elem[], int number, int print);
void output_lines(std::string source);
void output_elements(std::string source);
bool spans_completely(line l, int slab_i_start, int slab_i_end);

class min_heap{
private:
  min_heap_node* nodes;
  int heap_size;
  int type; // type of line comparator
  bool (*compare)(void *,int,void *,int); // the type of the heap determines the compare operation used
public:
  min_heap(min_heap_node array[], int size, int t);
  void min_heapify(int i);
  int left(int i);
  int right(int i);
  min_heap_node get_min();
  void replace_min(min_heap_node x);
};

class array_buffer{
private:
  int current_slot;
  void *buffer;
  int size;
  FILE *output_file;
  std::string output_filename;
  int stored_elements;
  int* READS;
  int not_open;
  int* WRITES;
  int (*write_n)(FILE*, void*, int, int, int*);
  int (*read_n)(FILE*, void*, int, int, int*);
  void* (*retrieve)(void *,int);
  void (*assign)(void *,int, void *);
  int buffer_type;
  int type;
  int read_slot;
  void initialize_on_insert(void *l);
  void real_insert_element(void *l);
public:
  array_buffer(int r_w, int t, int s, std::string  output, int* R, int* W);
  void insert_element(void* l);
  void* retrieve_element();
  int count_elements_in_disk();
  int number_of_stored_elements();
  void seek_to_start();
  void delete_file();
  void delete_buffer();
  void reset_file();
  bool has_elements();
  void backtrack_read();
  void reset_buffer();
  void dump_remaining_items();
  void reset_stored_count();
  void close_file();
  const char *get_filename();
  void seek_to_point(int i);
  void start_file();
  void open_file();
};
