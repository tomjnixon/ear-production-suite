install(FILES
        EAR-Production-Suite_Session-Template.RPP
        README.md
        DESTINATION
        .)
install(TARGETS
        project_upgrade
        project_upgrade_gui
        DESTINATION
        tools)

get_property(PACKAGE_TARGETS_REAPEREXT GLOBAL PROPERTY PACKAGE_TARGETS_REAPEREXT)
get_property(PACKAGE_TARGETS_OTHERVST GLOBAL PROPERTY PACKAGE_TARGETS_OTHERVST)
get_property(PACKAGE_TARGETS_EARVST GLOBAL PROPERTY PACKAGE_TARGETS_EARVST)

foreach(tgt ${PACKAGE_TARGETS_REAPEREXT})
	if(TARGET ${tgt})
		install(FILES
			$<TARGET_FILE:${tgt}>
			DESTINATION
			Extension/UserPlugins)
	endif()
endforeach()
foreach(tgt ${PACKAGE_TARGETS_OTHERVST})
	if(TARGET ${tgt})
		install(FILES
			$<TARGET_FILE:${tgt}>
			DESTINATION
			VST3)
	endif()
endforeach()
foreach(tgt ${PACKAGE_TARGETS_EARVST})
	if(TARGET ${tgt})
		install(FILES
			$<TARGET_FILE:${tgt}>
			DESTINATION
			VST3/ear-production-suite)
	endif()
endforeach()

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "EAR Production Suite")
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "EBU")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.TXT")
set(CPACK_MONOLITHIC_INSTALL ON)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
