BIN_DIR     ?= build
BUILD_TYPE  ?= Debug
BUILD_DIR   ?= build/$(BUILD_TYPE)
INSTALL_DIR ?= installation


all: build
.PHONY: all

configure:
	cmake -B $(BUILD_DIR) -Wno-dev -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

debug:   BUILD_TYPE = debug
release: BUILD_TYPE = release
build release debug: configure	
	cmake --build $(BUILD_DIR)
.PHONY: build release debug

LINTFLAGS := --style=google
lint:     LINTFLAGS += -n
lint_fix: LINTFLAGS += -i
lint lint_fix:
	clang-format ${LINTFLAGS} $(shell find src/app src/lib/src\
	                                      -type f \
																				-name '*.h'   -or -name '*.c' -or \
                                        -name '*.hpp' -or -name '*.cpp')
.PHONY: lint lint_fix

install: build
	cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR)
.PHONY: install

uninstall: build
	cmake --build $(BUILD_DIR) --target uninstall
	rm -rf $(INSTALL_DIR)
.PHONY: uninstall

clean:
	rm -rf build $(INSTALL_DIR)
	rm -rf *.tar* *.sh
	rm -rf _CPack_Packages
.PHONY: clean

dvi:
	texi2dvi readmi.texi --clean
.PHONY: dvi

docs: configure
	cmake --build $(BUILD_DIR) --target docs
.PHONY: docs

dist: configure
	cpack --config $(BUILD_DIR)/CPackSourceConfig.cmake
.PHONY: dist

bdist: configure
	cpack --config $(BUILD_DIR)/CPackConfig.cmake
.PHONY: bdist

test: configure
	cmake --build $(BUILD_DIR) --target run_tests
.PHONY: test

gcov_report: configure
	cmake --build $(BUILD_DIR) --target coverage
.PHONY: gcov_report


# 3Dmodels: https://disk.yandex.com/d/9L_EsXPKEMzZFQ
