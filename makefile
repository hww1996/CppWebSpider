cc=g++
cppflag=-std=c++11 -I include
object=StringAlgorithm.o CookieJar.o response.o request.o

vpath %.h include
vpath %.cpp src

WebSpider:main.cpp $(object)
	$(cc) -o WebSpider main.cpp $(object) $(cppflag)

StringAlgorithm.o:StringAlgorithm.cpp StringAlgorithm.h
	$(cc) -o StringAlgorithm.o -c src/StringAlgorithm.cpp $(cppflag)

CookieJar.o:CookieJar.cpp StringAlgorithm.h
	$(cc) -o CookieJar.o -c src/CookieJar.cpp $(cppflag)

response.o:response.cpp StringAlgorithm.h CookieJar.h
	$(cc) -o response.o -c src/response.cpp $(cppflag)

request.o:request.cpp StringAlgorithm.h CookieJar.h response.h
	$(cc) -o request.o -c src/request.cpp $(cppflag)
.PHONY:clean cleanall
cleanall:clean
	rm WebSpider
clean:
	rm $(object)
