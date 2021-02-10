
edc_files = [
  'default.edc'
]

foreach edc_file : edc_files
  custom_target('edje_cc_' + edc_file,
    input       : edc_file,
    depfile     : '@BASENAME@.edj.d',
    output      : '@BASENAME@.edj',
    command     : edje_cc_exe +
                  [
                   '-id', join_paths(meson.current_source_dir(), 'images'),
                   '@INPUT@', '@OUTPUT@'
                  ],
    install     : true,
    install_dir : join_paths(dir_pkgdata, 'themes'),
  )
endforeach
