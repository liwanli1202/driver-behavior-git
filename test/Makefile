
TARGET = DriverBehaviorTest

all: $(TARGET)

CXX = g++
LD = g++

CPPFLAGS = -I /usr/local/include -I /usr/local/include/opencv \
           -I ../Code/software/apps -I ../Code/software/lib \
           -I ../webdash/clientSocket/webclient -Wall

CXXFLAGS = -g -fprofile-arcs -ftest-coverage -fPIC -O0 -std=gnu++0x $(CPPFLAGS)

LDFLAGS = -L /usr/local/lib
CPPUNIT_LIBS = -lcppunit -ldl
OPENCV_LIBS = `pkg-config --libs opencv`
CPPUNIT_CONFIG = /usr/bin/cppunit-config
CPPUNIT_LIBS = -lcppunit -ldl
GCOV_LIBS = -lgcov
LIBS = $(OPENCV_LIBS) $(CPPUNIT_LIBS) $(GCOV_LIBS)

SOURCES = ./DriverBehaviorTest.cpp \
          ./ConvolutionTest.cpp ../Code/software/apps/Convolution.cpp \
          ./LaneMarkingDetectorTest.cpp ../Code/software/apps/LaneMarkingDetector.cpp \
          ./LaneMarkingFilterTest.cpp ../Code/software/apps/LaneMarkingFilter.cpp \
	  ./DriverBehaviorAnalysisTest.cpp ../Code/software/apps/DriverBehaviorAnalysis.cpp \
          ../Code/software/apps/RANSACEstimator.cpp ../Code/software/apps/Line.cpp \
          ../Code/software/apps/ConnectedComponentDetector.cpp \
          ../Code/software/apps/CameraGeometry.cpp \
          ../Code/software/apps/Lane.cpp \
          ../Code/software/apps/Util.cpp ../Code/software/lib/Timer.cpp

HDRS = $(wildcard *.h) $(wildcard ../Code/software/apps/*.h) $(wildcard ../Code/software/apps/*.hh)

OBJECTS = $(notdir $(SOURCES:.cpp=.o))

DEPS = $(OBJECTS:.o=.d)
-include $(DEPS)

all: check

$(TARGET): Makefile $(OBJECTS) $(HDRS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

.SECONDEXPANSION:
PERCENT = %

$(OBJECTS): %.o : $$(filter $$(PERCENT)/$$(notdir %).cpp, $(SOURCES))
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	$(CXX) -MM $(CXXFLAGS) $< > $*.d

check: Makefile $(TARGET)
	./$(TARGET) > $(TARGET).log

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPS) *.gcda *.gcno
