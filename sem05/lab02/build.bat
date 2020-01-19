masm %1,prog.obj,,;
link prog.obj,,,;

@echo off
del *.CRF >nul
del *.LST >nul
del *.MAP >nul
del *.OBJ >nul
