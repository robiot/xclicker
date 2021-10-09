xclicker_resources = gnome.compile_resources(
    'xclicker-resources',
    join_paths(
        'resources', 'xclicker.gresource.xml'
    ),
    source_dir: 'resources'
)

executable('xclicker',
    'main.c', 
    'xclicker-app.c',
    'mainwin.c',
    'x11api.c',
    'settings.c',
  
    xclicker_resources,
    dependencies: gtkdeps,
)
