#include "mutipart.h"

CMultipart::CMultipart()
{
  memset(mheader_accumulator,0,sizeof(mheader_accumulator));
	mheader_accumulator_pos = mheader_accumulator;
	mheader_name_value_pos = mheader_name_value;
	memset(mheader_name_value,0,sizeof(mheader_name_value));
	mheader_array = (sheader*)malloc(sizeof(sheader)*20);//default header is 20;
	memset(mheader_array,0,sizeof(mheader_array));	
	memset(mboundary,0,sizeof(mboundary));
	mboundary_pos = mboundary;
	mboundary_len = 0;
	mtmp_header_array = mheader_array;
	mheader_sum = 0;
//	mdata_state = 0;

	memset(mtmp_data_buf,0,sizeof(mtmp_data_buf));
	memset(mdata_accumulator_buf,0,sizeof(mdata_accumulator_buf));
	mdata_accumulator_buf_len = sizeof(mdata_accumulator_buf);
	mdata_accumulator_buf_pos = mdata_accumulator_buf;
	msum_file_data = 0;
	msum_tmp_data = 0;
	msum_all_tmp_data = 0;
	mtotal_write_file_data = 0;
	mtmp_data_buf_pos = mtmp_data_buf;

	
	memset(mheader_tmp_buf,0,sizeof(mheader_tmp_buf));
	mheader_tmp_buf_pos = mheader_tmp_buf;
	mheader_sum_data = 0;
	
	
	
}

CMultipart::~CMultipart()
{
	int i;
	for(i = 0; i < mheader_sum;i++){
		free(mheader_array[i].header_name);
		free(mheader_array[i].header_value);
	}
	free(mheader_array);
	free(mheader_content_type);
	free(mfile_path);
}


int CMultipart::set_boundary(char*boundary,int boundary_len)
{
	int i;
	for(i = 0; i < boundary_len;i++)
		*(mboundary+i) = *(boundary+i);
	mboundary_len = boundary_len;
	return MUL_OK;


}
void CMultipart::set_store_file(char *file_path)
{
		mfile_path_len = strlen(file_path);
		mfile_path  = (char *)malloc(sizeof(char)*(mfile_path_len+1));
		memset(mfile_path,0,mfile_path_len+1);
		memcpy(mfile_path,file_path,mfile_path_len);
}



