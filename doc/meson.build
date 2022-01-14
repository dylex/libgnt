if get_option('doc')

DOC_MODULE = 'libgnt'

# Header files or dirs to ignore when scanning. Use base file/dir names
ignore_hfiles = [
    'test',
    'wms',
    'gntinternal.h',
    'gnt-skel.h'
]

# Extra options to supply to gtkdoc-scan.
scan_args = [
    '--deprecated-guards=GNT_DISABLE_DEPRECATED',
    '--rebuild-types',
    '--rebuild-sections',
    '--ignore-headers=' + ' '.join(ignore_hfiles),
]

# Extra options to supply to gtkdoc-mkdb.
mkdb_args = [
    '--ignore-files=' + ' '.join(ignore_hfiles),
]

configure_file(
    input : 'version.xml.in',
    output : 'version.xml',
    configuration : gnt_config)

gnome.gtkdoc(DOC_MODULE,
    main_xml : DOC_MODULE + '-docs.xml',
    namespace : 'gnt',
    src_dir : libgnt_inc,
    dependencies : libgnt_dep,
    install : true,
    scan_args : scan_args,
    mkdb_args : mkdb_args,
    gobject_typesfile : DOC_MODULE + '.types')

endif # get_option('doc')

