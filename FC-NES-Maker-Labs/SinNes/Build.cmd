@Echo off
Echo %1 �������Ŀ¼
Echo %2 �ǹ�������

set myDir=%1
set myDir=%myDir:"=%
set mypj=%2
set mypj=%mypj:"=%
set Path=%myDir%_APP\_cc65\bin

Echo Path=%Path%
REM goto start 
echo ׼��sinnes.lib
cd sinnes\lib\

del runtime\*.o
del sinnes\*.o
cd runtime
for %%f in (*.s) do ca65 -t nes %%f
cd ..
cd sinnes
for %%f in (*.s) do ca65 -t nes %%f
cd ..
del *.lib
ar65 a sinnes.lib runtime\*.o
ar65 a sinnes.lib sinnes\*.o
ar65 l sinnes.lib >sinneslist.txt

cd ..
cd ..
:start
copy sinnes\lib\sinnes.lib .\nes.lib
copy sinnes\lib\sinnes\nes.inc .\nes.inc
copy sinnes\lib\sinnes\zeropage.inc .\zeropage.inc
copy sinnes\include\sinnes.h .\sinnes.h

echo ׼��sinnes.lib���



Echo .
Echo ��һ����ɾ��ģ����NES
del *.o
del *.nes
ar65 d nes.lib ctr0.o

Echo .
Echo �ڶ���������Ӳ��ģ�飬����ӵ���
ca65 -t nes ctr0.s
ar65 a nes.lib ctr0.o

Echo .
Echo ������������C�������ӿ⣬����nes
echo on
cl65 -C nes.cfg -t nes -o "%mypj%.nes" HelloWorld.c

@Echo off
Echo .
pause