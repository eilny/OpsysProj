all:
	# g++ -Wall -Werror -g -std=c++11 *.cpp 
	g++ -Wall -Werror -g -std=c++11 -D DEBUG_MODE *.cpp -o debug
	