#
# Intel Hex Format library
# Copyright (C) 2018 flavio santes, flavio dot santes at 1byt3 dot com
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

CFLAGS +=		\
	-Wall		\
	-Wextra		\
	-Werror		\
	-std=c89	\
	-pedantic	\
	-O3		\
	-g		\
	-Iinclude

TARGET = lib/libihexfmt.a
SAMPLE = bin/sample

OBJS = obj/ihexfmt.o

DIRS = obj bin lib

all: $(DIRS) $(TARGET) $(SAMPLE)

$(DIRS):
	mkdir -p $(DIRS)

$(TARGET): include/ihexfmt.h $(OBJS)
	ar r $@ $(OBJS)
	ranlib $@

$(SAMPLE): obj/sample.o $(TARGET)
	$(CC) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf bin/* obj/* lib/*

.PHONY: all clean
