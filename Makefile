all:
	g++ -I src/include -L src/lib -o main Gmail.cpp main.cpp -lcurl
