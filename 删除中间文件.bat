copy MDK-ARM\CX100\CX2200*.hex   MDK-ARM\
rd /Q /S MDK-ARM\RTE
rd /Q /S MDK-ARM\DebugConfig
rd /Q /S MDK-ARM\CX100
del /Q MDK-ARM\*.bak
del /Q MDK-ARM\*.lst
del /Q MDK-ARM\CX_Config.uvguix.*
