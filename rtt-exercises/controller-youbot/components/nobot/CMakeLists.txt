#
# Components, types and plugins.
#
# The CMake 'target' names are identical to the first argument of the
# macros below, except for orocos_typegen_headers, where the target is fully
# controlled by generated code of 'typegen'.
#

# Creates a component library libnobot-<target>.so
# and installs in the directory lib/orocos/nobot/
#
orocos_component(nobot nobot-component.hpp nobot-component.cpp) # ...you may add multiple source files
#
# You may add multiple orocos_component statements.

#
# Additional headers:
#
# Installs in the include/orocos/nobot/ directory
#
orocos_install_headers( nobot-component.hpp ) # ...you may add multiple header files
#
# You may add multiple orocos_install_headers statements.

#
# Generates and installs our package. Must be the last statement such
# that it can pick up all above settings.
#
orocos_generate_package()
