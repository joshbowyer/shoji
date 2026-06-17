project('shoji',
  'c',
  version : '0.0.1',
  license : 'TBD',
  meson_version : '>=0.60.0',
  default_options : ['warning_level=2', 'c_std=c11'],
)

# Dependencies
gtk_dep = dependency('gtk4', version : '>=4.0')
adwaita_dep = dependency('libadwaita-1', version : '>=1.0')
glib_dep = dependency('glib-2.0', version : '>=2.0')

# Sources
sources = files('src/main.c')

# Install the CSS so the installed binary can find it
css = files('data/style.css')
install_data(css, install_dir : get_option('datadir') / 'shoji' / 'css')

# The binary
executable('shoji', sources,
  dependencies : [gtk_dep, adwaita_dep, glib_dep],
  install : true,
  c_args : ['-Wno-unused-parameter'],
)
