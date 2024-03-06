#include "k_way_mergesort.h"

int DISK_READS = 0;
int DISK_WRITES = 0;

void merge(void* arr, int l, int m, int r, int type){
    int i, j, k;
    int nl = m - l + 1;
    int nr = r - m;

    //Might not fit in stack
    void *left_elements;
    void *right_elements;

    bool (*compare)(void *,int,void *,int);
    void (*assign)(void *,int, void *,int);

    if (type == 0){
      compare = &less_x;
      assign = &assign_ints;
      left_elements = new int[nl];
      right_elements = new int[nr];
    }else if(type == 1){
      compare = &less_y;
      assign = &assign_lines;
      left_elements = new line[nl];
      right_elements = new line[nr];
    }

    for(i = 0; i < nl; i++){
        assign(left_elements,i,arr, l + i);
      }
    for(j = 0; j < nr; j++){
        assign(right_elements,j,arr, m + 1 + j);
      }

    i = 0;
    j = 0;
    k = l;
    while (i < nl && j < nr){
        if (compare(left_elements,i,right_elements,j)){
            assign(arr,k++,left_elements,i++);
        }else{
          assign(arr,k++,right_elements,j++);
        }
    }

    while (i < nl){
      assign(arr,k++,left_elements,i++);
      }

    while(j < nr){
      assign(arr,k++,right_elements,j++);
      }

      if (type == 0){
        delete[] (int *)left_elements;
        delete[] (int *)right_elements;
      }else{
      delete[] (line *)left_elements;
      delete[] (line *)right_elements;
      }
}

   //type encodes if the comparison will be according to x or y coordinate, with
   //0 and 1, respectively.
void mergesort(void *arr, int l, int r, int type){
    if (l < r){
        int m = l + (r - l) / 2;

        mergesort(arr, l, m, type);
        mergesort(arr, m + 1, r, type);

        merge(arr, l, m, r, type);
    }
}

void create_runs(std::string source, int runs , int run_size, int B, int type, std::string offset){
    if (print_progress==1){
    std::cout << offset<< runs << " runs, each with size " << run_size << " and " << B << " elements per block." << std::endl;
  }
    array_buffer *input_buffer = new array_buffer(0,type, B, source, &DISK_READS, &DISK_WRITES);
    input_buffer->count_elements_in_disk();
    array_buffer** output_buffers;
    output_buffers = new array_buffer*[runs];
    char filename[20]; //In case we have too many pieces
    for (int i = 0; i < runs; i++){
        snprintf(filename, sizeof(filename), "%d", i);
        output_buffers[i] = new array_buffer(1,type, B, filename, &DISK_READS, &DISK_WRITES);
    }

    int next_output_file = 0;
    void (*assign)(void*,int,void*);
    void *(*retrieve)(void*,int);

    void *buffer;
    if(type == 0){
      buffer = new int[run_size];
      assign = &assign_int;
      retrieve = &retrieve_int;
    }else{
      buffer = new line[run_size];
      assign = &assign_line;
      retrieve = &retrieve_line;
    }

    int i;
    int read = 0;
    while (input_buffer->number_of_stored_elements()>0){
      i = 0;
        while (input_buffer->number_of_stored_elements() > 0 && i < run_size){
          assign(buffer,i,input_buffer->retrieve_element());
          i++;
        }

        mergesort(buffer, 0, i - 1, type);

        int j;
        for (j = 0; j < i; j++){
        output_buffers[next_output_file]->insert_element(retrieve(buffer,j));

        }
        next_output_file++;
    }

    for (int i = 0; i < runs; i++){
      output_buffers[i]->dump_remaining_items();
      output_buffers[i]->close_file();
      output_buffers[i]->delete_buffer();
      delete output_buffers[i];
      }
      delete[] output_buffers;

    input_buffer->close_file();
    input_buffer->delete_buffer();
    delete input_buffer;

    if (type == 0){
      delete[] (int *)buffer;
    }else{
      delete[] (line *)buffer;
    }

}

