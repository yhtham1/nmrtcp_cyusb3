//ＵＴＦ８
20/11/18(水) 11:54:48

Mingw-w64-gccでコンパイルすること。32bit版, 64bit版 両方対応
https://sourceforge.net/projects/mingw-w64/

注意点
	./lua53/下も合わせてmakeする。

#
# cd lua53
# make clean
# make generic
# cd ..
# make clean
# make


