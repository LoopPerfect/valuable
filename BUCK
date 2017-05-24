include_defs('//BUCKAROO_DEPS')

prebuilt_cxx_library(
  name = 'valuable',
  header_only = True,
  header_namespace = 'valuable',
  exported_headers = subdir_glob([
    ('valuable/include', '**/*.hpp'),
  ])
)

cxx_test(
  name = 'test',
  srcs = glob(['valuable/tests/*.cpp']),
  deps = [':valuable']
)

