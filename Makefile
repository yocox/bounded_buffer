test: test.out
	./test.out

test.out: ./test.cpp ./bounded_buffer.hpp
	g++ \
		-g -std=c++11 \
		-o test.out \
		-I../googletest/googletest/include \
		test.cpp -pthread \
		../googletest/googlemock/gtest/libgtest.a \
		../googletest/googlemock/gtest/libgtest_main.a \
		-pthread

clean:
	rm -rf ./test.out


