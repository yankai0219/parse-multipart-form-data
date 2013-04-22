#ifndef _MUTIPART_H_
#define _MUTIPART_H_
#include "common.h"
#define BUF_LEN 1000
#define MUL_ERROR -2
#define MUL_OK     2
#define FILE_BUF_LEN 500

#define CONTENT_DISPOSITION "Content-Disposition:"
#define CONTENT_TYPE "Content-Type:"
#define FORM_DATA "form-data;"
#define NAME "name"
#define FILE_NAME "filename"
typedef enum {
  state_seek_boundary = 0,
	state_after_boundary,
	state_headers,
	state_data,
	state_finish
}State;
typedef enum{
	state_data_begin = 0,
	state_data_r,
	state_data_n,
	state_data_finish_read_file_data

}DataState;
typedef enum{
	state_boundary_begin = 0,
	state_boundary_r
}BoundaryState;
typedef struct{
	char *header_name;
	int name_len;
	char *header_value;
	int value_len;

}sheader;
class CMultipart{

public:
	CMultipart();
	~CMultipart();
public:
	int parse_multipart_form_data(char*start,char*end);
	int parse_headers(char*start,char*end);
	int parse_headers_name(char*start,char*end);
	int set_boundary(char * boundary,int boundary_len);
	int disp_header_array();
	void set_store_file(char *file_path);
	int  output_file_date2file(char p);
private:
	char mboundary[BUF_LEN];
	char *mboundary_pos;
	int mboundary_len;
	State mstate;
	char mheader_accumulator[BUF_LEN];
	sheader *mheader_array; //one char *array to record all multipart/form-data headers
	
	int mheader_sum;
	sheader *mtmp_header_array;
	char mheader_name_value[BUF_LEN];
	char *mheader_name_value_pos;
	int mheader_accumulator_len ;
	char *mheader_accumulator_pos;
	char * mheader_content_type;
	char * mfile_name;
	int mfile_name_len;

	int mis_file;
	int mfile_fd;
	char *mfile_path;
	int mfile_path_len;

	DataState mdata_state;//used when operating the file data
	char mtmp_data_buf[FILE_BUF_LEN];
	char mdata_accumulator_buf[FILE_BUF_LEN];
	int mdata_accumulator_buf_len;
	char *mdata_accumulator_buf_pos;
	char *mtmp_data_buf_pos;
	int msum_file_data;
	int msum_tmp_data;//only summum boundary .NOT include \r\n
	int msum_all_tmp_data;
	long int mtotal_write_file_data;

	char *mheader_tmp_buf_pos;
	char mheader_tmp_buf[BUF_LEN];
	int mheader_sum_data;

	BoundaryState mboundary_state;
};
#endif

