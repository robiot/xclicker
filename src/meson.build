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
    'config.c',
    'mainwin.c',
    'x11api.c',
    'settings.c',
    'utils.c',
    xclicker_resources,
    dependencies: deps,
)