int CMultipart:: parse_multipart_form_data(char*start,char*end)
{
	char *p ;
	int header_len;
	int rc;

	if(start == end){
		if(mstate != state_finish){
			printf("no data\n");
			return MUL_ERROR;
	}else
		return MUL_OK;
	}	
//	mboundary_pos= mboundary;//the reason that delete this :the number of read data is less than boundary.
	for(p = start;p != end;p++)
	
	switch(mstate){

	case state_seek_boundary:
		switch(mboundary_state){

			case state_boundary_begin:
				if(*p == '\r'){
					mboundary_state = state_boundary_r;
				}else{
					*mboundary_pos++ = *p;
					mboundary_len++;
				}
				break;
			case state_boundary_r:
				if(*p == '\n'){
					mstate = state_headers;
				}else{
					*mboundary_pos++ = '\r';//because \r is not store.b
					mboundary_len++;
					*mboundary_pos++ = *p;
					mboundary_len++;
					mboundary_state = state_boundary_begin;
				}
				break;
		}
	
		
		break;

	case state_after_boundary:
		switch(*p){
			case '\n':
				mstate = state_headers;
				mheader_accumulator_pos =mheader_accumulator;// add this sentence yk
				break;
			case '\r':
				break;
			case '-':
				mstate = state_finish;
				break;

		}

		
		break;

	case state_headers:
	switch(*p){
			case '\n':
			//complete reading one header.Here if..else..is IMPORTANT
				if(mheader_accumulator_pos == mheader_accumulator){//complete reading one header this is SECOND encounter \r\n
					mstate = state_data;//reading one header \r\n\r\n
		
				}else{//complete reading one header this is the FIRST encounter \r\n
					//store one header name into mheader_array;
					
					header_len = mheader_accumulator_pos - mheader_accumulator;
					// parse the content of one header
					rc = parse_headers(mheader_accumulator,mheader_accumulator+header_len);
					if(rc == MUL_OK){					
			//			mstate = state_data;//here is deleted by yankai 20130320
						mheader_accumulator_pos = mheader_accumulator;
						memset(mheader_accumulator,0,strlen(mheader_accumulator));
					}else{
						printf("parse_headers error\n");
						return -1;

					}				
			      }
			      break;
			case '\r':
				break;
			default:
			*mheader_accumulator_pos++ =*p; //accumulate the content of one header 
				break;
			

		}
		
		
		break;

	case state_data:



		if(!mis_file){
//It will be better to add the order of the  operation of \r and \n
			if(*p == *mboundary_pos){
				//begin modify 20130328 0000
				*mheader_tmp_buf_pos++ = *p;
				mheader_sum_data++;
				//end modify 20130328 0000
				mboundary_pos++;
			}else{
			//begin modify 20130328 0000
				if(mheader_tmp_buf){
					if(mheader_sum_data < mboundary_len){
						memcpy(mheader_name_value_pos,mheader_tmp_buf,mheader_tmp_buf_pos-mheader_tmp_buf);
						mheader_name_value_pos += mheader_tmp_buf_pos-mheader_tmp_buf;
						//reset
						memset(mheader_tmp_buf,0,sizeof(mheader_tmp_buf));
						mheader_tmp_buf_pos = mheader_tmp_buf;
						mboundary_pos = mboundary;
					}else{
						//encounter boundary ,so  reset
						memset(mheader_tmp_buf,0,sizeof(mheader_tmp_buf));
						mheader_tmp_buf_pos = mheader_tmp_buf;
						mheader_sum_data = 0;
					}
					
					
					

				}
			//end modify 20130328 0000
				if(mboundary_pos == mboundary){
					//begin modify 20130328 0000
					if(*p == '\r'){
						break;
					}
					//end modify 20130328 0000
					if(*p != '\n'){
                    	*mheader_name_value_pos++ = *p;
					}else{/**p == \n this means :data end*/
				
                    	header_len = mheader_name_value_pos - mheader_name_value;
                        mtmp_header_array->header_value = (char*)malloc(sizeof(char)*(header_len+1));
                    	memset(mtmp_header_array->header_value,0,header_len+1);
                        memcpy(mtmp_header_array->header_value,mheader_name_value,header_len);
                        //printf("###%s\n",mtmp_header_array->header_value);
                        mtmp_header_array++;// to store next header name and value
                        mheader_sum++;
						//reset 
                   		mheader_name_value_pos = mheader_name_value;
                   		memset(mheader_name_value,0,sizeof(mheader_name_value));
					}
				}else{
					printf("*maybe error\n");
				}

			}
			if(mboundary_pos == mboundary + mboundary_len ){//modify the order of two if
	        	mstate = state_after_boundary;
	            mboundary_pos = mboundary;
	                        //mheader_name_value_pos = mheader_name_value;
			}


		}else{//if(!mis_file)
		//	printf("%!c",*p);
		if(output_file_date2file(*p) == 2){ //finish write file data .Return 2.Otherwise return 0
			mstate = state_headers;
		}


		}//else  if(mis_file)
		
		break;

	case state_finish:
		printf("ALL finished\n");
		break;



	}

	return MUL_OK;
}



