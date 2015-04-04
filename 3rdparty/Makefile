# If version > 3.6.0, version 
LLVM_VERSION=3.4.2

# Compresion of files, xz if version >= 3.5
CLANG_EXT=gz


all: llvm

llvm:
	# LLVM
	wget -c http://llvm.org/releases/$(LLVM_VERSION)/llvm-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	tar xf llvm-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	ln -s llvm-$(LLVM_VERSION).src llvm

	# Clang (cfe = Clang FrontEnd)
	wget -c http://llvm.org/releases/$(LLVM_VERSION)/cfe-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	tar xf cfe-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	mv cfe-$(LLVM_VERSION).src/ llvm/tools/clang

	# Clang tools extra
	wget -c http://llvm.org/releases/3.4/clang-tools-extra-3.4.src.tar.$(CLANG_EXT)
	tar xf clang-tools-extra-3.4.src.tar.$(CLANG_EXT)
	mv clang-tools-extra-3.4/ llvm/tools/clang/tools/extra

	# C++ standard library (LibC++)
	# wget -c http://llvm.org/releases/$(LLVM_VERSION)/libcxx-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	# wget -c http://llvm.org/releases/$(LLVM_VERSION)/libcxxabi-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	# tar xf libcxx-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	# tar xf libcxxabi-$(LLVM_VERSION).src.tar.$(CLANG_EXT)
	# mv libcxx-$(LLVM_VERSION).src llvm/projects/libcxx
	# mv libcxxabi-$(LLVM_VERSION).src llvm/projects/libcxxabi


	# Compile
	mkdir -p llvm/bin
	LLVM_BIN=$(PWD)/llvm/bin	
	mkdir -p llvm/build
	cd llvm/build
	# disable documents to save space
	# unsure: --enable-optimized --enable-expensive-checks --enable-debug-runtime --enable-debug-symbols --enable-embed-stdcxx
	../configure --prefix=$(LLVM_BIN) --enable-libcpp --enable-cxx1y --enable-jit --disable-docs --enable-threads --enable-pthreads --enable-shared --enable-targets=host,x86,x86_64,arm,cpp
	time make -j3 && make install
	# run the tests?
	make check-libcxx

clean:
	rm -rf *.$(CLANG_EXT) llvm*
