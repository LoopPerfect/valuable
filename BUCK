load('//:subdir_glob.bzl', 'subdir_glob')
load('//:buckaroo_macros.bzl', 'buckaroo_deps_from_package')

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

gtest = \
  buckaroo_deps_from_package('github.com/buckaroo-pm/google-googletest')

cxx_test(
  name = 'test',
  srcs = glob([
    'valuable/tests/*.cpp'
  ]),
  deps = [
    ':valuable',
  ] + gtest,
)
