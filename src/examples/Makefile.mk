#
#  Copyright (C) 2014 - 2016 Intel Corporation.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright notice(s),
#     this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright notice(s),
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) ``AS IS'' AND ANY EXPRESS
#  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
#  EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
#  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

noinst_PROGRAMS += examples/hello_memkind \
                   examples/hello_memkind_debug \
                   examples/hello_hbw \
                   examples/filter_memkind \
                   examples/stream \
                   examples/stream_memkind \
                   examples/new_kind \
                   examples/pmem_memkind \
                   examples/gb_realloc \
                   examples/numakind_test \
                   examples/autohbw_candidates \
                   # end
if ENABLE_CXX11
noinst_PROGRAMS += examples/memkind_allocated
endif


lib_LTLIBRARIES += examples/libnumakind.la \
                   examples/libautohbw.la \
                   # end

EXTRA_DIST += examples/autohbw_get_src_lines.pl

examples_hello_memkind_LDADD = libmemkind.la
examples_hello_memkind_debug_LDADD = libmemkind.la
examples_hello_hbw_LDADD = libmemkind.la
examples_filter_memkind_LDADD = libmemkind.la
examples_stream_LDADD = libmemkind.la
examples_stream_memkind_LDADD = libmemkind.la
examples_new_kind_LDADD = libmemkind.la
examples_pmem_memkind_LDADD = libmemkind.la
examples_gb_realloc_LDADD = libmemkind.la
examples_numakind_test_LDADD = examples/libnumakind.la libmemkind.la
examples_autohbw_candidates_LDADD = libmemkind.la
examples_libautohbw_la_LIBADD = libmemkind.la
examples_libautohbw_la_LDFLAGS = -version-info $(memkind_abi_version)

if ENABLE_CXX11
examples_memkind_allocated_LDADD = libmemkind.la
endif

examples_hello_memkind_SOURCES = examples/hello_memkind_example.c
examples_hello_memkind_debug_SOURCES = examples/hello_memkind_example.c examples/memkind_decorator_debug.c
examples_hello_hbw_SOURCES = examples/hello_hbw_example.c
examples_filter_memkind_SOURCES = examples/filter_example.c
examples_stream_SOURCES = examples/stream_example.c
examples_stream_memkind_SOURCES = examples/stream_example.c
examples_new_kind_SOURCES = examples/new_kind_example.c
examples_pmem_memkind_SOURCES = examples/pmem_example.c
examples_gb_realloc_SOURCES = examples/gb_realloc_example.c
examples_numakind_test_SOURCES = examples/numakind_test.c
examples_libnumakind_la_SOURCES = examples/numakind.c examples/numakind.h examples/numakind_macro.h
examples_libautohbw_la_SOURCES = examples/autohbw.c examples/autohbw_helper.h
examples_autohbw_candidates_SOURCES = examples/autohbw_candidates.c
noinst_HEADERS += examples/numakind.h examples/numakind_macro.h
if ENABLE_CXX11
examples_memkind_allocated_SOURCES = examples/memkind_allocated_example.cpp examples/memkind_allocated.hpp
endif

examples_stream_memkind_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) -DENABLE_DYNAMIC_ALLOC

examples_stream_memkind_CFLAGS = $(AM_CFLAGS) $(OPENMP_CFLAGS)
examples_stream_CFLAGS = $(AM_CFLAGS) $(OPENMP_CFLAGS)
examples_numakind_test_CFLAGS = $(AM_CFLAGS) $(OPENMP_CFLAGS)

NUMAKIND_MAX = 2048
examples_libnumakind_la_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) -DNUMAKIND_MAX=$(NUMAKIND_MAX)
CLEANFILES += examples/numakind_macro.h
examples/numakind.c: examples/numakind_macro.h
examples/numakind_macro.h:
	for ((i=0;i<$(NUMAKIND_MAX);i++)); do \
	  echo "NUMAKIND_GET_MBIND_NODEMASK_MACRO($$i)" >> $@; \
	done
	echo 'const struct memkind_ops NUMAKIND_OPS[NUMAKIND_MAX] = {' >> $@
	for ((i=0;i<$(NUMAKIND_MAX);i++)); do \
	  echo "NUMAKIND_OPS_MACRO($$i)," >> $@; \
	done
	echo '};' >> $@
	echo >> $@
