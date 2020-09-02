# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS main.c options.c osmolation.c tcp_process.c xpl_process.c)
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
