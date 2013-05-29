FUNCTION(AddConfigFiles TAG CONFS)
    get_target_property(PLUGIN_LOCATION "${TAG}" LOCATION)
    get_filename_component(PLUGIN_DEST_DIR ${PLUGIN_LOCATION} PATH)
    FOREACH(conf_file ${CONFS})
        get_filename_component(target ${conf_file} NAME)
        add_custom_command(
            TARGET "${TAG}" POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS
                -E
                copy_if_different
                ${conf_file}
                ${PLUGIN_DEST_DIR}/config/${target}
            COMMENT "Copying Plugin configuration file to the runtime folder."
        )
    ENDFOREACH(conf_file ${CONFS})
    
    install(FILES ${CONFS} DESTINATION bin/config)
ENDFUNCTION(AddConfigFiles TAG CONFFILES)