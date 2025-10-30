src=src/*.c
CFLAGS="$CFLAGS -Wall -Wextra -Iinclude -fPIC"
LDFLAGS="-shared"
if [[ ! -v CC ]]; then
	CC=gcc
fi
if [[ ! -v LD ]]; then
	LD=gcc
fi


mkdir -p obj

obj=""

for i in $src
do
	$CC -c $i -o obj/$(basename ${i%.c}.o) $CFLAGS
	obj="$obj"' '"obj/$(basename ${i%.c}.o)"
	echo "compiling $i..."
done

$LD -o libonion.so $LDFLAGS $obj
