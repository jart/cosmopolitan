Win32/README.txt
27 June 2008

The resource files zip.rc and windll.rc must not get edited and saved from
MS Visual Studio.  MS VS automatically re-adds its specific MFC-related resource
infrastructure to the "xx.rc" files when saved after any modification.  The
dependancies on MFC related headers break the compilation process, when you
try to use the freely available MS Visual Studio Express Editions (2005 or 2008)
for building Zip.  And, most third-party compilers also lack support for the
propietary MFC environment.
