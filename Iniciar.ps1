Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

$base = Split-Path -Parent $MyInvocation.MyCommand.Path
$core = Join-Path $base 'LoaderCore.exe'
$dll = Join-Path $base 'sys32.dll'

$form = New-Object System.Windows.Forms.Form
$form.Text = 'MTA | Inicializador'
$form.Size = New-Object System.Drawing.Size(620, 390)
$form.StartPosition = 'CenterScreen'
$form.BackColor = [System.Drawing.Color]::FromArgb(22, 27, 38)
$form.ForeColor = [System.Drawing.Color]::White
$form.Font = New-Object System.Drawing.Font('Segoe UI', 10)

$title = New-Object System.Windows.Forms.Label
$title.Text = 'Inicializador do MTA'
$title.Location = New-Object System.Drawing.Point(22, 17)
$title.Size = New-Object System.Drawing.Size(540, 35)
$title.Font = New-Object System.Drawing.Font('Segoe UI', 17, [System.Drawing.FontStyle]::Bold)
$form.Controls.Add($title)

$info = New-Object System.Windows.Forms.Label
$info.Text = 'sys32.dll deve estar na mesma pasta que LoaderCore.exe.'
$info.Location = New-Object System.Drawing.Point(25, 65)
$info.Size = New-Object System.Drawing.Size(560, 28)
$form.Controls.Add($info)

$button = New-Object System.Windows.Forms.Button
$button.Text = 'Iniciar MTA'
$button.Location = New-Object System.Drawing.Point(25, 104)
$button.Size = New-Object System.Drawing.Size(160, 39)
$button.BackColor = [System.Drawing.Color]::FromArgb(47, 132, 211)
$button.FlatStyle = 'Flat'
$form.Controls.Add($button)

$log = New-Object System.Windows.Forms.TextBox
$log.Multiline = $true
$log.ReadOnly = $true
$log.ScrollBars = 'Vertical'
$log.Location = New-Object System.Drawing.Point(25, 160)
$log.Size = New-Object System.Drawing.Size(555, 170)
$log.BackColor = [System.Drawing.Color]::FromArgb(13, 17, 23)
$log.ForeColor = [System.Drawing.Color]::FromArgb(214, 226, 235)
$form.Controls.Add($log)

$timer = New-Object System.Windows.Forms.Timer
$timer.Interval = 500
$script:process = $null
$timer.Add_Tick({
    if ($script:process -and $script:process.HasExited) {
        $timer.Stop()
        $output = $script:process.StandardOutput.ReadToEnd()
        $errors = $script:process.StandardError.ReadToEnd()
        $log.AppendText($output + $errors + [Environment]::NewLine)
        if ($script:process.ExitCode -eq 0) {
            $log.AppendText('Processo finalizado. O retorno nao confirma que a DLL foi carregada.' + [Environment]::NewLine)
        } else {
            $log.AppendText('Inicializador terminou com erro: ' + $script:process.ExitCode + [Environment]::NewLine)
        }
        $script:process.Dispose()
        $script:process = $null
        $button.Enabled = $true
    }
})

$button.Add_Click({
    if (-not (Test-Path -LiteralPath $core -PathType Leaf)) {
        [System.Windows.Forms.MessageBox]::Show('LoaderCore.exe nao encontrado nesta pasta.','Arquivo ausente') | Out-Null
        return
    }
    if (-not (Test-Path -LiteralPath $dll -PathType Leaf)) {
        [System.Windows.Forms.MessageBox]::Show('sys32.dll nao encontrada nesta pasta.','Arquivo ausente') | Out-Null
        return
    }
    $log.Clear()
    $log.AppendText('Iniciando o loader original adaptado...' + [Environment]::NewLine)
    try {
        $start = New-Object System.Diagnostics.ProcessStartInfo
        $start.FileName = $core
        $start.WorkingDirectory = $base
        $start.UseShellExecute = $false
        $start.CreateNoWindow = $true
        $start.RedirectStandardOutput = $true
        $start.RedirectStandardError = $true
        $script:process = [System.Diagnostics.Process]::Start($start)
        $button.Enabled = $false
        $timer.Start()
    } catch {
        $log.AppendText($_.Exception.Message + [Environment]::NewLine)
    }
})
$form.Add_FormClosed({ $timer.Stop() })
[void]$form.ShowDialog()
