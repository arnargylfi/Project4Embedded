all: controller test

controller: main.cpp
	g++ -o controller main.cpp -Iinclude

test: test_pwm.cpp
	g++ -o test_pwm test_pwm.cpp -Iinclude