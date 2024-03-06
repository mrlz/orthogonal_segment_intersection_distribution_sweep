#include "utility.h"


uint64_t operation_time_in_nanos(struct timespec start, struct timespec end){
  uint64_t diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  return diff;
}

double operation_time_in_seconds(uint64_t nanos){
  double d = (double)nanos/1000000000.0;
  return d;
}

void print_operation_time(struct timespec start, struct timespec end){
  uint64_t nanos = operation_time_in_nanos(start,end);
  double seconds = operation_time_in_seconds(nanos);
  std::cout << nanos << " nanoseconds = " << seconds << " seconds";
}


void swap(min_heap_node* x, min_heap_node* y){
    min_heap_node temp = *x;
    *x = *y;
    *y = temp;
}

int file_bytesize(std::string source){
struct stat filestatus;
stat( source.c_str(), &filestatus );
return filestatus.st_size;
}

void print_a_line(line l){
  std::cout << "[(" << l.x1 <<","<< l.y1<<"),(" << l.x2 << "," << l.y2 << ")]";
}

void print_all_elements(int elem[], int number, int print){
  int d = elem[1];
  int i = 0;
  while(i < number){
    std::cout << i<< " = " <<elem[i] << std::endl;
    i++;
  }
  std::cout << "total elements = " << i << std::endl;
}

void print_all_lines(line lines[], int number_of_lines, int print){
 int i = 0;
 int H = 0;
 int V = 0;
 while(i < number_of_lines){
   if (print == 1){
   std::cout << i << " ";
   print_a_line(lines[i]);
  }
   if (is_horizontal(lines[i])){
     if (print == 1){
     std::cout << " H";
    }
     H++;
   }else{
     if (print == 1){
     std::cout << " V";
    }
     V++;
   }
   if (print == 1){
   std::cout << std::endl;
  }
   i = i + 1;
 }
 std::cout << "total lines = " << H+V << " | H = " << H << " | V = " << V << std::endl;
}

int total_lines_in_file(std::string source){
  return file_bytesize(source)/(4*DATA_TYPE_SIZE);
}

int read_n_lines(FILE *pFile, void *buffer, int n_lines, int index_buffer, int* count){
  line *lines = (line *)buffer;
  int read = fread(&(lines[index_buffer]), DATA_TYPE_SIZE, 4*n_lines, pFile);
  *count = *count + 1;
  return read/4; //read reports the number of DATA_TYPE-sized elements read, if we read 32 lines
  //it would return 128 elements.
}

void output_lines(std::string source){
  int r = 0;
  int total = total_lines_in_file(source);
  line *lines = new line[total];
  int read = retrieve_lines(source,lines,&r);
  print_all_lines(lines,read,1);
}

void output_elements(std::string source){
  int r = 0;
  int total = total_elements_in_file(source);
  int *elements = new int[total];
  int read = retrieve_elements(source,elements,&r);
  print_all_elements(elements,read,1);
}

int write_n_lines(FILE *pFile, void *buffer, int n_lines, int index_buffer, int* count){
  line *lines = (line *)buffer;
  int wrote = fwrite(&(lines[index_buffer]), DATA_TYPE_SIZE, 4*n_lines, pFile);
  *count = *count + 1;
  return wrote/4;
}

int read_n_ints(FILE *pFile, void *buffer, int n, int index_buffer, int* count){ //Utility
  int *x_buffer = (int *)buffer;
  int read = fread(&(x_buffer[index_buffer]), DATA_TYPE_SIZE, n, pFile);
  *count = *count + 1;
  return read;
}

int write_n_ints(FILE *pFile, void *buffer, int n, int index_buffer, int* count){ //Utility
  int *x_buffer = (int *)buffer;
  int wrote = fwrite(&(x_buffer[index_buffer]), DATA_TYPE_SIZE, n, pFile);
  *count = *count + 1;
  return wrote;
}

int total_elements_in_file(std::string source){ //Utility
  return file_bytesize(source)/DATA_TYPE_SIZE;
}

