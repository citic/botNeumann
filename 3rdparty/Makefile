# If version > 3.6.0, version 
LLVM_VERSION=3.6.0

# Compresion of files, xz if version >= 3.5
CLANG_EXT=xz

LLVM_BIN=$(PWD)/llvm/bin

# Add here other 3rd party libraries
all: build-llvm

# Compile LLVM, Clang and LLDB
build-llvm: llvm llvm/tools/clang llvm/tools/lldb
	# Compile
	mkdir -p llvm/bin
	mkdir -p llvm/build
	# disable documents to save space
	# unsure: --enable-optimized --enable-expensive-checks --enable-debug-runtime --enable-debug-symbols --enable-embed-stdcxx
	cd llvm/build && ../configure --prefix=$(LLVM_BIN) --enable-libcpp --enable-cxx1y --enable-jit --disable-docs --enable-threads --enable-pthreads --enable-shared --enable-targets=host,x86,x86_64,arm,cpp
	cd llvm/build && time make -j1
	cd llvm/build && make install
	# run the tests?
	#cd llvm/build && make check

# LLVM extract
llvm: llvm-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	tar xf llvm-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	#ln -s llvm-$(LLVM_VERSION).src llvm
	mv llvm-$(LLVM_VERSION).src llvm

# Clang extract (cfe = Clang FrontEnd)
llvm/tools/clang: cfe-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	tar xf cfe-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	mv cfe-$(LLVM_VERSION).src/ llvm/tools/clang

# LLDB (debugger) extract
llvm/tools/lldb: lldb-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	tar xf lldb-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	mv lldb-$(LLVM_VERSION).src/ llvm/tools/lldb

# Module download
%.src.tar.$(CLANG_EXT):
	wget -c http://llvm.org/releases/$(LLVM_VERSION)/$@

ignored:
	# Clang tools extra
	#wget -c http://llvm.org/releases/3.4/clang-tools-extra-3.4.src.tar.$(CLANG_EXT)
	#tar xf clang-tools-extra-3.4.src.tar.$(CLANG_EXT)
	#mv clang-tools-extra-3.4/ llvm/tools/clang/tools/extra

	# C++ standard library (LibC++)
	# wget -c http://llvm.org/releases/$(LLVM_VERSION)/libcxx-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	# wget -c http://llvm.org/releases/$(LLVM_VERSION)/libcxxabi-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	# tar xf libcxx-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	# tar xf libcxxabi-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	# mv libcxx-$(LLVM_VERSION).src llvm/projects/libcxx
	# mv libcxxabi-$(LLVM_VERSION).src llvm/projects/libcxxabi

clean:
	rm -rf *.$(CLANG_EXT) llvm*
