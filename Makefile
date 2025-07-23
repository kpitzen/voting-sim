# Variables
BUILD_DIR := build
SRC_DIRS := src include tests
FORMAT_EXT := hpp cpp
CLANG_TIDY := clang-tidy
CLANG_FORMAT := clang-format
CMAKE := cmake

# Compiler flags (optional override)
CXXFLAGS := -std=c++20

# File discovery
FORMAT_FILES = \
    $(wildcard src/**/*.cpp) \
    $(wildcard src/**/*.hpp) \
    $(wildcard include/**/*.cpp) \
    $(wildcard include/**/*.hpp) \
    $(wildcard tests/**/*.cpp) \
    $(wildcard tests/**/*.hpp)


TIDY_FILES = $(shell find src tests -name '*.cpp')


# Targets
.PHONY: all format tidy build test clean check-format

all: build

check-format:
	@echo "🔍 Checking format (dry-run)..."
	@diff_output=$$(clang-format --dry-run --Werror $(FORMAT_FILES) 2>&1); \
	if [ -n "$$diff_output" ]; then \
		echo "$$diff_output"; \
		echo "❌ Formatting issues found. Run 'make format' to fix."; \
		exit 1; \
	else \
		echo "✅ All files properly formatted."; \
	fi

format:
	@echo "🧽 Running clang-format..."
	@if [ -z "$(FORMAT_FILES)" ]; then \
		echo "No files to format."; \
	else \
		$(CLANG_FORMAT) -i $(FORMAT_FILES); \
	fi

tidy:
	@echo "🔍 Running clang-tidy..."
	@SDKROOT=$$(xcrun --show-sdk-path); \
	for file in $(shell find src include tests -type f \( -name '*.cpp' -o -name '*.hpp' \)); do \
		echo "Linting $$file"; \
		clang-tidy $$file \
			-p build \
			--extra-arg=-isysroot \
			--extra-arg=$$SDKROOT \
			-header-filter='^$(realpath $(CURDIR))' \
			--quiet || exit 1; \
	done

build:
	@echo "🛠️  Configuring and building..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. && $(CMAKE) --build .

test:
	@echo "🧪 Running tests..."
	@cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	@echo "🧹 Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

check-tidy:
	@echo "🔎 Running clang-tidy (check only)..."
	@SDKROOT=$$(xcrun --show-sdk-path); \
	failed=0; \
	for file in $(TIDY_FILES); do \
		echo "Checking $$file"; \
		clang-tidy $$file \
			-p build \
			--extra-arg=-isysroot \
			--extra-arg=$$SDKROOT \
			-header-filter='^$(realpath $(CURDIR))' \
			--quiet || failed=1; \
	done; \
	if [ $$failed -eq 0 ]; then \
		echo "✅ clang-tidy check passed."; \
	else \
		echo "⚠️  clang-tidy check reported issues."; \
	fi

check: check-tidy check-format
