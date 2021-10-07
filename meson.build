project(
    'XClicker',
    'c',
    license: 'GPL-3.0',
    default_options: [
        'warning_level=2'
    ],
)

# add_project_arguments(
#     '-Wunused-command-line-argument',
#     language : 'c'
# )


gnome = import('gnome')
gtkdeps = [dependency('gtk+-3.0'), dependency('x11')]

subdir('src')

