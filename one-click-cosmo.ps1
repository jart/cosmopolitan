# § - "meRead" - as a 'mini-Readme,' in the script itself - §

# Header Metadata
# **Script Name**: Cosmopolitan Libc Setup Script  
# **Author**: LouminAI Labs - louminai.com  
# **Creation Date**: 2024-08-09  
# **Last Modified Date**: 2024-08-09  
# **Version**: 1.0.0  
# **Dependencies**:
# - Windows 11
# - PowerShell 5.1 or higher
# - Git (via Chocolatey)
# - Visual Studio Build Tools
# - WSL (Windows Subsystem for Linux)
# - Windows Terminal (via Chocolatey)

# **Description**:  
# This PowerShell script automates the setup, compilation, and execution of a sample Cosmopolitan Libc program on Windows 11. It ensures all necessary dependencies are installed, configures the environment in WSL, and compiles a "Hello, World!" program using Cosmopolitan Libc.

# **Usage**:
# ```powershell
# Run the script with administrative privileges
# powershell -ExecutionPolicy Bypass -File cosmopolitan_setup.ps1
# ```

# **Contact Information**:  
# Email: opensource@louminai.com  
# Url: louminai.com  
# Github-url: https://github.com/LouminAILabs/OpenSource-lAIl  
# License: MIT

# ---

# Script Overview and Purpose
# This PowerShell script provides a one-click solution to automate the entire process of setting up, compiling, and running a sample Cosmopolitan Libc program on Windows 11. The script handles the installation of dependencies, setup of the development environment within WSL, and compilation of a basic "Hello, World!" C program using Cosmopolitan Libc. The goal is to streamline the development process and ensure that the program can be executed seamlessly across different platforms.

# Linkages and Backlinks
# - Cosmopolitan Libc: [Cosmopolitan Libc Website](https://justine.lol/cosmopolitan/index.html)
# - Related Documentation: [Cosmopolitan API Documentation](https://justine.lol/cosmopolitan/documentation.html)

# Integration Pointers
# - The script integrates with Windows 11's PowerShell and WSL to create a seamless development environment for Cosmopolitan Libc.
# - Outputs a compiled "Hello, World!" program that can run natively on multiple platforms.
# - Automates the interaction between PowerShell and Bash (via WSL) to ensure proper environment setup and compilation.

# Prerequisites and Dependencies
# - Requires Windows 11 with PowerShell 5.1 or higher.
# - Dependencies include Git, Visual Studio Build Tools, WSL, and Windows Terminal.
# - Installation of Git and Windows Terminal is automated through Chocolatey if not already present.
# - Visual Studio Build Tools and WSL are installed if not detected.

# Versioning Details
# - Script Version: 1.0.0
# - Last Updated: 2024-08-09
# - Initial release with all required features for automating Cosmopolitan Libc setup on Windows 11.

# Additional Elements
# **Configuration**:
# - The script automatically checks for and installs dependencies.
# - Environment variables such as `COSMOPOLITAN_DIR` are set up in WSL to streamline compilation and execution.
# **Adaptive Behavior**:
# - The script detects whether dependencies are already installed and skips unnecessary steps to optimize the process.
# - If WSL is already configured, the script adapts by focusing only on Cosmopolitan Libc setup and program compilation.

# Example Usage in Production
# ```powershell
# Execute the script in PowerShell with administrative privileges
# powershell -ExecutionPolicy Bypass -File cosmopolitan_setup.ps1

# Expected Output:
# Checking for Git...
# Git is already installed.
# Checking for Visual Studio Build Tools...
# Visual Studio Build Tools is already installed.
# Checking for WSL...
# WSL is already installed.
# Checking for Windows Terminal...
# Windows Terminal is already installed.
# Cosmopolitan Libc setup and execution completed. Check WSL output for the result.
# ```

# Ultimate One-Click PowerShell Script for Cosmopolitan Libc on Windows 11
# This script automates the entire process of setting up, compiling, and running Cosmopolitan Libc.
# Run this script with administrative privileges in Windows PowerShell.

# Step 1: Function to Check and Install Dependencies
function Install-Dependency {
    param (
        [string]$PackageName,
        [string]$InstallCommand,
        [string]$CheckCommand,
        [string]$DownloadUrl
    )

    Write-Host "Checking for $PackageName..."

    if (-not (Invoke-Expression $CheckCommand)) {
        Write-Host "$PackageName not found. Installing..."
        if ($InstallCommand) {
            Invoke-Expression $InstallCommand
        } elseif ($DownloadUrl) {
            $Installer = "$env:TEMP\$PackageName.exe"
            Invoke-WebRequest -Uri $DownloadUrl -OutFile $Installer
            Start-Process -FilePath $Installer -ArgumentList "/silent" -Wait
        }
    } else {
        Write-Host "$PackageName is already installed."
    }
}

# Step 2: Pre-checks and Installation of Required Tools
$tools = @(
    @{ Name = "Git" ; Install = "choco install git -y"; Check = "git --version"; Url = "https://gitforwindows.org/" },
    @{ Name = "Visual Studio Build Tools"; Install = ""; Check = "cl.exe"; Url = "https://aka.ms/vs/17/release/vs_BuildTools.exe" },
    @{ Name = "WSL"; Install = "wsl --install"; Check = "wsl -l"; Url = "" },
    @{ Name = "Windows Terminal"; Install = "choco install microsoft-windows-terminal -y"; Check = "Get-AppxPackage -Name Microsoft.WindowsTerminal"; Url = "https://aka.ms/terminal" }
)

foreach ($tool in $tools) {
    Install-Dependency -PackageName $tool.Name -InstallCommand $tool.Install -CheckCommand $tool.Check -DownloadUrl $tool.Url
}

# Step 3: Set Up Cosmopolitan Environment in WSL
$wslScript = @"
#!/bin/bash
# WSL Script to Set Up Cosmopolitan Environment

# Install wget and unzip if not present
sudo apt-get update
sudo apt-get install -y wget unzip

# Create directory for Cosmopolitan compiler
COSMOPOLITAN_DIR=\$HOME/cosmocc
mkdir -p \$COSMOPOLITAN_DIR
cd \$COSMOPOLITAN_DIR

# Download and unzip Cosmopolitan compiler
wget -q https://cosmo.zip/pub/cosmocc/cosmocc.zip
unzip -o cosmocc.zip

# Add cosmocc to PATH
if ! grep -q "\`$COSMOPOLITAN_DIR" ~/.bashrc; then
    echo 'export PATH=\`$PATH:\`$COSMOPOLITAN_DIR' >> ~/.bashrc
fi
source ~/.bashrc

# Create a sample hello world program
cat << 'EOF' > hello.c
#include <stdio.h>

int main() {
    printf("hello world\\n");
    return 0;
}
EOF

# Compile the hello world program
cosmocc -o hello.exe hello.c

# Run the compiled program
./hello.exe

"@

$wslScriptPath = "$env:TEMP\cosmopolitan_setup.sh"
$wslScript | Out-File -FilePath $wslScriptPath -Encoding ASCII

# Step 4: Execute the Script in WSL
wsl bash $wslScriptPath

Write-Host "Cosmopolitan Libc setup and execution completed. Check WSL output for the result."
