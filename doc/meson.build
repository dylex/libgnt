DOC_MODULE = 'libgnt'

# Header files or dirs to ignore when scanning. Use base file/dir names
ignore_headers = [
    'test',
    'wms',
    'gntinternal.h',
    'gnt-skel.h',
    'gntboxprivate.h',
    'gntmainprivate.h',
    'gntmenuprivate.h',
    'gntmenuitemprivate.h',
    'gntstyleprivate.h',
    'gnttreeprivate.h',
    'gntwidgetprivate.h',
    'gntwmprivate.h',
    'gntwsprivate.h',
]

# Extra options to supply to gtkdoc-scan.
scan_args = [
    '--deprecated-guards=GNT_DISABLE_DEPRECATED',
    '--rebuild-types',
    '--rebuild-sections',
]

configure_file(
    input : 'version.xml.in',
    output : 'version.xml',
    configuration : gnt_config)

gnome.gtkdoc(DOC_MODULE,
    main_xml : DOC_MODULE + '-docs.xml',
    src_dir : libgnt_inc,
    dependencies : libgnt_dep,
    install : true,
    ignore_headers : ignore_headers,
    scan_args : scan_args,
    gobject_typesfile : DOC_MODULE + '.types')
