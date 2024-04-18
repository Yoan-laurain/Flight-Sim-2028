
Vous pouvez créer un fichier batch moveresources.bat pour effectuer cette tâche. Voici un exemple de script qui réalise ce que vous avez demandé :

batch
Copy code
@echo off
setlocal

set "sourceDir=res"
set "destinationDir=build\OpenGLProject\res"

echo Copie des fichiers depuis %sourceDir% vers %destinationDir%...

if not exist "%destinationDir%" (
    mkdir "%destinationDir%"
)

xcopy /E /Y /I "%sourceDir%\*" "%destinationDir%\"

echo Opération terminée.
pause