int retrieve_elements(std::string source, int *nums, int *count){
  int total_elements = total_elements_in_file(source);
  std::cout << "total elements = " << total_elements << std::endl;
  FILE *in = fopen(source.c_str(), "rb");
  int read = read_n_ints(in, nums, total_elements, 0, count);
  fclose(in);
  return total_elements;
}

int retrieve_lines(std::string source, line *lines, int* count){
  int total_lines = file_bytesize(source)/(4*DATA_TYPE_SIZE);
  FILE *in = fopen(source.c_str(), "rb");
  int read = read_n_lines(in,lines,total_lines, 0, count);
  fclose(in);
  return read;
}

bool less_x(void *vl1, int i, void *vl2, int j){
  int l1 = ((int *)vl1)[i];
  int l2 = ((int *)vl2)[j];
  if (l1 <= l2){
    return true;
  }else{
    return false;
  }
}

bool less_y(void* vl1, int i, void* vl2, int j){
  line l1 = ((line *)vl1)[i];
  line l2 = ((line *)vl2)[j];
  if (l1.y1 < l2.y1){
    return true;
  }else{
    if (l1.y1 == l2.y1){
      if ((l2.x1 == l2.x2) && (l1.x1 != l1.x2)){
        return false;
      }else{
        return true;
      }
    }
    return false;
  }
}

void assign_ints(void* vl1, int i, void* vl2, int j){
  ((int *)vl1)[i] = ((int *)vl2)[j];
}

void assign_lines(void* vl1, int i, void* vl2, int j){
  ((line *)vl1)[i] = ((line *)vl2)[j];
}

void assign_int(void *vl1, int i, void* value){
int *e = &(((int *)vl1)[i]);
*e = *((int *)value);
}

void assign_line(void *vl1, int i, void* value){
  line *l = (line *)value;
  line *line_in_buffer = &(((line *)vl1)[i]);
  line_in_buffer->x1 = l->x1;
  line_in_buffer->x2 = l->x2;
  line_in_buffer->y1 = l->y1;
  line_in_buffer->y2 = l->y2;
}

void* retrieve_int(void *vl1, int i){
  return &(((int *)vl1)[i]);
}

void* retrieve_line(void *vl1, int i){
  return &(((line *)vl1)[i]);
}

void file_stats(std::string file){
  int total_lines = total_lines_in_file(file);
  std::cout << "total lines in sorted file = " << total_lines << std::endl;
  //line *lines = new line[total_lines]; // Could be too big for stack so we allocate to heap
  //retrieve_lines(file, lines);
  //print_all_lines(lines,total_lines);
  //delete[] lines;
}

bool is_horizontal(line l){ //Utility
  if (l.y1 == l.y2){
    return true;
  }else{
    return false;
  }
}

bool spans_completely(line l, int slab_i_start, int slab_i_end){
  if (l.x1 <= slab_i_start && l.x2 >= (slab_i_end-1)){
    return true;
  }
  return false;
}

min_heap::min_heap(min_heap_node array[], int size, int t){
    heap_size = size;
    nodes = array;
    type = t;

    //type defines the compare function
    if (type == 0){
      compare = &less_x;
    }else if(type == 1){
      compare = &less_y;
    }

    int i = (heap_size - 1) / 2;
    while (i >= 0){
        min_heapify(i);
        i--;
    }
}

void min_heap::min_heapify(int i){
    int l = left(i);
    int r = right(i);
    int smallest = i;

    if (l < heap_size && compare(nodes[l].element,0, nodes[i].element,0)){
        smallest = l;
    }

    if (r < heap_size && compare(nodes[r].element,0, nodes[smallest].element,0)){
        smallest = r;
    }

    if (smallest != i){
        swap(&nodes[i], &nodes[smallest]);
        min_heapify(smallest);
    }
}

int min_heap::left(int i) {
return (2 * i + 1);
}

int min_heap::right(int i) {
return (2 * i + 2);
}

min_heap_node min_heap::get_min() {
return nodes[0];
}

void min_heap::replace_min(min_heap_node x){
    nodes[0] = x;
    min_heapify(0);
}

