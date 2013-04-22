#include "mutipart.h"
#include "common.h"
#define READ 50
CMultipart part;

int main(int argc,char *argv[])
{
  if(argc != 2){
		printf("./Multi  multipart_file_name");
		return -1;

	}
	int fd;
	int readn;
	char buf[READ];
	char  file_path[] = "/tmp/mm.txt";

	if((fd = open(argv[1],O_RDWR)) == -1){
		printf("open error\n");
		return -1;
	}

	part.set_store_file(file_path);

	memset(buf,0,READ);
	for(;;){
		memset(buf,0,READ);
		readn = read(fd,buf,READ);
		if(readn != 0){

			part.parse_multipart_form_data(buf,buf+readn);
		}else if(readn ==0){
			printf("end read\n");
			break;
		}
	}
	
	part.disp_header_array();
	return 0;
}

