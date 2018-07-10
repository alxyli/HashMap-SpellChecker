C11FLAG = -std=c++11

all: spellChecker.cpp
	g++ $(C11FLAG) spellChecker.cpp -o spellChecker

clean:
	rm -rf spellChecker

