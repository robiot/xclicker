project(
    'XClicker',
    'c',
    license: 'GPL-3.0',
    default_options: [
        'warning_level=2'
    ],
)

add_project_arguments(
    '-Wno-incompatible-pointer-types',
    language : 'c'
)


gnome = import('gnome')
deps = [dependency('gtk+-3.0'), dependency('x11'), dependency('xi'), dependency('xtst')]

subdir('src')
