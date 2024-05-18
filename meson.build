project('zaolib', 'cpp')

inc = include_directories('include')
zaolib = static_library(
    'zaolib',
    sources: [
        'src/device.cpp',
        'src/Z2.cpp'
    ],
    dependencies: [
        dependency('hidapi')
    ],
    include_directories : inc,
    install : false
)

zaolib_dep = declare_dependency(
    include_directories: inc,
    link_with: [
        zaolib
    ]
)
