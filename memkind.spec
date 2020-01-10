%global gittag0 v1.5.0

Name: memkind
Summary: User Extensible Heap Manager
Version: 1.5.0
Release: 1%{?checkout}%{?dist}
License: BSD
Group: System Environment/Libraries
URL: http://memkind.github.io/memkind
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: automake libtool numactl-devel systemd

# x86_64 is the only arch memkind will build and work due to
# its current dependency on SSE4.2 CRC32 instruction which
# is used to compute thread local storage arena mappings
# with polynomial accumulations via GCC's intrinsic _mm_crc32_u64
# For further info check: 
# - /lib/gcc/<target>/<version>/include/smmintrin.h
# - https://gcc.gnu.org/bugzilla/show_bug.cgi?id=36095 
# - http://en.wikipedia.org/wiki/SSE4
ExclusiveArch: x86_64

Source0: https://github.com/%{name}/%{name}/archive/%{gittag0}/%{name}-%{version}.tar.gz

%description
The memkind library is an user extensible heap manager built on top of
jemalloc which enables control of memory characteristics and a
partitioning of the heap between kinds of memory.  The kinds of memory
are defined by operating system memory policies that have been applied
to virtual address ranges. Memory characteristics supported by
memkind without user extension include control of NUMA and page size
features. The jemalloc non-standard interface has been extended to
enable specialized arenas to make requests for virtual memory from the
operating system through the memkind partition interface. Through the
other memkind interfaces the user can control and extend memory
partition features and allocate memory while selecting enabled
features. This software is being made available for early evaluation.
Feedback on design or implementation is greatly appreciated.

%package devel
Summary: Memkind User Extensible Heap Manager development lib and tools
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Install header files and development aids to link memkind library 
into applications. The memkind library is an user extensible heap manager 
built on top of jemalloc which enables control of memory characteristics and
heap partitioning on different kinds of memory. This software is being made 
available for early evaluation. The memkind library should be considered 
pre-alpha: bugs may exist and the interfaces may be subject to change prior to 
alpha release. Feedback on design or implementation is greatly appreciated.

%prep
%setup -q -a 0 -n %{name}-%{version}

%build
# It is required that we configure and build the jemalloc subdirectory
# before we configure and start building the top level memkind directory.
# To ensure the memkind build step is able to discover the output
# of the jemalloc build we must create an 'obj' directory, and build
# from within that directory.
cd %{_builddir}/%{name}-%{version}/jemalloc/
echo %{version} > %{_builddir}/%{name}-%{version}/jemalloc/VERSION
test -f configure || %{__autoconf}
mkdir %{_builddir}/%{name}-%{version}/jemalloc/obj
ln -s %{_builddir}/%{name}-%{version}/jemalloc/configure \
      %{_builddir}/%{name}-%{version}/jemalloc/obj/
cd %{_builddir}/%{name}-%{version}/jemalloc/obj
%configure --enable-autogen --with-jemalloc-prefix=jemk_ --enable-memkind \
           --enable-safe --enable-cc-silence --prefix=%{_prefix} \
	   --without-export --disable-stats --disable-fill \
	   --disable-valgrind --disable-experimental\
           --includedir=%{_includedir} --libdir=%{_libdir} \
           --bindir=%{_bindir} --docdir=%{_docdir}/%{name} \
           --mandir=%{_mandir} CFLAGS="$RPM_OPT_FLAGS -std=gnu99"

%{__make} %{?_smp_mflags}

# Build memkind lib and tools
cd %{_builddir}/%{name}-%{version}
echo %{version} > %{_builddir}/%{name}-%{version}/VERSION
touch %{_builddir}/%{name}-%{version}/jemalloc/.git
test -f configure || ./autogen.sh
%configure --enable-tls --prefix=%{_prefix} --libdir=%{_libdir} \
           --includedir=%{_includedir} --sbindir=%{_sbindir} \
           --mandir=%{_mandir} --docdir=%{_docdir}/%{name} \
           CFLAGS="$RPM_OPT_FLAGS -std=gnu99"
%{__make} %{?_smp_mflags}

%install
cd %{_builddir}/%{name}-%{version}
%{__make} DESTDIR=%{buildroot} install
make install DESTDIR=%{buildroot} INSTALL='install -p'
rm -f %{buildroot}/%{_libdir}/lib%{name}.{l,}a
rm -f %{buildroot}/%{_libdir}/lib{numakind,autohbw}.*
rm -f %{buildroot}/%{_docdir}/%{name}/VERSION

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/lib%{name}.so.*
%{_bindir}/%{name}-hbw-nodes
%dir %{_docdir}/%{name}
%doc %{_docdir}/%{name}/README
%license %{_docdir}/%{name}/COPYING

%files devel
%defattr(-,root,root,-)
%dir %{_includedir}/%{name}
%dir %{_includedir}/%{name}/internal/
%{_includedir}/%{name}/internal/*.h
%{_includedir}/%{name}*.h
%{_includedir}/hbwmalloc.h
%{_includedir}/hbw_allocator.h
%{_libdir}/lib%{name}.so
%{_mandir}/man3/hbwmalloc.3.*
%{_mandir}/man3/hbwallocator.3.*
%{_mandir}/man3/%{name}*.3.*

%changelog
* Mon Mar 27 2017 Rafael Aquini <aquini@linux.com> - 1.5.0-1
- Update memkind source file to 1.5.0 upstream

* Fri Feb 17 2017 Rafael Aquini <aquini@linux.com> - 1.4.0-1
- Update memkind source file to 1.4.0 upstream

* Fri Feb 10 2017 Fedora Release Engineering <releng@fedoraproject.org> - 1.3.0-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_26_Mass_Rebuild

* Wed Nov 16 2016 Rafael Aquini <aquini@linux.com> - 1.3.0-1
- Update memkind source file to 1.3.0 upstream

* Wed Jun 08 2016 Rafael Aquini <aquini@linux.com> - 1.1.0-1
- Update memkind source file to 1.1.0 upstream

* Thu Mar 17 2016 Rafael Aquini <aquini@linux.com> - 1.0.0-1
- Update memkind source file to 1.0.0 upstream

* Sun Feb 07 2016 Rafael Aquini <aquini@linux.com> - 0.3.0-5
- Fix rpmlint error dir-or-file-in-var-run for /var/run/memkind

* Sat Feb 06 2016 Rafael Aquini <aquini@linux.com> - 0.3.0-4
- Update upstream fixes for memkind-0.3.0
- Switch old init.d scripts for systemd unit service
- Fix fc24 build error

* Thu Feb 04 2016 Fedora Release Engineering <releng@fedoraproject.org> - 0.3.0-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_24_Mass_Rebuild

* Tue Nov 17 2015 Rafael Aquini <aquini@linux.com> - 0.3.0-2
- Minor clean-ups and adjustments required for the RPM

* Tue Nov 17 2015 Rafael Aquini <aquini@linux.com> - 0.3.0-1
- Update memkind source file to 0.3.0 upstream

* Wed Jun 17 2015 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 0.2.2-4.20150525git
- Rebuilt for https://fedoraproject.org/wiki/Fedora_23_Mass_Rebuild

* Mon May 25 2015 Rafael Aquini <aquini@linux.com> - 0.2.2-3.20150525git
- Get rid of obsolete m4 macros usage on autotool scripts

* Mon May 18 2015 Rafael Aquini <aquini@linux.com> - 0.2.2-2.20150525git
- Fix to BuildRequires and License Text Marker in spec file (1222709#c1)

* Mon May 18 2015 Rafael Aquini <aquini@linux.com> - 0.2.2-1.20150518git
- Initial RPM packaging for Fedora
