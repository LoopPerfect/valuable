prebuilt_cxx_library(
  name = 'valuable',
  header_only = True,
  header_namespace = 'valuable',
  exported_headers = subdir_glob([
    ('valuable/include', '**/*.hpp'),
  ]),
  visibility = [
    'PUBLIC',
  ],
)

cxx_test(
  name = 'test',
  srcs = glob([
    'valuable/tests/*.cpp'
  ]),
  platform_compiler_flags = [
    ('^macos.*', [ '-std=c++14' ]),
  ],
  deps = [
    ':valuable',
  ],
)