int CMultipart::parse_headers(char * start,char * end)
{
	
	char * p = start;
	int content_type_len;
	if(strncasecmp(start,CONTENT_DISPOSITION,strlen(CONTENT_DISPOSITION)) == 0){
		p = p + strlen(CONTENT_DISPOSITION) - 1;

		p += strspn(p, " ");//p is : form-data;name="xxx"
		
		parse_headers_name(p,end);

		

	}else if(strncasecmp(start,CONTENT_TYPE,strlen(CONTENT_TYPE)) == 0){
		p = p + strlen(CONTENT_TYPE) - 1;
		p += strspn(p, " ");//p is : application/octet-stream
		//store content-type in mheader_content_type
		content_type_len = end - p;
		mheader_content_type = (char*)malloc(sizeof(char)*(content_type_len+1));
		memset(mheader_content_type,0,1+content_type_len);
		memcpy(mheader_content_type,p,content_type_len);

	}else{
		printf("maybe error.headers is NOT content-disposition and content_type\n");
		return MUL_ERROR;

	}
	
	return MUL_OK;

}
//start is : 
// form-data;name="xxx"
// form-data;filename="yk.txt"
int CMultipart::parse_headers_name(char*start,char*end)
{
	typedef enum {
		begin_state,
		first_quotation,
		second_quotaion
	}read_state;
	read_state state_read;
	printf("parse-headers-name\n");
	char * p;
	char *name,*tmp;
	int header_len;
	p = strstr(start,FILE_NAME);
	if(p != 0){//
		p = p + strlen(FILE_NAME) - 1;
		name = (char*)malloc(sizeof(char)*(end - p));
		tmp = name;
		memset(name,0,end-p);
		state_read = begin_state;
		mis_file = 1;//This is file 
		//open file
		if((mfile_fd = open(mfile_path,O_RDWR|O_CREAT|O_TRUNC)) == -1){
			printf("open %s file error",mfile_path);
			return MUL_ERROR;
		}
		//get the name of file
		for(;p != end;p++){
			switch(state_read){
				case begin_state:
					if(*p == '\"'){
						state_read = first_quotation;
					}
					break;
				case first_quotation:
					if(*p != '\"'){
						*tmp++ = *p; //accumulator name

					}else {
						state_read = second_quotaion;
						header_len = tmp - name;
						mfile_name=(char*)malloc((header_len+1)*sizeof(char));
						memset(mfile_name,0,(header_len));
						mfile_name_len = header_len;
						memcpy(mfile_name,name,header_len);

						//free name
						free(name);
						
					}
					break;
				case second_quotaion:
					break;
			}
		}

	}

	p = strstr(start,NAME);
	if(p != 0){//
//		mis_file = 0;// This is NOT file
		p = p + strlen(NAME) - 1;
		name = (char*)malloc(sizeof(char)*(end - p));
		tmp = name;
		memset(name,0,end-p);
		state_read = begin_state;
		for(;p != end;p++){
			switch(state_read){
				case begin_state:
					if(*p == '\"'){
						state_read = first_quotation;
					}
					break;
				case first_quotation:
					if(*p != '\"'){
						*tmp++ = *p; //accumulator name
					}else {
						state_read = second_quotaion;
						header_len = tmp - name;		
						mtmp_header_array->header_name =(char*)malloc((header_len+1)*sizeof(char));
						memset(mtmp_header_array->header_name,0,(header_len));
						mtmp_header_array->name_len = header_len;
						memcpy(mtmp_header_array->header_name,name,header_len);
						//free name
						free(name);
								
					}
					break;
				case second_quotaion:
					break;
				}
			}

		}
	
}

