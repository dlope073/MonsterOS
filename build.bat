echo off
REM Copyright (c) 2014, Daniel Lopez
REM All rights reserved.

REM Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

REM Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
REM Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
REM THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
echo on
cd source 
nasm -f bin boot.asm -o ..\bin\boot.bin 
nasm -f bin os.asm -o ..\bin\os.bin -l ..\bin\os.lst
cd programs
nasm -f bin ver.asm -o ..\..\programs\ver.com  -l ..\..\programs\ver.lst
nasm -f bin dbug.asm -o ..\..\programs\dbug.com 
cd ..
cd ..
cd bin
copy /b boot.bin + os.bin img.bin
cd ..
echo off
REM Remember to change these paths according to your machine
echo on
MonsterOS-VHD-Builder C:\Users\user\Documents\GitHub\MonsterOS\vbox\MonsterOS\hdisk.vhd  C:\Users\user\Documents\GitHub\MonsterOS\bin\img.bin C:\Users\user\Documents\GitHub\MonsterOS\bin\file-table.bin
pause