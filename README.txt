
LCTT (Light CORBA Testing Tool)
v 0.0.4

Alat testira brzinu protoka CORBA komunikacionog kanala tako sto prilikom poziva objekta na serveru
sa servera dobija odredjenu kolicinu podataka, a pri tom se meri vreme potremno da se data kolicina
podataka primi.

Opsti oblik poziva programa je: 
	lcctt [opcije] [adresa_servera] 

Opcija:		Značenje:
-s		serverski režim (ako nema ove opcije, podrazumeva se klijentski režim). U ovom režimu nije potrebno zadavanje adrese servera.
-k<br_bajtova>	veličina komada (podrazomevana veličina je 1000 000 bajtova) 
-n<br_ponavljanja>	broj komada koji će biti preneseni u toku testa (podrazumevani broj komada je 1000) 
-t<br_ponavljanja>	broj testova (podrazumevani broj testova je 5) 
-h		pomoć, ispisuju se kratko uputstvo za korišćenje programa 
-T<br_sekundi>	Maksimalno moguće trajanje poziva metode (call timeout). Podrazumevano vreme je 2 sekunde. 
 -R<br_pokusaja>	Maksimalan broj pokušaja za ponovno uspostavljanje veze. Podrazumevana vrednost je 1. 
-X		Isključivanje obrade izuzetaka 


Za rad programa neophodan je omniORB 4.1.4
============================================
Kompajliranje omniORB-a (Linux)

i.	Za kompajliranje omniORB-a potrebni paketi:
		gcc, g++, python, python-dev
ii.	za gcc4.4+ potrebno je ispraviti stdio.h (“getline” u “parseline”)
iii. 	po kompajliranju kopirati <omni>/build/bin u ~/bin i <omni>/build/lib u ~/lib
iv. 	**(izmeniti .profile: u PATH dodati ~/bin a u LD_LIBRARY_PATH ~/lib)

Instalacija i pokretanje (Linux)

v. 	napraviti linkove u lctt direktorijumu:
		1. lib -> <omni>/build/lib
		2. include -> <omni>/build/include
		3. iskopirati iz <omni>/include u <omni>/build/include
		foldere omniORB4, omnithread i fajl omnithread.h
vi. 	iskompajlirati lctt (make)
vii. 	promeniti adrese u "ctt_start"
viii.	 napraviti direktorijum "log" za omniNames
ix. 	pokrenuti "omniNames -start -logdir log"
x.	parametri za pokretanje lctt (ctt_start):
		 -ORBInitRef "NameService=corbaname::${NAMES_IP1},:${NAMES_IP2}"
			(lokacija NameService-a)
		 -ORBendPoint "giop:tcp:${OBJ_IP1}:" -ORBendPoint "giop:tcp:${OBJ_IP2}:"
			(koje adrese koristi i gde slusa)
		 -ORBverifyObjectExistsAndType 0 -ORBresetTimeOutOnRetries 1
			(potrebno za uspesan FT mreze)
--------------------------------------------------------------------------

kompajliranje omniORB-a (Windows)

instalirati cygwin (ili gnu-win32-lite.zip u npr c:\gnuwin32) i dodati bin direktorijum u path

-otpakovati sors omniORB-a
-otpakovati omniPython u isti direktorijum

-izabrati platformu u <top>/config/config.mk
-izabrati putanju do omniPython-a u <top>/mk/platforms/x86_win32_vs_8.mk
(otkomentarisati liniju "PYTHON = $(ABSTOP)/$(BINDIR)/omnipython")

-pokrenuti vcvarsall.bat
-u <top>/src direktorijumu pokrenuti make export


Instalacija i pokretanje (Windows)

-pokrenuti "vcvarsall.bat"
-iskopirati "<omni>/include" direktorijum u lokalni "include" direktorijum
-iskopirati "<omni>/bin/x86_win32" direktorijum negde u PATH (zbog dll-ova)
-iz "<omni>/lib/x86_win32" iskopirati omniORB412_rt.lib omniDynamic412_rt.lib omnithread33_rt.lib u lokalni "lib" direktorijum
- kompajlirati (make -f wMakefile)
--------------------------------------------------------------------------

Kompajliranje i pokretanje lctt-a
(Linux)
1. Raspakovati arhivu 
2. U direktorijumu lctt napraviti linkove "lib" i "include"  omniORB/build/lib i omniORB/build/include direktorijuma 
3. ***Iskopirati sadrzaj omniORB/include direktorijuma u omniORB/build/include i uradi ponovo configure omniORB-a*** smisli kako da se uradi drugacije
4. U lctt direktorijumu pokrenuti "make"
5. Pre pokretanja postaviti LD_LIBRARY_PATH promenljivu na omniORB/build/lib direktorijum




