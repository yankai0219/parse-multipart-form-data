Readme:
Edited by Yankai 2013/04/10
AIMS:This Program is used for parse multipart/form-data
This Program is the imitation of the function upload_process_buf in Nginx Upload Module.
1.About Compile:
  		make 
2.About Run:
	When you run the program,two parameter is needed.
	first is boundary,second is multipart-filename.
	Boundary is boundary in the file  multipart/form-data
	multipart-filename stores the data of multipart/form-data
	For Example:
			./Multi  test.txt

3.About Result:
	The Multipart/form-data will be divided. 
	1)Headers and its value will be stored in the mheader_array and be showed in screen.
	2)the upload file data will be stored in /tmp/mm.txt.
4.About Used:
	main.cpp show how to use this program.
		
