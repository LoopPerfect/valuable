include_defs('//BUCKAROO_DEPS')

cxx_library(
  name = 'valuable',
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

