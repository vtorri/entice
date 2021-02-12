edc_files = [
  'default'
]

foreach src : edc_files
  custom_target(src + '.edj',
    input        : src + '.edc',
    output       : src + '.edj',
    command      : [ edje_cmd,
                     '-id', join_paths(meson.current_source_dir(), 'images'),
                     '@INPUT@', '@OUTPUT@' ],
    install_dir  : join_paths(dir_pkgdata, 'themes'),
    install_mode : 'rw-r--r--',
    install      : true,
  )
endforeach
