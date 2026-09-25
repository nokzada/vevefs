Set fs = CreateObject("Scripting.FileSystemObject")
Set app = CreateObject("Shell.Application")
folder = fs.GetParentFolderName(WScript.ScriptFullName)
arguments = "-NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File " & Chr(34) & folder & "\Iniciar.ps1" & Chr(34)
app.ShellExecute "powershell.exe", arguments, folder, "runas", 0
