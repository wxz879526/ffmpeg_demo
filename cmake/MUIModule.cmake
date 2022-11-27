macro(updateDependentResIfNec TargetName OutputDir)
  add_custom_command(TARGET ${TargetName} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OutputDir}/mui/)
	
  find_program(LUPDATE_EXECUTABLE lupdate)
  find_program(LRELEASE_EXECUTABLE lrelease)
  find_program(RCC_EXECUTABLE rcc)
  
  set(TEMP_ROOT_DIR ${CMAKE_SOURCE_DIR}/SD4dstc)
  
  add_custom_command(TARGET ${TargetName} PRE_BUILD
        COMMAND ${LUPDATE_EXECUTABLE} -silent -noobsolete -locations none -target-language en_US -recursive ${TEMP_ROOT_DIR}/ideation/ ${TEMP_ROOT_DIR}/irender/ ${TEMP_ROOT_DIR}/public/ ${TEMP_ROOT_DIR}/shared/ -ts ${TEMP_ROOT_DIR}/mui/language/en_US.ts)
  add_custom_command(TARGET ${TargetName} PRE_BUILD
        COMMAND ${LUPDATE_EXECUTABLE} -silent -noobsolete -locations none -target-language zh_CN -recursive ${TEMP_ROOT_DIR}/ideation/ ${TEMP_ROOT_DIR}/irender/ ${TEMP_ROOT_DIR}/public/ ${TEMP_ROOT_DIR}/shared/ -ts ${TEMP_ROOT_DIR}/mui/language/zh_CN.ts)

  add_custom_command(TARGET ${TargetName} PRE_BUILD
        COMMAND ${LRELEASE_EXECUTABLE} ${TEMP_ROOT_DIR}/mui/language/en_US.ts -qm ${TEMP_ROOT_DIR}/mui/images/en_US.qm)
  add_custom_command(TARGET ${TargetName} PRE_BUILD
        COMMAND ${LRELEASE_EXECUTABLE} ${TEMP_ROOT_DIR}/mui/language/zh_CN.ts -qm ${TEMP_ROOT_DIR}/mui/images/zh_CN.qm)

  add_custom_command(TARGET ${TargetName} PRE_BUILD
        COMMAND ${RCC_EXECUTABLE} -binary ${TEMP_ROOT_DIR}/mui/images/res.qrc -o ${OutputDir}/mui/res.rcc)

endmacro(updateDependentResIfNec)