array_buffer::array_buffer(int r_w,int t, int s, std::string output, int* R, int* W){
  current_slot = 0;
  size = s;
  output_filename = output;

  stored_elements = 0;
  READS = R;
  WRITES = W;
  type = t;
  read_slot = 0;
  not_open = 1;
  buffer_type = r_w;

  if (r_w == 0){
    output_file = fopen(output_filename.c_str(),"rb+");
    setvbuf ( output_file , NULL , _IONBF , 0 ); // UNBUFFERED STREAM
    not_open = 0;
  }
  if (type == 0){
    buffer = new int[size];
    write_n = &write_n_ints;
    read_n = &read_n_ints;
    assign = &assign_int;
    retrieve = &retrieve_int;
  }else{
    buffer = new line[size];
    write_n = &write_n_lines;
    read_n = &read_n_lines;
    assign = &assign_line;
    retrieve = &retrieve_line;
  }
}

void array_buffer::open_file(){
  if (buffer_type == 0){
    output_file = fopen(output_filename.c_str(),"rb+");
    setvbuf ( output_file , NULL , _IONBF , 0 ); // UNBUFFERED STREAM
  }else{
  output_file = fopen(output_filename.c_str(),"wb+");
  setvbuf ( output_file , NULL , _IONBF , 0 ); // UNBUFFERED STREAM
  }
  not_open = 0;
}

void array_buffer::insert_element(void *l){
  assign(buffer, current_slot, l);
  current_slot++;
  stored_elements++;
  if (current_slot == size){
    if (not_open == 1){
      open_file();
    }
    if (not_open == 0){
    write_n(output_file, buffer, size, 0,WRITES);
    current_slot = 0;
  }
  }
}

void* array_buffer::retrieve_element(){
  if (read_slot < current_slot){
    stored_elements--;
    void * b = retrieve(buffer,read_slot);
    read_slot++;
    return b;
  }else{
    int read = read_n(output_file,buffer,size,0,READS);
    current_slot = read;
    read_slot = 0;
    if (current_slot > 0){
    return retrieve_element();
    }
  }
}

void array_buffer::backtrack_read(){
  if (read_slot > 0){
    read_slot--;
    stored_elements++;
  }
}

int array_buffer::count_elements_in_disk(){
  if (not_open == 0){
  if (type == 0){
    stored_elements = stored_elements + total_elements_in_file(output_filename);
  }else{
  stored_elements = stored_elements + total_lines_in_file(output_filename);
  }
  }
  return stored_elements;
}

int array_buffer::number_of_stored_elements(){
  return stored_elements;
}

void array_buffer::seek_to_start(){
  if (not_open == 0){
  fseek(output_file,0,SEEK_SET);
  }
}

void array_buffer::seek_to_point(int i){
  if (not_open == 0){
  if (type == 0){
    fseek(output_file,i*DATA_TYPE_SIZE,SEEK_SET);
  }else{
    fseek(output_file,i*DATA_TYPE_SIZE*4,SEEK_SET);
  }
  }
}

void array_buffer::delete_file(){
  if (not_open == 0){
  fclose(output_file);
  not_open = 1;
  }
  remove(output_filename.c_str());
}

void array_buffer::delete_buffer(){
  if (type == 0){
    delete[] (int *)buffer;
  }else{
    delete[] (line *)buffer;
  }
}

void array_buffer::reset_file(){
  if (not_open == 0){
    fclose(output_file);
  output_file = fopen(output_filename.c_str(),"wb+");
  setvbuf ( output_file , NULL , _IONBF , 0 ); // UNBUFFERED STREAM
  }
}

bool array_buffer::has_elements(){
  if (current_slot > 0){
    return true;
  }else{
    return false;
  }
}

void array_buffer::dump_remaining_items(){
  if (not_open == 1){
    open_file();
  }
  if (current_slot > 0){
    write_n(output_file, buffer, current_slot,0,WRITES);
    current_slot = 0;
  }
}

void array_buffer::reset_buffer(){
  current_slot = 0;
  read_slot = 0;
}

void array_buffer::reset_stored_count(){
  stored_elements = 0;
}

void array_buffer::close_file(){
  if (not_open == 0){
  fclose(output_file);
  not_open = 1;
  }
}

const char* array_buffer::get_filename(){
  return output_filename.c_str();
}
