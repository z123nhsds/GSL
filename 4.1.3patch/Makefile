# 一键验证 patch:
#   make          # 用 g++ 分别编译 trigger.cc（应失败）和 fixed.cc（应通过）
#   make CXX=clang++  # 改用 clang++

CXX ?= g++
CXXFLAGS = -std=c++17 -Werror -Wall -Wextra

.PHONY: all
all: check-trigger check-fixed

check-trigger: trigger.cc
	@echo "--- 编译 trigger.cc（预期 FAIL: 'unknown attribute suppress'） ---"
	-$(CXX) $(CXXFLAGS) -c $< -o /tmp/gsl_trigger.o 2>&1
	@echo ""

check-fixed: fixed.cc
	@echo "--- 编译 fixed.cc  （预期 PASS） ---"
	$(CXX) $(CXXFLAGS) -c $< -o /tmp/gsl_fixed.o 2>&1
	@echo "OK"
	@echo ""

.PHONY: clean
clean:
	rm -f /tmp/gsl_trigger.o /tmp/gsl_fixed.o
