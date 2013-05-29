FUNCTION(InstallSDK TAG HEADERS)
    SET(SDK_DEST_DIR ${CMAKE_SOURCE_DIR}/sdk)
    FOREACH(conf_file ${HEADERS})
        get_filename_component(target ${conf_file} NAME)
        add_custom_command(
            TARGET "${TAG}" POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS
                -E
                copy_if_different
                ${conf_file}
                ${SDK_DEST_DIR}/${target}
            COMMENT "Copying Plugin custom sdk headers to the sdk folder."
        )
    ENDFOREACH(conf_file ${CONFS})
    
    #install(FILES ${CONFS} DESTINATION bin/config)
ENDFUNCTION(InstallSDK TAG HEADERS)