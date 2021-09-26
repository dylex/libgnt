# UPDATING VERSION NUMBERS FOR RELEASES
#
# The version number is:
#   <major>.<minor>.<micro><suffix>
#
# If any code has changed in libgnt:
#   micro += 1
#
# If any functions have been added to libgnt:
#   micro = 0
#   minor += 1
#
# If backwards compatibility has been broken in libgnt:
#   micro = 0
#   minor = 0
#   major += 1
#   gnt_soversion += 1
#
# suffix should be similar to one of the following:
#   For beta releases:          '-beta2'
#   For code under development: '-devel'
#   For production releases:    ''
#

# the last version for Finch 2 was 2.14.2,
# the first version for Finch 3 was 3.0.0

project('libgnt', 'c',
    version : '2.14.3',
    meson_version : '>=0.44.0')
gnt_soversion = 0

parts = meson.project_version().split('-')
if parts.length() > 1
  gnt_extra_version = parts[1]
else
  gnt_extra_version = ''
endif

parts = parts[0].split('.')
gnt_major_version = parts[0]
gnt_minor_version = parts[1]
gnt_micro_version = parts[2]

gnt_config = configuration_data()
gnt_config.set('GNT_MAJOR_VERSION', gnt_major_version)
gnt_config.set('GNT_MINOR_VERSION', gnt_minor_version)
gnt_config.set('GNT_MICRO_VERSION', gnt_micro_version)
gnt_config.set_quoted('GNT_EXTRA_VERSION', gnt_extra_version)
gnt_config.set_quoted('GNT_VERSION', meson.project_version())

compiler = meson.get_compiler('c')
pkgconfig = import('pkgconfig')

# #######################################################################
# # Check for GLib 2.16
# #######################################################################
glib = dependency('glib-2.0', version : '>= 2.16.0')
gobject = dependency('gobject-2.0')
gmodule = dependency('gmodule-2.0')
gnome = import('gnome')

#######################################################################
# Check for LibXML2
#######################################################################
libxml = dependency('libxml-2.0', version : '>= 2.6.0', required : false)
gnt_config.set('NO_LIBXML', not libxml.found())

#######################################################################
# Check for ncurses and other things used by it
#######################################################################
ncurses_available = true
ncurses_inc = []
# The order of this list is important to the condition that follows.
ncurses_libs = [
	compiler.find_library('ncursesw', required : false),
	compiler.find_library('panelw', required : false),
	compiler.find_library('tinfow', required : false),
]
if not ncurses_libs[0].found() or not ncurses_libs[1].found()
	ncurses_available = false
endif

if host_machine.system() == 'windows'
	# FIXME: $host ?
	ncurses_sys_prefix = '/usr/$host/sys-root/mingw'
else
	ncurses_sys_prefix = '/usr'
endif

ncurses_sys_dirs = [ncurses_sys_prefix + '/include/ncursesw',
                    ncurses_sys_prefix + '/include']

if ncurses_available
	# Some distros put the headers in ncursesw/, some don't
	found_ncurses_h = false
	foreach location : ncurses_sys_dirs
		f = location + '/ncurses.h'
		if not found_ncurses_h
			if compiler.has_header_symbol(f, 'get_wch',
			    prefix : '#define _XOPEN_SOURCE_EXTENDED')
				if location != '.'
					ncurses_inc += [include_directories(location)]
				endif
				found_ncurses_h = true
			endif
		endif
	endforeach

	if not found_ncurses_h
		ncurses_inc = []
		ncurses_libs = []
		ncurses_available = false
	endif
else
	# ncursesw was not found. Look for plain old ncurses
	# The order of this list is important to the condition that follows.
	ncurses_libs = [
		compiler.find_library('ncurses', required : false),
		compiler.find_library('panel', required : false),
		compiler.find_library('tinfo', required : false),
	]
	ncurses_available = ncurses_libs[0].found() and ncurses_libs[1].found()
	gnt_config.set('NO_WIDECHAR', true)
endif
if not ncurses_available
	error('ncurses could not be found!')
endif

ncurses = declare_dependency(
    include_directories : ncurses_inc,
    dependencies : ncurses_libs
)

libgnt_SOURCES = [
	'gntwidget.c',
	'gntbindable.c',
	'gntbox.c',
	'gntbutton.c',
	'gntcheckbox.c',
	'gntclipboard.c',
	'gntcolors.c',
	'gntcombobox.c',
	'gntentry.c',
	'gntfilesel.c',
	'gntkeys.c',
	'gntlabel.c',
	'gntline.c',
	'gntmenu.c',
	'gntmenuitem.c',
	'gntmenuitemcheck.c',
	'gntprogressbar.c',
	'gntslider.c',
	'gntstyle.c',
	'gnttextview.c',
	'gnttree.c',
	'gntutils.c',
	'gntwindow.c',
	'gntwm.c',
	'gntws.c',
	'gntmain.c'
]

libgnt_headers = [
	'gntwidget.h',
	'gntbindable.h',
	'gntbox.h',
	'gntbutton.h',
	'gntcheckbox.h',
	'gntclipboard.h',
	'gntcolors.h',
	'gntcombobox.h',
	'gntentry.h',
	'gntfilesel.h',
	'gntkeys.h',
	'gntlabel.h',
	'gntline.h',
	'gntmenu.h',
	'gntmenuitem.h',
	'gntmenuitemcheck.h',
	'gntprogressbar.h',
	'gntslider.h',
	'gntstyle.h',
	'gnttextview.h',
	'gnttree.h',
	'gntutils.h',
	'gntwindow.h',
	'gntwm.h',
	'gntws.h',
	'gnt.h'
]

# Check for Python headers
python_dep = disabler()
if get_option('python2')
	python_dep = dependency('python2', required : false)
	if not python_dep.found()
		python_dep = dependency('python-2.7', required : false)
	endif

	if not python_dep.found()
		error('failed to find python')
	endif
endif
gnt_config.set('USE_PYTHON', get_option('python2'))

configure_file(output : 'gntconfig.h',
    configuration : gnt_config)

install_headers(libgnt_headers, subdir : 'gnt')

if host_machine.system() == 'windows'
	libgnt_SOURCES += windows.compile_resources('libgnt_winres.rc')
endif

gntmarshal = gnome.genmarshal('gntmarshal',
                              sources : 'genmarshal',
                              prefix : 'gnt_closure_marshal',
                              install_header : true,
                              install_dir : join_paths(get_option('includedir'), 'gnt'))

libgnt_inc = include_directories('.')
libgnt_dependencies= [ncurses, libxml, glib, gobject, gmodule]
if get_option('python2')
	libgnt_dependencies += python_dep
endif
libgnt = library('gnt',
                 libgnt_SOURCES + gntmarshal,
                 install : true,
                 version : '@0@.@1@.@2@'.format(gnt_soversion,
                                                gnt_minor_version,
                                                gnt_micro_version),
                 dependencies : libgnt_dependencies)
libgnt_dep = declare_dependency(
    include_directories : libgnt_inc,
    link_with : libgnt,
    dependencies : [ncurses, glib])

if meson.version().version_compare('>=0.54.0')
	meson.override_dependency('gnt', libgnt_dep)
endif

pkgconfig.generate(
    name : 'LibGNT',
    description : 'Glib Ncurses Toolkit is a collection of curses-widgets.',
    version : meson.project_version(),
    filebase : 'gnt',
    subdirs : 'gnt',
    libraries : [libgnt],
    requires : ['glib-2.0'],
    variables : ['plugindir = ${libdir}/gnt'],
    )

subdir('wms')
subdir('test')
subdir('doc')