int CMultipart::output_file_date2file(char p)
{
	int n =0;
	
	
	switch(mdata_state){
	
	case state_data_begin:
//mdata_accumulator_buf full 

		if(msum_file_data == mdata_accumulator_buf_len){
			n = write(mfile_fd,mdata_accumulator_buf,msum_file_data);
			mtotal_write_file_data += n;
			//reset	
			memset(mdata_accumulator_buf,0,sizeof(mdata_accumulator_buf));//origin:strlen .modify is sizeof
			msum_file_data = 0;
			mdata_accumulator_buf_pos = mdata_accumulator_buf;
		}

//flush mtmp_data_buf		
		if(mtmp_data_buf){
			n = write(mfile_fd,mtmp_data_buf,msum_all_tmp_data);
			mtotal_write_file_data += n;
			//reset
			memset(mtmp_data_buf,0,sizeof(mtmp_data_buf));		//origin:strlen .modify is sizeof
			msum_tmp_data = 0;
			msum_all_tmp_data = 0;
			mtmp_data_buf_pos = mtmp_data_buf;
		}

	
		if(p == '\r'){//char is \r 
		
			// store mtmp_data_buf[0]='\r';		
			*mtmp_data_buf_pos++ = '\r';
			msum_all_tmp_data++;
			//msum_tmp_data++;
			mdata_state = state_data_r;
		}else {//char is NOT \r ,accumulate
			*mdata_accumulator_buf_pos++ = p;
			msum_file_data++;
		}
		
		
		break;
	case state_data_r:

			//flush mdata_accumulator_buf
			if(mdata_accumulator_buf){
				n = write(mfile_fd,mdata_accumulator_buf,msum_file_data);
				mtotal_write_file_data += n;
				//reset
				memset(mdata_accumulator_buf,0,sizeof(mdata_accumulator_buf));
				msum_file_data = 0;
				mdata_accumulator_buf_pos = mdata_accumulator_buf;
				
			}	
			*mtmp_data_buf_pos++=p;
			msum_all_tmp_data++;
			if(p == '\n'){
				mdata_state = state_data_n;
			}else{
				mdata_state = state_data_begin;
				
				//flush mtmp_data_buf		
				if(mtmp_data_buf){
					
					n = write(mfile_fd,mtmp_data_buf,msum_all_tmp_data);
					mtotal_write_file_data += n;
					//reset
					memset(mtmp_data_buf,0,sizeof(mtmp_data_buf));		
					msum_tmp_data = 0;
					msum_all_tmp_data = 0;
					mtmp_data_buf_pos = mtmp_data_buf;
				}
		
			}
			
		break;
	case state_data_n:
			if(msum_tmp_data == mboundary_len){//encounter boundary
				mdata_state = state_data_finish_read_file_data;
				if(msum_all_tmp_data - msum_tmp_data - 2){//if >0 means :mtmp_data_buf include other data except boundary. the reason why decrease 2 is \r\n before boundary
					
					n = write(mfile_fd,mtmp_data_buf,msum_all_tmp_data - msum_tmp_data -2);
					mtotal_write_file_data += n;
					//reset
					memset(mtmp_data_buf,0,sizeof(mtmp_data_buf));
					mtmp_data_buf_pos = mtmp_data_buf;
					msum_all_tmp_data = 0;
					msum_tmp_data = 0;
				}
			}else if(msum_tmp_data < mboundary_len){
				if(p != *mboundary_pos){
					mboundary_pos = mboundary;//reset mboundary_pos
					*mtmp_data_buf_pos++ = p;
					msum_all_tmp_data++;
					if( p == '\r'){
						mdata_state = state_data_r;
					}else{
						mdata_state = state_data_begin;
					}
					
				
				}else{
					
					*mtmp_data_buf_pos++ = p;
					msum_tmp_data++;
					msum_all_tmp_data++;
					mboundary_pos++;
					
				}	
			}
		break;
	case state_data_finish_read_file_data:
		mis_file = 0;
		mboundary_pos = mboundary;//reset
		printf("finish read file_data\n");
		return 2;
		break;


	}
	return 0;

}

int  CMultipart::disp_header_array()
{	
	printf("disp_header_array\n");
	printf("mheader_sum:%d\n",mheader_sum);
	sheader * p;
	int i;
	p = mheader_array;
	printf("\ncontent-type:%s\n",mheader_content_type);
	printf("\nmfile_name%s\n\n",mfile_name);

	for(i=0;i < mheader_sum  ;i++,p++){
		printf("%s:%s\n",p->header_name,p->header_value);
		
	}
	return MUL_OK;
}
