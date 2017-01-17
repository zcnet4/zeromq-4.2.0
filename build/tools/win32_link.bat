
@echo off

set yres_dir=D:\yy_code\yworld\yres
set yscripts_dir=D:\yy_code\yworld\yscripts
set yworld_dir=D:\yy_code\yworld\yworld

echo %yres_dir%
echo %yscripts_dir%
echo %yworld_dir%

if exist "%yworld_dir%\server\gateway_Debug" (
	del %yworld_dir%\server\gateway_Debug\gateway.conf
	mklink  %yworld_dir%\server\gateway_Debug\gateway.conf %yres_dir%\server\gateway.conf
	del %yworld_dir%\server\gateway_Debug\log.conf
	mklink  %yworld_dir%\server\gateway_Debug\log.conf     %yres_dir%\server\log.conf

)  

if exist "%yworld_dir%\server\dbserver_Debug" (
	del %yworld_dir%\server\dbserver_Debug\dbserver.conf 
	mklink  %yworld_dir%\server\dbserver_Debug\dbserver.conf %yres_dir%\server\dbserver.conf 

	del %yworld_dir%\server\dbserver_Debug\log.conf
	mklink  %yworld_dir%\server\dbserver_Debug\log.conf      %yres_dir%\server\log.conf

)  

 
if exist "%yworld_dir%\server\ymapserver_Debug" (
	rmdir /s/q %yworld_dir%\server\ymapserver_Debug\config 
	copy /Y  %yres_dir%\common %yres_dir%\server
	mklink  /D %yworld_dir%\server\ymapserver_Debug\config %yres_dir%\server

	rmdir /s/q %yworld_dir%\server\ymapserver_Debug\scripts
	mklink  /D %yworld_dir%\server\ymapserver_Debug\scripts %yscripts_dir%\server 

)  
 
if exist "%yworld_dir%\client\yworldclient_Debug" (
	rmdir /s/q %yworld_dir%\client\res 
	copy /Y  %yres_dir%\common %yres_dir%\client\config
	copy /Y  %yres_dir%\server\gamedata.dat %yres_dir%\client\config\gamedata.dat
	mklink  /D %yworld_dir%\client\res %yres_dir%\client

)  
 
if exist "%yworld_dir%\server\gateway_Release" (
	del %yworld_dir%\server\gateway_Release\gateway.conf
	mklink  %yworld_dir%\server\gateway_Release\gateway.conf %yres_dir%\server\gateway.conf
	del %yworld_dir%\server\gateway_Release\log.conf
	mklink  %yworld_dir%\server\gateway_Release\log.conf %yres_dir%\server\log.conf

)  

if exist "%yworld_dir%\server\dbserver_Release" (
	del %yworld_dir%\server\dbserver_Release\dbserver.conf 
	mklink  %yworld_dir%\server\dbserver_Release\dbserver.conf %yres_dir%\server\dbserver.conf 

	del %yworld_dir%\server\dbserver_Release\log.conf
	mklink  %yworld_dir%\server\dbserver_Release\log.conf %yres_dir%\server\log.conf

)  

 
if exist "%yworld_dir%\server\ymapserver_Release" (
	rmdir /s/q %yworld_dir%\server\ymapserver_Release\config 
	copy /Y  %yres_dir%\common %yres_dir%\server
	mklink  /D %yworld_dir%\server\ymapserver_Release\config %yres_dir%\server

	rmdir /s/q %yworld_dir%\server\ymapserver_Release\scripts
	mklink  /D %yworld_dir%\server\ymapserver_Release\scripts %yscripts_dir%\server 

) 

pause