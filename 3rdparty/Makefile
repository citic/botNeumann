all: llvm

llvm:
	# LLVM
	wget -c http://llvm.org/releases/3.6.0/llvm-3.6.0.src.tar.xz
	tar xf llvm-3.6.0.src.tar.xz
	ln -s llvm-3.6.0.src llvm

	# Clang
	wget -c http://llvm.org/releases/3.6.0/cfe-3.6.0.src.tar.xz
	tar xf cfe-3.6.0.src.tar.xz
	mv cfe-3.6.0.src/ llvm/tools/clang

	# Clang tools extra
	wget -c http://llvm.org/releases/3.6.0/clang-tools-extra-3.6.0.src.tar.xz
	tar xf clang-tools-extra-3.6.0.src.tar.xz
	mv clang-tools-extra-3.6.0.src/ llvm/tools/clang/tools/extra

	# C++ standard library (LibC++)
	# wget -c http://llvm.org/releases/3.6.0/libcxx-3.6.0.src.tar.xz
	# wget -c http://llvm.org/releases/3.6.0/libcxxabi-3.6.0.src.tar.xz
	# tar xf libcxx-3.6.0.src.tar.xz
	# tar xf libcxxabi-3.6.0.src.tar.xz
	# mv libcxx-3.6.0.src llvm/projects/libcxx
	# mv libcxxabi-3.6.0.src llvm/projects/libcxxabi


	# Compile
	mkdir -p llvm/bin
	LLVM_BIN=`pwd`/llvm/bin
	mkdir -p llvm/build
	cd llvm/build
	# disable documents to save space
	# unsure: --enable-optimized --enable-expensive-checks --enable-debug-runtime --enable-debug-symbols --enable-embed-stdcxx
	../configure --prefix=$(LLVM_BIN) --enable-libcpp --enable-cxx1y --enable-jit --disable-docs --enable-threads --enable-pthreads --enable-shared --enable-targets=host,x86,x86_64,arm,arm64,cpp
	time make -j3 && make install
	# run the tests?
	make check-libcxx

clean:
	rm -rf *.xz llvm*
