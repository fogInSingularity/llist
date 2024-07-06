CC = g++
EXE = llist

DIRS = lib listcode

SOURCES = $(foreach dir, $(DIRS), $(wildcard $(dir)/source/*.cpp))
HEADERS = $(foreach dir, $(DIRS), $(wildcard $(dir)/include/*.h))

INCLUDE = -I./lib/include -I./listcode/include

DEFINES = -D_DEBUG
FLAGS = -O0 -std=c++17 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
-Wc++14-compat -Wuseless-cast -Wmissing-declarations -Wcast-align -Wcast-qual \
-Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy \
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor \
-Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self \
-Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
-Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn \
-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override \
-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code \
-Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix \
-Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast \
-Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation \
-fstack-protector -fstrict-overflow -flto-odr-type-merging \
-fno-omit-frame-pointer -Wstack-usage=8192 \
-fsanitize=address,bool,bounds,enum,float-cast-overflow,$\
float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,$\
object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
undefined,unreachable,vla-bound,vptr
# -Wlarger-than=8192

all:
	@$(CC) $(INCLUDE) $(SOURCES) $(FLAGS) $(DEFINES) -o $(EXE)

release:
	@$(CC) $(INCLUDE) $(SOURCES) -O2 -std=c++17 -Wall -Wextra $(DEFINES) -o $(EXE)

clean:
	@rm llist

analyze:
	@clang-tidy $(SOURCES) -checks=clang-analyzer-*

performance:
	@clang-tidy $(SOURCES) -checks=performance-*

portability:
	@clang-tidy $(SOURCES) -checks=portability-*

readablility:
	@clang-tidy $(SOURCES) -checks=readability-$(READ_FLAGS)

