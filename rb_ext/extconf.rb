require 'mkmf'

dir_config("ming")
if have_header("ming.h") and have_library("ming", "newSWFMovie")
	create_makefile("mingc")
end