void merge_files(std::string destination, int runs, int B, int type){
    void (*assign)(void*,int,void*);
    if(type == 0){
      assign = &assign_int;
    }else{
      assign = &assign_line;
    }
    array_buffer *output_buffer = new array_buffer(1,type, B, destination, &DISK_READS, &DISK_WRITES);

    array_buffer** input_buffers;
    input_buffers = new array_buffer*[runs];
    int *buffer_indexes = new int[runs];
    min_heap_node nodes[runs];
    int depleted_files = 0;
    int i;
    int reads = 0;

    for (i = 0; i < runs; i++){
        char file[20];
        snprintf(file, sizeof(file), "%d", i);
        input_buffers[i] = new array_buffer(0,type, B, file, &DISK_READS, &DISK_WRITES);
        input_buffers[i]->count_elements_in_disk();
        nodes[i].element = input_buffers[i]->retrieve_element();
        nodes[i].i = i;
    }
    min_heap heap(nodes, i, type);
    while (depleted_files != i){
        min_heap_node root = heap.get_min();
        output_buffer->insert_element(root.element);
        if (input_buffers[root.i]->number_of_stored_elements()>0){
        root.element=input_buffers[root.i]->retrieve_element();
        }else{
          void *maxim;
          depleted_files++;
          if (type == 0){
            int max_int = INT_MAX;
            maxim = &max_int;
          }else{
            line l;
            l.y1 = INT_MAX;
            maxim = &l;
          }
        root.element = maxim;
        }
        heap.replace_min(root);
    }
    for (int i = 0; i < runs; i++){
        input_buffers[i]->delete_file();
        input_buffers[i]->delete_buffer();
        delete input_buffers[i];
    }
    delete[] input_buffers;

    output_buffer->dump_remaining_items();
    output_buffer->close_file();
    output_buffer->delete_buffer();
    delete output_buffer;
}


double k_way_mergesort(std::string source,  std::string destination, double memory, double blocksize, int type, std::string offset, unsigned long long* read_output, unsigned long long* write_output, double* time_output, double * estimate_output, double * c_output){
    // Merge the runs using the K-way merging
    //double blocksize = 512.0; //Reported optimal amount of bytes to read/write to disk in current machine.
    if (type == 0 && print_progress==1){
      std::cout << offset << "SORTING A LIST OF x-coordinates from file " << source << " into file " << destination << std::endl;
    }else if(print_progress==1){
      std::cout << offset << "SORTING A LIST OF LINES BY y1 coordinate from file " << source << " into file "<<destination << std::endl;
    }
    DISK_READS = 0;
    DISK_WRITES = 0;
    double M = 0.0;
    double B = 0.0;
    double N = 0.0;
    double filesize = 0.0;
    if (type == 1){
    M = memory/(4*DATA_TYPE_SIZE); //Amount of lines that can be stored in main memory.
    B = blocksize/(4*DATA_TYPE_SIZE); //Lines per block.
    N = total_lines_in_file(source); //Number of lines to process. Problem size.
    filesize = N*(4*DATA_TYPE_SIZE);
    }else{
      M = memory/(DATA_TYPE_SIZE); //Amount of ints that can be stored in main memory.
      B = blocksize/(DATA_TYPE_SIZE); //ints per block.
      N = total_elements_in_file(source); //Number of ints to process. Problem size.
      filesize = N*(DATA_TYPE_SIZE);
    }
    double m = memory/blocksize; //Amount blocks that can be stored in main memory.
    struct timespec start, end;

    double n = N/B; //Problem size in number of blocks.

    double effective_m = floor(m);
    effective_m = effective_m/2.0; // We'll do a merge with O(n) main memory cost.
    double max_lines_per_run = effective_m * B;
    double number_of_runs = N/max_lines_per_run;

    double aprox_num_reads = n*log(n)/log(m/2);
    double total_memory = (memory/1024.0)/1024.0;

    if (verbose == 1){
    std::cout << offset<<"TOTAL MEMORY = " << memory << " bytes = " << total_memory << " Mb" << std::endl;
    std::cout << offset<<"Effective memory is half = " << total_memory/2.0 << std::endl;
    std::cout << offset<<"File to be sorted is " << (int)filesize << " bytes = " << (filesize/1024.0)/1024.0 << " Mb, with " << (int)N << " elements" << std::endl;
    std::cout << offset<<"BLOCKSIZE is " << blocksize << " bytes" << std::endl;
    }
    if (print_progress==1){
    std::cout << offset<<"IN THEORY: nlog(n) ~ " << aprox_num_reads << std::endl;
    }

    int runs = (int)ceil(number_of_runs);
    int run_size = (int)max_lines_per_run;
    int lines_block = (int)B;

    clock_gettime(CLOCK_MONOTONIC, &start);
    create_runs(source, runs, run_size, lines_block, type, offset);
    merge_files(destination, runs, lines_block, type);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double c = (double)DISK_READS/aprox_num_reads;
    double total_time = operation_time_in_seconds(operation_time_in_nanos(start,end));

    if (print_progress==1){
      std::cout << offset<<"TOTAL TIME OF MERGESORT = ";
      std::cout << total_time;
        std::cout << std::endl;
        std::cout << offset << "NUMBER OF READS: " << DISK_READS << " NUMBER OF WRITES: " << DISK_WRITES << std::endl;
          std::cout << offset <<"With C = " << c << " we have C*nlog(n) = " << c <<"*"<<aprox_num_reads <<" = "  << c*aprox_num_reads << std::endl;
    }
    *read_output=DISK_READS;
    *write_output=DISK_WRITES;
    *time_output=total_time;
    *estimate_output=aprox_num_reads;
    *c_output=c;
    return aprox_num_reads;
}
